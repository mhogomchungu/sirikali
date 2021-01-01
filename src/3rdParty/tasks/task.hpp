/*
 * copyright: 2014-2020
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

#include <type_traits>
#include <vector>
#include <utility>
#include <future>
#include <functional>
#include <QThread>
#include <QEventLoop>
#include <QMutex>
#include <QProcess>
#include <QVariant>
/*
 *
 * Examples on how to use the library are at the end of this file.
 *
 */

/*
 * This library wraps a function into a future where the result of the function
 * can be retrieved through the future's below public methods:
 *
 * 1. .get().  This method runs the wrapped function on the current thread
 *             and could block the thread and hang GUI.
 *
 * 2. .then(). This method does three things:
 *
 *             1. Registers a method to be called when a wrapped function finish running.
 *
 *             2. Runs the wrapped function on a background thread.
 *
 *             3. Runs the registered method on the current thread when the wrapped function finish
 *                running.
 *
 * 3. .await(). This method does three things:
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
 * 4. .queue(). This method runs tasks in a future sequentially and a passed in function will be called
 *              when all tasks finish running. This method behaves like .then( [](){} ) if the future is
 *              managing only one task.
 *
 * 5. .cancel(). This method can be used to cancel a future. It is important to know
 *               that this method does not terminate a running thread that is powering a future, it just
 *               releases memory used by a future and this method should be used if a future is to be discarded
 *               after it it is acquired but never used.
 *
 * 6. .all_threads(). This method returns a vector of QThreads that are powering futures.
 *                    The vector will contain a single entry if this future powers its own task. If this future
 *                    manages other futures,then the returned vector will contain QThread pointers that are in
 *                    the same order as tasks/futures passed to Task::run().
 *
 * 7. .start(). This method is to be used if a future is to be run without caring about
 *              its result. Use this method if you want a future to run but dont want to use any of the above mentioned
 *              methods.
 *
 * 8. .manages_multiple_futures(). This method can be used to check if a future powers
 *    its own task or manages other futures.
 *
 *
 * The future is of type "Task::future<T>&" and "std::reference_wrapper"[1]
 * class can be used if they are to be managed in a container that can not handle references.
 *
 * [1] http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
 */

namespace Task
{
	template< typename T >
	class future;

	namespace detail
	{
		template< typename T >
		void add_void( Task::future< T >&,Task::future< T >&,std::function< T() >&& ) ;
		template< typename T >
		void add( Task::future< T >&,Task::future< T >&,std::function< void( T ) >&& ) ;

		template<typename Function>
		class functionWrapper
		{
		public:
			template< typename ... Args >
			auto operator()( Args&& ... args ) const
			{
				return (*m_function)( std::forward<Args>( args ) ... ) ;
			}
			functionWrapper( Function function ) :
				m_function( std::make_shared<Function>( Function( std::move( function ) ) ) )
			{
			}
		private:
			std::shared_ptr<Function> m_function ;
		};

		template<typename Function>
		functionWrapper<Function> function( Function function )
		{
			return functionWrapper<Function>( std::move( function ) ) ;
		}

#if __cplusplus >= 201703L
		template<typename Function,typename ... Args>
		using result_of = std::invoke_result_t<Function,Args ...> ;
#else
		template<typename Function,typename ... Args>
		using result_of = std::result_of_t<Function(Args ...)> ;
#endif
		template<typename Function>
		using copyable = std::enable_if_t<std::is_copy_constructible<Function>::value,int> ;

		template<typename Function>
		using not_copyable = std::enable_if_t<!std::is_copy_constructible<Function>::value,int> ;

		template<typename ReturnType,typename Function,typename ... Args>
		using has_same_return_type = std::enable_if_t<std::is_same<result_of<Function,Args...>,ReturnType>::value,int> ;

		template<typename Function,typename ... Args>
		using has_void_return_type = has_same_return_type<void,Function,Args...> ;

		template<typename Function,typename ... Args>
		using has_bool_return_type = has_same_return_type<bool,Function,Args...> ;

		template<typename Function,typename ... Args>
		using has_non_void_return_type = std::enable_if_t<!std::is_void<result_of<Function,Args...>>::value,int> ;

		template<typename Function,typename ... Args>
		using has_argument = has_same_return_type<result_of<Function,Args...>,Function,Args...> ;

		template<typename Function>
		using has_no_argument = has_same_return_type<result_of<Function>,Function> ;

		template<typename Function,typename ... Args>
		using returns_void = has_void_return_type<Function,Args...> ;

		template<typename Function,typename ... Args>
		using returns_value = has_non_void_return_type<Function,Args...> ;
	}

