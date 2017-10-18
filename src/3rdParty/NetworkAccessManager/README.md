NetworkAccessManager
====================

This single header library "modernizes" QNetworkAccessManager by
adding modern C++ features to it.

An understanding of how QNetworkAccessManager works is required because
this library is a thin wrapper to it.

Example below shows an example of how to process a network response asynchronously
using a lambda.

```
void foo::bar()
{
	QNetworkRequest networRequest( QUrl( "https://example.com" ) ) ;

	networRequest.setRawHeader( "Host","example.com" ) ;
	networRequest.setRawHeader( "Accept-Encoding","text/plain" ) ;

	m_networkAccessManager.get( networRequest,[ this ]( QNetworkReply& e ){

		QByteArray result = e.readAll() ;

		this->processResult( result ) ;
	} ) ;
}

```
