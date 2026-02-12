
#pragma once

#include <QString>
#include <QStringList>

namespace cmd
{
	class exe
	{
	public:
		exe()
		{
		}
		exe( const QString& exe,const QStringList& args ) :
			m_exe( exe ),m_args( args )
		{
		}
		exe( const QString& exe ) : m_exe( exe )
		{
		}
		const QString& command() const
		{
			return m_exe ;
		}
		const QStringList args() const
		{
			return m_args ;
		}
		template< typename ... Args >
		QStringList addArgs( Args&& ... args ) const
		{
			auto m = m_args ;

			this->add( m,std::forward< Args >( args ) ... ) ;

			return m ;
		}
		QString toString() const
		{
			QString m = m_exe ;

			if( m_args.size() ){

				for( const auto& it : m_args ){

					m += " \"" + it + "\"" ;
				}
			}

			return m ;
		}
	private:
		void add( QStringList& ) const
		{
		}
		template< typename First,typename ... Rest >
		void add( QStringList& m,const First& f,Rest&& ... rest ) const
		{
			m.append( f ) ;
			this->add( m,std::forward< Rest >( rest ) ... ) ;
		}
		QString m_exe ;
		QStringList m_args ;
	} ;
}