	template< typename T >
	struct pair{
		pair( std::function< T() > first,std::function< void( T ) > second ) :
			value( std::make_pair( std::move( first ),std::move( second ) ) )
		{
		}
		std::pair< std::function< T() >,std::function< void( T ) > > value ;
	};

	template<>
	struct pair<void>{
		pair( std::function< void() > first,std::function< void() > second ) :
			value( std::make_pair( std::move( first ),std::move( second ) ) )
		{
		}
		std::pair< std::function< void() >,std::function< void() > > value ;
	};	
	template< typename E,
		  typename F,
		  Task::detail::not_copyable<E> = 0,
		  Task::detail::not_copyable<F> = 0 >
	pair<Task::detail::result_of<E>> make_pair( E e,F f )
	{
		using type = Task::detail::result_of<E> ;
		return pair<type>( Task::detail::function( std::move( e ) ),Task::detail::function( std::move( f ) ) ) ;
	}
	template< typename E,
		  typename F,
		  Task::detail::copyable<E> = 0,
		  Task::detail::copyable<F> = 0 >
	pair<Task::detail::result_of<E>> make_pair( E e,F f )
	{
		return pair<Task::detail::result_of<E>>( std::move( e ),std::move( f ) ) ;
	}
	template< typename E,
		  typename F,
		  Task::detail::not_copyable<E> = 0,
		  Task::detail::copyable<F> = 0 >
	pair<Task::detail::result_of<E>> make_pair( E e,F f )
	{
		using type = Task::detail::result_of<E> ;
		return pair<type>( Task::detail::function( std::move( e ) ),std::move( f ) ) ;
	}
	template< typename E,
		  typename F,
		  Task::detail::copyable<E> = 0,
		  Task::detail::not_copyable<F> = 0 >
	pair<Task::detail::result_of<E>> make_pair( E e,F f )
	{
		using type = Task::detail::result_of<E> ;
		return pair<type>( std::move( e ),Task::detail::function( std::move( f ) ) ) ;
	}
	template< typename T >
	class future : private QObject
	{
	public:
		/*
		 * Use this API if you care about the result
		 */
		//std::function< void( T ) >
		template<typename Function,
			Task::detail::has_argument<Function,T> = 0,
			Task::detail::copyable<Function> = 0>
		void then( Function function )
		{
			m_function = std::move( function ) ;
			this->start() ;
		}
		template<typename Function,
			Task::detail::has_argument<Function,T> = 0,
			Task::detail::not_copyable<Function> = 0>
		void then( Function function )
		{
			m_function = Task::detail::function( std::move( function ) ) ;
			this->start() ;
		}
		/*
		 * Use this API if you DO NOT care about the result
		 */
		//std::function< void( void ) >
		template<typename Function,
			Task::detail::has_no_argument<Function> = 0,
			Task::detail::copyable<Function> = 0>
		void then( Function function )
		{
			m_function_1 = std::move( function ) ;
			this->start() ;
		}
		template<typename Function,
			Task::detail::has_no_argument<Function> = 0,
			Task::detail::not_copyable<Function> = 0>
		void then( Function function )
		{
			m_function_1 = Task::detail::function( std::move( function ) ) ;
			this->start() ;
		}
		template<typename Function,
			 Task::detail::has_no_argument<Function> = 0,
			 Task::detail::not_copyable<Function> = 0>
		void queue( Function function )
		{
			if( this->manages_multiple_futures() ){

				m_function_1 = Task::detail::function( std::move( function ) ) ;

				this->_queue() ;
			}else{
				this->then( Task::detail::function( std::move( function ) ) ) ;
			}
		}
		template<typename Function,
			 Task::detail::has_no_argument<Function> = 0,
			 Task::detail::copyable<Function> = 0>
		void queue( Function function )
		{
			if( this->manages_multiple_futures() ){

				m_function_1 = std::move( function ) ;

				this->_queue() ;
			}else{
				this->then( std::move( function ) ) ;
			}
		}

