

Asynchronous programming in Qt/C++ using tasks and continuations.
========

The project seeks to do async based programming in Qt/C++ using modern C++.

This library wraps a function into a future where the result of the wrapped function
can be retrieved through the future's below public methods:

1. .get().  This method runs the wrapped function on the current thread
            and could block the thread and hang GUI. This API is useful when you are already
            in a background thread.

2. .then(). This method does three things:

            1. Registers a method to be called when a wrapped function finish running.

            2. Runs the wrapped function on a background thread.

            3. Runs the registered method on the current thread when the wrapped
               function finish running.

3. .await(). This method does three things:

            1. Suspends the current thread at a point where this method is called.

            2. Creates a background thread and then runs the wrapped function in the
               background thread.

            3. Unsuspends the current thread when the wrapped function finish and let
               the current thread continue normally.

            The suspension at step 1 is done without blocking the thread and hence the
            suspension can be done in the GUI thread and the GUI will remain responsive.

4. .queue(). This method runs tasks in a future sequentially and a passed in function will be called when all tasks
	     finish running. This method behaves like ```.then( [](){} )``` if the future is managing only one task.

5. .cancel(). This method can be used to cancel a future. It is important to know
              that this method does not terminate a running thread that is powering a future, it just
              releases memory used by a future and this method should be used if a future is to be discarded
	      after it it is acquired but never used. To terminate a thread,call .all_threads() method,locate a QThread
	      instance you want to terminate and call .terminate() method on the instance.

6. .all_threads(). This method returns a vector of QThreads that are powering futures.
                   The vector will contain a single entry if this future powers its own task. If this future
                   manages other futures,then the returned vector will contain QThread pointers that are in
                   the same order as tasks/futures passed to Task::run().

7. .start(). This method is to be used if a future is to be run without caring about its result.
	     Use this API if you want a future to run but dont want to use any of the above mentioned methods.

8. .manages_multiple_futures(). This method can be used to check if a future powers
                                its own task or manages other futures.

Examples of using a future.
========

**1. Example use of .get() method of a future.**

```c++

Task::future<int>& foo = bar() ;

int r = foo.get() ;

```

**2. Example use of .await() method of a future.**

```c++

Task::future<int>& foo = bar() ;

int r = foo.await() ;

```
**3. Example use an alternative use of .await() method of a future.**

```c++

int foo() ; //function prototype

int r = Task::await( foo ) ;

```

**4. Example use of .then() method of a future.**

```c++

void meaw( int ) ; //function prototype

Task::future<int>& foo = bar() ;

foo.then( meaw ) ;

```

**5. Example use of .queue() method of a future.**

```c++

void meaw() ; //function prototype

Task::future<int>& foo = bar() ;

foo.queue( meaw ) ;

```

Examples of creating a future.
========

**1. Creating a future that has no result.**
```c++

void bar() ; //function prototype

Task::future<void>& foo = Task::run( bar ) ;

```

**2. Creating a future that has result.**
```c++

int foo() ; //function prototype

Task::future<int>& foo = Task::run( foo ) ;

```

**3. Creating a future that combines multiple functions. .get() and .queue() on the future will cause passed in functions to run sequentially and in the order they are specified. .await() and .then() will cause passed in functions to run concurrently.**

```c++

void foo() ; //function prototype
void bar() ; //function prototype

Task::future<void>& foo = Task::run( foo,bar ) ;

```

**4. Creating a future that combines multiple tasks and their continuations that take no argument. .get() and .queue() on the future will cause passed in functions to run sequentially and in the order they are specified. .await() and .then() will cause passed in functions to run concurrently.**

```c++

void foo() ; //function prototype
void bar() ; //function prototype

void cfoo() ; //function prototype
void cbar() ; //function prototype

Task::future<void>& e = Task::run( Task::void_pair{ foo,cfoo },Task::void_pair{ bar,cbar } ) ;

```

**5. Creating a future that combines multiple tasks and their continuations that takes an argument. .get() and .queue() on the future will cause passed in pairs to run sequentially and in the order they are specified. .await() and .then() will cause passed in pairs to run concurrently. The result of the future is undefined and a function that takes no argument should be used if .await() method of the future is called.**

```c++

int foo() ; //function prototype
int bar() ; //function prototype

void cfoo( int ) ; //function prototype
void cbar( int ) ; //function prototype

Task::future<int>& e = Task::run( Task::pair<int>{ foo,cfoo },Task::pair<int>{ bar,cbar } ) ;
```

Further documentation of how to use the library is here[1] and here[2].

[1] https://github.com/mhogomchungu/tasks/blob/master/example.cpp

[2] https://github.com/mhogomchungu/tasks/blob/0bd4fd227aa4f13bba25dff74df06719ada6317d/task.h#L598
