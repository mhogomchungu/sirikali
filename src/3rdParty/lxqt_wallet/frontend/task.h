/*
 * copyright: 2014-2016
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __TASK_H_INCLUDED__
#define __TASK_H_INCLUDED__

#include <utility>
#include <future>
#include <functional>
#include <QThread>
#include <QEventLoop>

/*
 *
 * Examples on how to use the library are at the end of this file.
 *
 */

/*
 * This library wraps a function into a future where the result of the function
 * can be retrieved through the future's 3 public methods:
 *
 * 1. .get()   This method runs the wrapped function on the current thread
 *             and could block the thread and hang GUI.
 *
 * 2. .then()  This method does three things:
 *
 *             1. Registers a method to be called when a wrapped function finish running.
 *
 *             2. Runs the wrapped function on a background thread.
 *
 *             3. Runs the registered method on the current thread when the wrapped function finish
 *                running.
 *
 * 3. .await() This method does three things:
 *
 *             1. Suspends the current thread at a point where this method is called.
 *
 *             2. Creates a background thread and then runs the wrapped function in the background
 *                thread.
 *
 *             3. Unsuspends the current thread when the wrapped function finish and let the
 *                current thread continue normally.
 *
 *             The suspension at step 1 is done without blocking the thread and hence the suspension
 *             can be done in the GUI thread and the GUI will remain responsive.
 *
 * The future is of type "Task::future<T>&" and "std::reference_wrapper"[1]
 * class can be used if they are to be managed in a container that can not handle references.
 *
 * [1] http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
 */
namespace LXQt
{

namespace Wallet
{

namespace Task
{
class Thread : public QThread
{
    Q_OBJECT
public:
    Thread()
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
#else
        connect(this, &QThread::finished, this, &QThread::deleteLater);
#endif
    }
protected:
    virtual ~Thread()
    {
    }
private:
    virtual void run()
    {
    }
};

template< typename T >
class future
{
public:
    future(QThread *e,
           std::function< void() > && start,
           std::function< void() > && cancel,
           std::function< void(T &) > && get) :
        m_thread(e),
        m_start(std::move(start)),
        m_cancel(std::move(cancel)),
        m_get(std::move(get))
    {
    }
    void then(std::function< void(T) > function)
    {
        m_function = std::move(function);
        this->start();
    }
    T get()
    {
        T r;
        m_get(r);
        return r;
    }
    T await()
    {
        QEventLoop p;

        T q;

        m_function = [ & ](T && r) { q = std::move(r); p.exit(); };

        this->start();

        p.exec();

        return q;
    }
    QThread &thread()
    {
        return *m_thread;
    }
    void start()
    {
        m_start();
    }
    void cancel()
    {
        m_cancel();
    }
    void run(T && r)
    {
        m_function(std::move(r));
    }
private:
    QThread *m_thread;
    std::function< void(T) > m_function = [](T && t) { Q_UNUSED(t); };
    std::function< void() > m_start;
    std::function< void() > m_cancel;
    std::function< void(T &) > m_get;
};

template< typename T >
class ThreadHelper : public Thread
{
public:
    ThreadHelper(std::function< T() > && function) :
        m_function(std::move(function)),
        m_future(this,
                 [this]() { this->start(); },
                 [this]() { this->deleteLater(); },
                 [this](T &r) { r = m_function(); this->deleteLater(); })
    {
    }
    future<T>& Future()
    {
        return m_future;
    }
private:
    ~ThreadHelper()
    {
        m_future.run(std::move(m_result));
    }
    void run()
    {
        m_result = m_function();
    }
    std::function< T() > m_function;
    future<T> m_future;
    T m_result;
};

template<>
class future< void >
{
public:
    future(QThread *e ,
           std::function< void() > && start,
           std::function< void() > && cancel,
           std::function< void() > && get) :
        m_thread(e),
        m_start(std::move(start)),
        m_cancel(std::move(cancel)),
        m_get(std::move(get))
    {
    }
    void then(std::function< void() > function)
    {
        m_function = std::move(function);
        this->start();
    }
    void get()
    {
        m_get();
    }
    void await()
    {
        QEventLoop p;

        m_function = [ & ]() { p.exit(); };

        this->start();

        p.exec();
    }
    QThread &thread()
    {
        return *m_thread;
    }
    void start()
    {
        m_start();
    }
    void run()
    {
        m_function();
    }
    void cancel()
    {
        m_cancel();
    }
private:
    QThread *m_thread;
    std::function< void() > m_function = []() {};
    std::function< void() > m_start;
    std::function< void() > m_cancel;
    std::function< void() > m_get;
};

template<>
class ThreadHelper< void > : public Thread
{
public:
    ThreadHelper(std::function< void() > && function) :
        m_function(std::move(function)),
        m_future(this,
                 [this]() { this->start(); },
                 [this]() { this->deleteLater(); },
                 [this]() { m_function(); this->deleteLater(); })
    {
    }
    future< void >& Future()
    {
        return m_future;
    }
private:
    ~ThreadHelper()
    {
        m_future.run();
    }
    void run()
    {
        m_function();
    }
    std::function< void() > m_function;
    future< void > m_future;
};

/*
 *
 * Below APIs wrappes a function around a future and then returns the future.
 *
 */

template< typename T >
future<T>& run(std::function< T() > function)
{
    return (new ThreadHelper<T>(std::move(function)))->Future();
}

template< typename T, typename ... Args >
future<T>& run(std::function< T(Args ...) > function, Args ... args)
{
    return Task::run<T>(std::bind(std::move(function), std::move(args) ...));
}

static inline future< void >& run(std::function< void() > function)
{
    return Task::run< void >(std::move(function));
}

template< typename ... Args >
future< void >& run(std::function< void(Args ...) > function, Args ... args)
{
    return Task::run< void >(std::bind(std::move(function), std::move(args) ...));
}

/*
 *
 * A few useful helper functions
 *
 */

template< typename T >
T await(std::function< T() > function)
{
    return Task::run<T>(std::move(function)).await();
}

template< typename T, typename ... Args >
T await(std::function< T(Args ...) > function, Args ... args)
{
    return Task::await<T>(std::bind(std::move(function), std::move(args) ...));
}

static inline void await(std::function< void() > function)
{
    Task::await< void >(std::move(function));
}

template< typename T >
T await(Task::future<T>& e)
{
    return e.await();
}

template< typename T >
T await(std::future<T> t)
{
    return Task::await<T>([ & ]() { return t.get(); });
}

/*
 * These methods run their arguments in a separate thread and does not offer
 * continuation feature.Useful when wanting to just run a function in a
 * different thread.
 */
static inline void exec(std::function< void() > function)
{
    Task::run(std::move(function)).start();
}

template< typename T, typename ... Args >
void exec(std::function< T(Args ...) > function, Args ... args)
{
    Task::exec(std::bind(std::move(function), std::move(args) ...));
}

}

}

}