		void queue()
		{
			if( this->manages_multiple_futures() ){

				m_function_1 = [](){} ;

				this->_queue() ;
			}else{
				this->then( [](){} ) ;
			}
		}
		/*
		 * Below two API just exposes existing functionality using more standard names
		 */
		template<typename Function>
		void when_all( Function function )
		{
			this->then( std::move( function ) ) ;
		}
		template<typename Function>
		void when_seq( Function function )
		{
			this->queue( std::move( function ) ) ;
		}
		template<typename Function,
			 Task::detail::has_no_argument<Function> = 0,
			 Task::detail::copyable<Function> = 0>
		void when_any( Function function )
		{
			if( this->manages_multiple_futures() ){

				this->_when_any( std::move( function ) ) ;
			}else{
				this->then( std::move( function ) ) ;
			}
		}
		template<typename Function,
			 Task::detail::has_no_argument<Function> = 0,
			 Task::detail::not_copyable<Function> = 0>
		void when_any( Function function )
		{
			if( this->manages_multiple_futures() ){

				this->_when_any( Task::detail::function( std::move( function ) ) ) ;
			}else{
				this->then( Task::detail::function( std::move( function ) ) ) ;
			}
		}
		void when_all()
		{
			this->then( [](){} ) ;
		}
		void when_seq()
		{
			this->queue( [](){} ) ;
		}
		void when_any()
		{
			if( this->manages_multiple_futures() ){

				this->_when_any( [](){} ) ;
			}else{
				this->then( [](){} ) ;
			}
		}
		T get()
		{
			if( this->manages_multiple_futures() ){

				for( auto& it : m_tasks ){

					it.second( it.first->get() ) ;
				}

				this->deleteLater() ;

				return T() ;
			}else{
				return m_get() ;
			}
		}
		T await()
		{
			QEventLoop p ;

			T q ;

			m_function = [ & ]( T&& r ){ q = std::move( r ) ; p.exit() ; } ;

			this->start() ;

			p.exec() ;

			return q ;
		}
		bool manages_multiple_futures()
		{
			return m_tasks.size() > 0 ;
		}
		const std::vector< QThread * >& all_threads()
		{
			return m_threads ;
		}
		QThread * first_thread()
		{
			return m_threads[ 0 ] ;
		}
		QThread * thread_at( std::vector< QThread * >::size_type s )
		{
			return m_threads[ s ] ;
		}
		void start()
		{
			if( this->manages_multiple_futures() ){

				this->_start() ;
			}else{
				m_start() ;
			}
		}
		void cancel()
		{
			if( this->manages_multiple_futures() ){

				for( auto& it : m_tasks ){

					it.first->cancel() ;
				}

				this->deleteLater() ;
			}else{
				m_cancel() ;
			}
		}
		future() = default ;
		future( const future& ) = delete ;
		future( future&& ) = delete ;
		future& operator=( const future& ) = delete ;
		future& operator=( future&& ) = delete ;

		future( QThread * e,
			std::function< void() >&& start,
			std::function< void() >&& cancel,
			std::function< T() >&& get ) :
			m_thread( e ),
			m_start ( std::move( start ) ),
			m_cancel( std::move( cancel ) ),
			m_get   ( std::move( get ) )
		{
			if( m_thread ){

				m_threads.push_back( m_thread ) ;
			}else{
				/*
				 * This object was created by "_private_future< T >()" class.
				 * It has no QThread of its own because it only manages other futures.
				 *
				 */
			}
		}
		void run( T&& r )
		{
			if( m_function_1 != nullptr ){

				m_function_1() ;

			}else if( m_function != nullptr ){

				m_function( std::move( r ) ) ;
			}
		}

		template< typename E >
		friend void Task::detail::add( Task::future< E >&,
					  Task::future< E >&,
					  std::function< void( E ) >&& ) ;
	private:
		void _when_any( std::function< void() > function )
		{
			m_when_any_function = std::move( function ) ;

			for( auto& it : m_tasks ){

				it.first->then( [ & ]( T&& e ){

					QMutexLocker m( &m_mutex ) ;

					m_counter++ ;

					if( m_task_not_run ){

						m_task_not_run = false ;

						m.unlock() ;

						it.second( std::forward<T>( e ) ) ;

						m_when_any_function() ;
					}else{
						m.unlock() ;
						it.second( std::forward<T>( e ) ) ;
					}

					if( m_counter == m_tasks.size() ){

						this->deleteLater() ;
					}
				} ) ;
			}
		}
		void _queue()
		{
			m_tasks[ m_counter ].first->then( [ this ]( T&& e ){

				m_tasks[ m_counter ].second( std::forward<T>( e ) ) ;

				m_counter++ ;

				if( m_counter == m_tasks.size() ){

					m_function_1() ;

					this->deleteLater() ;
				}else{
					this->_queue() ;
				}
			} ) ;
		}
		void _start()
		{
			for( auto& it : m_tasks ){

				it.first->then( [ & ]( T&& e ){

					QMutexLocker m( &m_mutex ) ;

					Q_UNUSED( m ) ;

					m_counter++ ;

					it.second( std::forward<T>( e ) ) ;

					if( m_counter == m_tasks.size() ){

						if( m_function_1 != nullptr ){

							m_function_1() ;

						}else if( m_function != nullptr ){

							m_function( T() ) ;
						}

						this->deleteLater() ;
					}
				} ) ;
			}
		}

		QThread * m_thread = nullptr ;
		std::function< void( T ) > m_function = nullptr ;
		std::function< void() > m_function_1  = nullptr ;
		std::function< void() > m_start       = [](){} ;
		std::function< void() > m_cancel      = [](){} ;
		std::function< T() > m_get            = [](){ return T() ; } ;
		std::function< void() > m_when_any_function ;

