/*
 * copyright: 2016
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QEventLoop>
#include <QVector>

#include <functional>
#include <utility>
#include <memory>

class NetworkAccessManager : public QObject
{
	Q_OBJECT
public:
	class NetworkReply
	{
	public:
		NetworkReply( QNetworkReply * e ) : m_QNetworkReply( e )
		{
		}
		NetworkReply( NetworkReply&& other )
		{
			if( m_QNetworkReply ){

				m_QNetworkReply->deleteLater() ;
			}

			m_QNetworkReply = other.m_QNetworkReply ;
			other.m_QNetworkReply = nullptr ;
		}
		QNetworkReply * release()
		{
			auto e = m_QNetworkReply ;
			m_QNetworkReply = nullptr ;
			return e ;
		}
		~NetworkReply()
		{
			if( m_QNetworkReply ){

				m_QNetworkReply->deleteLater() ;
			}
		}
		QNetworkReply * operator->()
		{
			return m_QNetworkReply ;
		}
	private:
		QNetworkReply * m_QNetworkReply = nullptr ;
	};

	using function_t = std::function< void( NetworkAccessManager::NetworkReply ) > ;
#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	NetworkAccessManager()
	{
		connect( &m_manager,SIGNAL( finished( QNetworkReply * ) ),
			 this,SLOT( networkReply( QNetworkReply * ) ),Qt::QueuedConnection ) ;
	}
#else
	NetworkAccessManager()
	{
		connect( &m_manager,&QNetworkAccessManager::finished,
		         this,&NetworkAccessManager::networkReply,Qt::QueuedConnection ) ;
	}
#endif
	QNetworkAccessManager& QtNAM()
	{
		return m_manager ;
	}
	QNetworkReply * get( const QNetworkRequest& r,function_t&& f )
	{
		auto e = m_manager.get( r ) ;

		m_entries.append( { e,std::move( f ) } ) ;

		return e ;
	}
	QNetworkReply * get( const QNetworkRequest& r )
	{
		QNetworkReply * q ;

		QEventLoop l ;

		this->get( r,[ & ]( NetworkAccessManager::NetworkReply e ){

			q = e.release() ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return q ;
	}
	template< typename T >
	QNetworkReply * post( const QNetworkRequest& r,const T& e,function_t&& f )
	{
		auto q = m_manager.post( r,e ) ;

		m_entries.append( { q,std::move( f ) } ) ;

		return q ;
	}
	template< typename T >
	QNetworkReply * post( const QNetworkRequest& r,const T& e )
	{
		QNetworkReply * q ;

		QEventLoop l ;

		this->post( r,e,[ & ]( NetworkAccessManager::NetworkReply e ){

			q = e.release() ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return q ;
	}
	QNetworkReply * head( const QNetworkRequest& r,function_t&& f )
	{
		auto e = m_manager.head( r ) ;

		m_entries.append( { e,std::move( f ) } ) ;

		return e ;
	}
	QNetworkReply * head( const QNetworkRequest& r )
	{
		QNetworkReply * q ;

		QEventLoop l ;

		this->head( r,[ & ]( NetworkAccessManager::NetworkReply e ){

			q = e.release() ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return q ;
	}
private slots:
	void networkReply( QNetworkReply * r )
	{
		auto s = m_entries.size() ;

		for( decltype( s ) i = 0 ; i < s ; i++ ){

			const auto& q = m_entries.at( i ) ;

			if( q.first == r ){

				q.second( r ) ;

				m_entries.remove( i ) ;

				break ;
			}
		}
	}
private:
	QVector< std::pair< QNetworkReply *,function_t > > m_entries ;
	QNetworkAccessManager m_manager ;
};