#if 0 // start example block

Examples on how to use the library

** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
* Example use cases on how to use Task::run().then() API
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **

templated version that passes a return value of one function to another function
-------------------------------------------------------------------------------- -

int foo
{
    /*
     * This task will run on a different thread
     * This tasks returns a result
     */
    return 0;
}

void bar(int r)
{
    /*
     * This task will run on the original thread.
     * This tasks takes an argument returned by task _a
     */
}

Task::run<int>(foo).then(bar);

alternatively,

Task::future<int>& e = Task::run<int>(foo);

e.then(bar);


Non templated version that does not pass around return value
----------------------------------------------------------------
void foo_1()
{
    /*
     * This task will run on a different thread
     * This tasks returns with no result
     */
}

void bar_1()
{
    /*
     * This task will run on the original thread.
     * This tasks takes no argument
     */
}

Task::run(foo_1).then(bar_1);

alternatively,

Task::future<void>& e = Task::run(foo_1);

e.then(bar_1);

** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
* Example use cases on how to use Task::run().await() API
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **

int r = Task::await<int>(foo);

alternatively,

Task::future<int>& e = Task::run<int>(foo);

int r = e.await();

alternatively,

int r = Task::run<int>(foo).await();

** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** *
* Example use cases on how to use lambda that requires an argument
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** *

/*
 * declaring "foo_2" with an auto keyword will not be sufficient here
 * and the full std::function<blablabla> is required.
 *
 * For the same reason,just plugging in a lambda that requires arguments
 * into Task::run() will not be sufficent and the plugged in lambda must
 * be casted to std::function<blablabla> for it to compile.
 *
 * Why the above restriction? No idea but i suspect it has to do with
 * variadic template type deduction failing to see something.
 */

std::function< int(int) > foo_2 = [](int x)
{
    return x + 1;
};

Task::run(foo_2, 6).then([](int r)
{
    qDebug() << r;
});

alternatively,

r = Task::await(foo_2, 6);

#endif //end example block

#endif //__TASK_H_INCLUDED__
