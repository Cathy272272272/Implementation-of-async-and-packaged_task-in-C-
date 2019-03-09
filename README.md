# Implementation-of-async-and-packaged_task-in-C-
async:
async is a function that returns a future.
I use packaged_task to implement it.
The method is assign the future of each packaged_task to each async, one per thread. 
And use detach() to separate the thread of execution from the thread object, allowing execution to continue independently, so async() would return result without waiting for the result of the function called.

packaged_task:
packaged_task is a class that has multiple methods to play around future.
I use promise to implement packaged_task.
The main idea id to pass a function into promise, using set_value(), and get future using promise's get_future() method.