		QMutex m_mutex ;
		std::vector< std::pair< Task::future< T > *,std::function< void( T ) > > > m_tasks ;
		std::vector< QThread * > m_threads ;
		decltype( m_tasks.size() ) m_counter = 0 ;
		bool m_task_not_run = true ;
	};

	template<>
	class future< void > : private QObject
	{
	public:
		template<typename Function,Task::detail::copyable<Function> = 0>
		void then( Function function )
		{
			m_function = std::move( function ) ;
			this->start() ;
		}
		template<typename Function,Task::detail::not_copyable<Function> = 0>
		void then( Function function )
		{
			m_function = Task::detail::function( std::move( function ) ) ;
			this->start() ;
		}
		template<typename Function,Task::detail::copyable<Function> = 0>
		void queue( Function function )
		{
			if( this->manages_multiple_futures() ){

				m_function = std::move( function ) ;

				this->_queue() ;
			}else{
				this->then( std::move( function ) ) ;
			}
		}
		template<typename Function,Task::detail::not_copyable<Function> = 0>
		void queue( Function function )
		{
			if( this->manages_multiple_futures() ){

				m_function = Task::detail::function( std::move( function ) ) ;

				this->_queue() ;
			}else{
				this->then( Task::detail::function( std::move( function ) ) ) ;
			}
		}
		void queue()
		{
			if( this->manages_multiple_futures() ){

				m_function = [](){} ;

				this->_queue() ;
			}else{
				this->then( [](){} ) ;
			}
		}
		void when_any()
		{
			if( this->manages_multiple_futures() ){

				this->_when_any( [](){} ) ;
			}else{
				this->then( [](){} ) ;
			}
		}
		template<typename Function,Task::detail::copyable<Function> = 0>
		void when_any( Function function )
		{
			if( this->manages_multiple_futures() ){

				this->_when_any( std::move( function ) ) ;
			}else{
				this->then( std::move( function ) ) ;
			}
		}
		template<typename Function,Task::detail::not_copyable<Function> = 0>
		void when_any( Function function )
		{
			if( this->manages_multiple_futures() ){

				this->_when_any( Task::detail::function( std::move( function ) ) ) ;
			}else{
				this->then( Task::detail::function( std::move( function ) ) ) ;
			}
		}
		/*
		 * Below two API just exposes existing functionality using more standard names
		 */
		template<typename Function>
		void when_all( Function function )
		{
			this->then( std::move( function ) ) ;
		}
		template<typename Function>
		void when_seq( Function function )
		{
			this->queue( std::move( function ) ) ;
		}
		void when_all()
		{
			this->then( [](){} ) ;
		}
		void when_seq()
		{
			this->queue() ;
		}
		void get()
		{
			if( this->manages_multiple_futures() ){

				for( auto& it : m_tasks ){

					it.first->get() ;
					it.second() ;
				}

				this->deleteLater() ;
			}else{
				m_get() ;
			}
		}
		void await()
		{
			QEventLoop p ;

			m_function = [ & ](){ p.exit() ; } ;

			this->start() ;

			p.exec() ;
		}
		bool manages_multiple_futures()
		{
			return m_tasks.size() > 0 ;
		}
		const std::vector< QThread * >& all_threads()
		{
			return m_threads ;
		}
		QThread * first_thread()
		{
			return m_threads[ 0 ] ;
		}
		QThread * thread_at( std::vector< QThread * >::size_type s )
		{
			return m_threads[ s ] ;
		}
		void start()
		{
			if( this->manages_multiple_futures() ){

				this->_start() ;
			}else{
				m_start() ;
			}
		}
		void cancel()
		{
			if( this->manages_multiple_futures() ){

				for( auto& it : m_tasks ){

					it.first->cancel() ;
				}

				this->deleteLater() ;
			}else{
				m_cancel() ;
			}
		}
		future() = default ;
		future( const future& ) = delete ;
		future( future&& ) = delete ;
		future& operator=( const future& ) = delete ;
		future& operator=( future&& ) = delete ;

		future(	QThread * e ,
			std::function< void() >&& start,
			std::function< void() >&& cancel,
			std::function< void() >&& get ) :
			m_thread( e ),
			m_start ( std::move( start ) ),
			m_cancel( std::move( cancel ) ),
			m_get   ( std::move( get ) )
		{
			if( m_thread ){

				m_threads.push_back( m_thread ) ;
			}else{
				/*
				 * This object was created by "_private_future< T >()" class.
				 * It has no QThread of its own because it only manages other futures.
				 *
				 */
			}
		}

