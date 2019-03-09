#include <iostream>
#include <future>
#include <thread>
#include <type_traits>
#include <chrono>
#include <ctime> 
template<typename Function, typename... Args>
auto async(Function func, Args&& ... args) {
    typedef typename std::result_of<Function(Args...)>::type result_type;
    typedef std::packaged_task<result_type()> task_type;
    auto bd = std::bind(func, std::forward<Args>(args)...);
    task_type task(bd);
    std::future<result_type> result = task.get_future();
    std::thread task_td(std::move(task));
    //detach() separates the thread of execution from the thread object, allowing execution to continue independently. 
    //so async() would return result without waiting for the result of the function called.
    task_td.detach();
    //Below is the output using detach():

    // Fib result: 5
    // Sleep 5s
    // Sleep 5s
    // 5
    // Time elapsed: 5.00556s.

    //We can see that time elapsed is only 5s, though we called async twice(but get() is called only once).



    //join() will wait for the result of the function called, so it is not suitable for async().

    //task_td.join();
    //If uncommented the line above, the output is below:
    // Fib result: 5
    // Sleep 5s
    // Sleep 5s
    // 5
    // Time elapsed: 10.006s.

    //We can see that time elaspsed is 10s, which means the thread waits for the result of the function.
    return result;
}
int fib(int n)
{
    if (n < 3) return 1;
    else return fib(n-1) + fib(n-2);
}
auto sleep = [](){
    std::cout << "Sleep 5s \n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 5;
};
int main(){
    auto f = async(fib, 5);
    std::cout << "Fib result: " << f.get() << "\n";
    auto start = std::chrono::system_clock::now();
    auto s = async(sleep);
    auto s2 = async(sleep);
    std::cout << s.get() << "\n";
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Time elapsed: " << elapsed_seconds.count() << "s.\n";
}