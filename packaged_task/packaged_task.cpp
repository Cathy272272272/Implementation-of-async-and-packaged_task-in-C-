#include <iostream>
#include <future>
#include <thread>
#include <ctime> 
#include <cmath>

template <typename> class my_task;

template <typename R, typename ...Args>
class my_task<R(Args...)>
{
    std::function<R(Args...)> fn; //use std::function to store function and arguments
    std::promise<R> pr;             
public:
    template <typename ...Ts>
    explicit my_task(Ts &&... ts) : fn(std::forward<Ts>(ts)...) { }

    template <typename ...Ts>
    void operator()(Ts &&... ts)
    {
        //pass function and arguments
        pr.set_value(fn(std::forward<Ts>(ts)...));  
    }

    std::future<R> get_future() { return pr.get_future(); }

    // disable copy, default move
};
template<typename> class packaged_task;
template< class R, class ...Args > 
class packaged_task<R(Args...)>{

    std::promise<R> p;
    std::function<R(Args...)> fn; //use std::function to store function and arguments
    public:
        packaged_task() noexcept{}

        //pass function and arguments
        template <typename ...Ts>
        explicit packaged_task(Ts &&... ts) : fn(std::forward<Ts>(ts)...) { }
        
        //No copy
        packaged_task(packaged_task&) = delete;
        packaged_task& operator=(packaged_task&) = delete;

        //move
        packaged_task( packaged_task&& rhs ) noexcept{ swap(rhs); }
        packaged_task& operator=(packaged_task&& __other)
        {
          packaged_task(std::move(__other)).swap(*this);
          return *this;
        }
        
        //pass function and arguments
        template <typename ...Ts>
        void operator()(Ts &&... ts)
        {
            p.set_value(fn(std::forward<Ts>(ts)...)); 
        }

        std::future<R> get_future() 
        { 
            return p.get_future(); 
        }

        void swap(packaged_task& other){
            p.swap(other.p);
            fn.swap(other.fn);
        }

        bool valid() const{
            return static_cast<bool>(fn); 
        }
       void reset()
       {
            packaged_task(std::move(fn)).swap(*this);
       }
};
  
 
int main()
{
    packaged_task<int(int,int)> task([](int a, int b) {
        return std::pow(a, b);
    });
    packaged_task<int(int,int)> tmp;
    std::future<int> result = task.get_future();

    //Test valid()
    //Below are the output of the following code:

    // Test valid():
    // packaged_task task: 1
    // packaged_task tmp: 0

    //We can see that tmp(a empty packaged_task with no function and arguments specified) is not valid.
    std::cout << "Test valid(): \n";
    std::cout << "packaged_task task: " << task.valid() << "\n";
    std::cout << "packaged_task tmp: " << tmp.valid() << "\n";

    //Test reset()
    //Below are the output of the following code:

    // Test reset():
    // Before reset: 2^9 = 512
    // After reset: 2^10 = 1024

    //We can see that after reset, we can change the arguments.
    std::cout << "Test reset(): \nBefore reset: ";
    task(2, 9);
    std::cout << "2^9 = " << result.get() << '\n';
    std::cout << "After reset: ";
    task.reset();
    result = task.get_future();
    task(2, 10);
    std::cout << "2^10 = " << result.get() << '\n';

    //Test swap()

    //Below are the output of the following code:

    // Test swap():
    // If called get_future() before swap():
    // Result: 1024, Result2: 100
    // However, after swap, the function in eack task exchanged:
    // Result: 100, Result2: 1024
    // If called get_future() after swap():
    // Result: 1024, Result2: 100

    //The output shows the importance of the timing of calling get_future()
    // If called get_future() before swap(), the output shows that the results don't swap, that is because we retrive future before swap.
    // However, if we reset each task, and pass the same arguments again, we can see that results do swap(even if we don't call swap after reset).
    // Finally, we reset and then swap again, and call get_future after swap, then we can see the results are swapped.
    std::cout << "Test swap(): \n";
    task.reset();
    //difference between task and task2 is pow(a,b) and pow(b,a)
    packaged_task<int(int,int)> task2([](int a, int b) {
        return std::pow(b, a);
    });
    result = task.get_future();
    std::future<int> result2 = task2.get_future();
    task2.swap(task);
    task(2,10);
    task2(2,10);
    std::cout << "If called get_future() before swap(): \nResult: " << result.get() << ", Result2: " << result2.get() << "\n";
    task.reset();
    task2.reset();
    result = task.get_future();
    result2 = task2.get_future();
    task(2,10);
    task2(2,10);
    std::cout << "However, after swap, the function in eack task exchanged: \nResult: " << result.get() << ", Result2: " << result2.get() << "\n";
    task.reset();
    task2.reset();
    task2.swap(task);
    result = task.get_future();
    result2 = task2.get_future();
    task(2,10);
    task2(2,10);
    std::cout << "If called get_future() after swap(): \nResult: " << result.get() << ", Result2: " << result2.get() << "\n";

}