		template< typename T >
		friend void Task::detail::add_void( Task::future< T >&,
						    Task::future< T >&,
						    std::function< T() >&& ) ;
		void run()
		{
			m_function() ;
		}
	private:
		void _when_any( std::function< void() > function )
		{
			m_when_any_function = std::move( function ) ;

			for( auto& it : m_tasks ){

				it.first->then( [ & ](){

					QMutexLocker m( &m_mutex ) ;

					m_counter++ ;

					if( m_task_not_run ){

						m_task_not_run = false ;

						m.unlock() ;

						it.second() ;

						m_when_any_function() ;
					}else{
						m.unlock() ;
						it.second() ;
					}

					if( m_counter == m_tasks.size() ){

						this->deleteLater() ;
					}
				} ) ;
			}
		}
		void _queue()
		{
			m_tasks[ m_counter ].first->then( [ this ](){

				m_tasks[ m_counter ].second() ;

				m_counter++ ;

				if( m_counter == m_tasks.size() ){

					m_function() ;

					this->deleteLater() ;
				}else{
					this->_queue() ;
				}
			} ) ;
		}

		void _start()
		{
			for( auto& it : m_tasks ){

				it.first->then( [ & ](){

					QMutexLocker m( &m_mutex ) ;

					Q_UNUSED( m ) ;

					m_counter++ ;

					it.second() ;

					if( m_counter == m_tasks.size() ){

						m_function() ;

						this->deleteLater() ;
					}
				} ) ;
			}
		}

		QThread * m_thread = nullptr ;

		std::function< void() > m_function = [](){} ;
		std::function< void() > m_start    = [](){} ;
		std::function< void() > m_cancel   = [](){} ;
		std::function< void() > m_get      = [](){} ;
		std::function< void() > m_when_any_function ;
		QMutex m_mutex ;
		std::vector< std::pair< Task::future< void > *,std::function< void() > > > m_tasks ;
		std::vector< QThread * > m_threads ;
		decltype( m_tasks.size() ) m_counter = 0 ;
		bool m_task_not_run = true ;
	};

	namespace detail
	{
		/*
		 * -------------------------Start of internal helper functions-------------------------
		 */
		template< typename Type,typename Function >
		class ThreadHelper : public QThread
		{
		public:
			ThreadHelper( Function function ) :
				m_function( std::move( function ) ),
				m_future( this,
					  [ this ](){ this->start() ; },
					  [ this ](){ this->deleteLater() ; },
					  [ this ](){ this->deleteLater() ; return m_function() ; } )
			{
				connect( this,&QThread::finished,this,&QThread::deleteLater ) ;
			}
			future<Type>& Future()
			{
				return m_future ;
			}
		private:
			~ThreadHelper()
			{
				m_future.run( std::move( m_result ) ) ;
			}
			void run()
			{
				m_result = m_function() ;
			}
			Function m_function ;
			future<Type> m_future ;
			Type m_result ;
		};

		template< typename Function>
		class ThreadHelperVoid : public QThread
		{
		public:
			ThreadHelperVoid( Function function ) :
				m_function( std::move( function ) ),
				m_future( this,
					  [ this ](){ this->start() ; },
					  [ this ](){ this->deleteLater() ; },
					  [ this ](){ m_function() ; this->deleteLater() ; } )
			{
				connect( this,&QThread::finished,this,&QThread::deleteLater ) ;
			}
			future< void >& Future()
			{
				return m_future ;
			}
		private:
			~ThreadHelperVoid()
			{
				m_future.run() ;
			}
			void run()
			{
				m_function() ;
			}
			Function m_function ;
			future< void > m_future ;
		};
		template<typename Fn,Task::detail::returns_value<Fn> = 0>
		Task::future<Task::detail::result_of<Fn>>& run( Fn function )
		{
			using t = Task::detail::result_of<Fn> ;

			return ( new ThreadHelper<t,Fn>( std::move( function ) ) )->Future() ;
		}
		template<typename Fn,Task::detail::returns_void<Fn> = 0>
		Task::future<Task::detail::result_of<Fn>>& run( Fn function )
		{
			return ( new ThreadHelperVoid<Fn>( std::move( function ) ) )->Future() ;
		}
		template< typename T >
		void add( Task::future< T >& a,Task::future< T >& b,std::function< void( T ) >&& c )
		{
			a.m_tasks.emplace_back( std::addressof( b ),std::move( c ) ) ;
			a.m_threads.push_back( b.m_thread ) ;
		}

		template< typename T >
		void add_void( Task::future< T >& a,Task::future< T >& b,std::function< T() >&& c )
		{
			a.m_tasks.emplace_back( std::addressof( b ),std::move( c ) ) ;
			a.m_threads.push_back( b.m_thread ) ;
		}

		template< typename T >
		void add_task( Task::future< T >& f )
		{
			Q_UNUSED( f )
		}

		template< typename T >
		void add_future( Task::future< T >& f )
		{
			Q_UNUSED( f )
		}

		template< typename T >
		void add_pair( Task::future< T >& f )
		{
			Q_UNUSED( f )
		}

		template< typename T >
		void add_pair_void( Task::future< T >& f )
		{
			Q_UNUSED( f )
		}

		template< typename ... T,
			  typename Function,
			  Task::detail::not_copyable<Function> = 0 >
		void add_task( Task::future< void >& f,Function e,T&& ... t ) ;

		template< typename ... T,
			  typename Function,
			  Task::detail::copyable<Function> = 0 >
		void add_task( Task::future< void >& f,Function e,T&& ... t )
		{
			add_void( f,Task::detail::run( std::function< void() >( std::move( e ) ) ),
				  std::function< void() >( [](){} ) ) ;
			add_task( f,std::forward<T>( t ) ... ) ;
		}

		template< typename ... T,
			  typename Function,
			  Task::detail::not_copyable<Function> >
		void add_task( Task::future< void >& f,Function e,T&& ... t )
		{
			auto a = std::function< void() >( Task::detail::function( std::move( e ) ) ) ;

			add_void( f,Task::detail::run( std::move( a ) ),
				  std::function< void() >( [](){} ) ) ;

			add_task( f,std::forward<T>( t ) ... ) ;
		}

		template< typename ... T >
		void add_future( Task::future< void >& f,Task::future< void >& e,T&& ... t )
		{
			add_void( f,e,std::function< void() >( [](){} ) ) ;
			add_future( f,std::forward<T>( t ) ... ) ;
		}

		template< typename E,typename F,typename ... T >
		void add_pair( Task::future< E >& f,F&& s,T&& ... t )
		{
			add( f,Task::detail::run( std::move( s.value.first ) ),std::move( s.value.second ) ) ;
			add_pair( f,std::forward<T>( t ) ... ) ;
		}

		template< typename F,typename ... T >
		void add_pair_void( Task::future< void >& f,F&& s,T&& ... t )
		{
			add_void( f,Task::detail::run( std::move( s.value.first ) ),std::move( s.value.second ) ) ;
			add_pair_void( f,std::forward<T>( t ) ... ) ;
		}

		template< typename T >
		Task::future< T >& future()
		{
			return *( new Task::future< T >() ) ;
		}

	} //end of detail namespace


	/*
	 * -------------------------End of internal helper functions-------------------------
	 */
	template< typename Fn,Task::detail::copyable<Fn> = 0 >
	auto& run( Fn function )
	{
		return Task::detail::run( std::move( function ) ) ;
	}
	template< typename Fn,Task::detail::not_copyable<Fn> = 0 >
	auto& run( Fn function )
	{
		return Task::detail::run( Task::detail::function( std::move( function ) ) ) ;
	}
#if __cplusplus > 201703L
	template< typename Fn,typename ... Args >
	future<std::invoke_result_t<Fn,Args...>>& run( Fn function,Args ... args )
	{
		return Task::run( [ function = std::move( function ),... args = std::move( args ) ]()mutable{

			return function( std::move( args ) ... ) ;
		} ) ;
	}
#elif __cplusplus == 201703L
	template< typename Fn,typename ... Args >
	future<std::invoke_result_t<Fn,Args...>>& run( Fn function,Args ... args )
	{
		return Task::run( [ args = std::make_tuple( std::move( args ) ... ),function = std::move( function ) ]()mutable{

			return std::apply( [ function = std::move( function ) ]( Args ... args ){

				return function( std::move( args ) ... ) ;

			},std::move( args ) ) ;
		} ) ;
	}
#else
	template< typename Fn,typename ... Args >
	future<std::result_of_t<Fn(Args...)>>& run( Fn function,Args ... args )
	{
		return Task::run( [ =,function = std::move( function ) ](){

			return function( std::move( args ) ... ) ;
		} ) ;
	}
#endif
	class progress : public QObject{
		Q_OBJECT
	public:
		template< typename function >
		progress( QObject * obj,function fn )
		{
			connect( this,&progress::update,obj,std::move( fn ) ) ;
		}
	signals:
		void update( QVariant x ) const ;
	private:
	};

	template< typename Fn,typename cb >
	future<Task::detail::result_of<Fn,const progress&>>& run( QObject * obj,Fn function,cb rp )
	{
		return Task::run( [ obj,rp = std::move( rp ),function = std::move( function ) ](){

			return function( progress( obj,std::move( rp ) ) ) ;
		} ) ;
	}

	template< typename Function,
		  Task::detail::copyable<Function> = 0,
		  typename ... T >
	Task::future< void >& run_tasks( Function f,T ... t )
	{
		auto& e = Task::detail::future< void >() ;
		Task::detail::add_task( e,std::move( f ),std::move( t ) ... ) ;
		return e ;
	}
	template< typename Function,
		  Task::detail::not_copyable<Function> = 0,
		  typename ... T >
	Task::future< void >& run_tasks( Function f,T ... t )
	{
		auto& e = Task::detail::future< void >() ;
		Task::detail::add_task( e,Task::detail::function( std::move( f ) ),std::move( t ) ... ) ;
		return e ;
	}

	template< typename ... T >
	Task::future< void >& run_tasks( Task::future< void >& s,T&& ... t )
	{
		auto& e = Task::detail::future< void >() ;
		Task::detail::add_future( e,s,std::forward<T>( t ) ... ) ;
		return e ;
	}

	template< typename ... T >
	Task::future< void >& run( pair< void > s,T ... t )
	{
		auto& e = Task::detail::future< void >() ;
		Task::detail::add_pair_void( e,std::move( s ),std::move( t ) ... ) ;
		return e ;
	}

	template< typename E,typename ... T >
	Task::future< E >& run( pair< E > s,T ... t )
	{
		auto& e = Task::detail::future< E >() ;
		Task::detail::add_pair( e,std::move( s ),std::move( t ) ... ) ;
		return e ;
	}

	/*
	 *
	 * A few useful helper functions
	 *
	 */
	template< typename Fn >
	Task::detail::result_of<Fn> await( Fn function )
	{
		return Task::run( std::move( function ) ).await() ;
	}

	template< typename Fn,typename ... Args >
	Task::detail::result_of<Fn,Args ...> await( Fn function,Args ... args )
	{
		return Task::run( std::move( function ),std::move( args ) ... ).await() ;
	}
	template< typename T >
	T await( Task::future<T>& e )
	{
		return e.await() ;
	}

	template< typename T >
	T await( std::future<T> t )
	{
		return Task::await<T>( [ & ](){ return t.get() ; } ) ;
	}

	/*
	 * These methods run their arguments in a separate thread and does not offer
	 * continuation feature.Useful when wanting to just run a function in a
	 * different thread.
	 */
	template< typename Fn >
	void exec( Fn function )
	{
		Task::run( std::move( function ) ).start() ;
	}
	template< typename Fn,typename ... Args >
	void exec( Fn function,Args ... args )
	{
		Task::run( std::move( function ),std::move( args ) ... ).start() ;
	}
	template< typename T >
	void exec( Task::future<T>& e )
	{
		e.start() ;
	}

	namespace process {

		class result{
		public:
			result() = default ;
			result( int exit_code ) :
				m_finished( true ),
				m_exitCode( exit_code ),
				m_exitStatus( 0 )
			{
			}
			template< typename E,typename F >
			result( E&& std_out,
				F&& std_error,
				int exit_code,
				int exit_status,
				bool finished ) :
				m_stdOut( std::forward<E>( std_out ) ),
				m_stdError( std::forward<F>( std_error ) ),
				m_finished( finished ),
				m_exitCode( exit_code ),
				m_exitStatus( exit_status )
			{
			}
			result( QProcess& e,int s )
			{
				m_finished   = e.waitForFinished( s ) ;
				m_stdOut     = e.readAllStandardOutput() ;
				m_stdError   = e.readAllStandardError() ;
				m_exitCode   = e.exitCode() ;
				m_exitStatus = e.exitStatus() ;
			}
			const QByteArray& std_out() const
			{
				return m_stdOut ;
			}
			const QByteArray& std_error() const
			{
				return m_stdError ;
			}
			bool finished() const
			{
				return m_finished ;
			}
			bool success() const
			{
				return m_exitCode == 0 &&
				       m_exitStatus == QProcess::NormalExit &&
				       m_finished == true ;
			}
			bool failed() const
			{
				return !this->success() ;
			}
			int exit_code() const
			{
				return m_exitCode ;
			}
			int exit_status() const
			{
				return m_exitStatus ;
			}
		private:
			QByteArray m_stdOut ;
			QByteArray m_stdError ;
			bool m_finished = false ;
			int m_exitCode = 255 ;
			int m_exitStatus = 255 ;
		};

		static inline Task::future< result >& run( const QString& cmd,
		                                           const QStringList& args,
							   int waitTime = -1,
							   const QByteArray& password = QByteArray(),
							   const QProcessEnvironment& env = QProcessEnvironment(),
							   std::function< void() > setUp_child_process = [](){} )
		{
			return Task::run( [ = ](){

				class Process : public QProcess{
				public:
					Process( std::function< void() > function,
						 const QProcessEnvironment& env ) :
						 m_function( std::move( function  ) )
					{
						this->setProcessEnvironment( env ) ;
					}
				protected:
					void setupChildProcess()
					{
						m_function() ;
					}
				private:
					std::function< void() > m_function ;

				} exe( std::move( setUp_child_process ),env ) ;

				if( args.isEmpty() ){

					#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
						exe.start( cmd ) ;
					#else
						exe.start( cmd,args ) ;
					#endif
				}else{
					exe.start( cmd,args ) ;
				}

				if( !password.isEmpty() ){

					exe.waitForStarted( waitTime ) ;
					exe.write( password ) ;
					exe.closeWriteChannel() ;
				}

				return result( exe,waitTime ) ;
			} ) ;
		}

		static inline Task::future< result >& run( const QString& cmd,const QByteArray& password )
		{
			return Task::process::run( cmd,{},-1,password ) ;
		}

		static inline Task::future< result >& run( const QString& cmd,
							   const QStringList& args,
							   const QByteArray& password )
		{
			return Task::process::run( cmd,args,-1,password ) ;
		}
	}
}

#if 0 // start example block

Examples on how to use the library

**********************************************************
* Example use cases on how to use Task::run().then() API
**********************************************************

templated version that passes a return value of one function to another function
---------------------------------------------------------------------------------

int foo
{
	/*
	 * This task will run on a different thread
	 * This tasks returns a result
	 */
	return 0 ;
}

void bar( int r )
{
	/*
	 * This task will run on the original thread.
	 * This tasks takes an argument returned by task _a
	 */
}

Task::run( foo ).then( bar ) ;

alternatively,

Task::future<int>& e = Task::run( foo ) ;

e.then( bar ) ;


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

Task::run( foo_1 ).then( bar_1 ) ;

alternatively,

Task::future<void>& e = Task::run( foo_1 ) ;

e.then( bar_1 ) ;

**********************************************************
* Example use cases on how to use Task::run().await() API
**********************************************************

int r = Task::await( foo ) ;

alternatively,

Task::future<int>& e = Task::run( foo ) ;

int r = e.await() ;

alternatively,

int r = Task::run( foo ).await() ;

*******************************************************************
* Example use cases on how to use lambda that requires an argument
*******************************************************************

auto foo_2 = []( int x ){

	return x + 1 ;
} ;

Task::run( foo_2,6 ).then( []( int r ){

	qDebug() << r ;
} ) ;

alternatively,

r = Task::await( foo_2,6 ) ;

*******************************************************************
* Example use cases on how to run multiple tasks and wait for all to
* to finish before continuing
*******************************************************************

Task::future<void>& e = Task::run( fn1,fn2,fn3 ) ;

or alternatively,

Task::future<void>& f1 = Task::run( fn1 ) ;
Task::future<void>& f2 = Task::run( fn2 ) ;
Task::future<void & f3 = Task::run( fn3 ) ;

Task::future<void>& e = Task::run( f1,f2,f3 ) ;

1.0 .await() can then be called on the future to wait for all tasks to finish before continuing.
2.0 .then()  can then be called on the future to invoke a callback on the current thread when all tasks finish.

*******************************************************************
* Example use cases on how to run multiple tasks and their continuations
* and wait for all to finish before continuing
*******************************************************************

std::cout<< "Testing multiple tasks without continuation arguments" << std::endl ;

auto fna1 = [](){ _printThreadID(); } ;
auto fna2 = [](){ _printThreadID(); } ;
auto fna3 = [](){ _printThreadID(); } ;

auto ra1 = [](){ std::cout << "r1" << std::endl ; } ;
auto ra2 = [](){ std::cout << "r2" << std::endl ; } ;
auto ra3 = [](){ std::cout << "r3" << std::endl ; } ;

Task::future<void>& e = Task::run( Task::make_pair( fna1,ra1 ),
				   Task::make_pair( fna2,ra2 ),
				   Task::make_pair( fna3,ra3 ) ) ;

e.await() ;

std::cout<< "Testing multiple tasks with continuation arguments" << std::endl ;

auto fn1 = [](){ _printThreadID(); return 0 ; } ;
auto fn2 = [](){ _printThreadID(); return 0 ; } ;
auto fn3 = [](){ _printThreadID(); return 0 ; } ;

auto r1 = []( int ){ std::cout << "r1" << std::endl ; } ;
auto r2 = []( int ){ std::cout << "r2" << std::endl ; } ;
auto r3 = []( int ){ std::cout << "r3" << std::endl ; } ;

Task::future<int>& s = Task::run( Task::make_pair( fn1,r1 ),
				  Task::make_pair( fn2,r2 ),
				  Task::make_pair( fn3,r3 ) ) ;

s.then( [](){ QCoreApplication::quit() ; } ) ;

#endif //end example block

#endif //__TASK_H_INCLUDED__
