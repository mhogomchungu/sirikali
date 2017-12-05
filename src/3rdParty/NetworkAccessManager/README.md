NetworkAccessManager
====================

This single header library "modernizes" QNetworkAccessManager by
adding modern C++ features to it.

An understanding of how QNetworkAccessManager works is required because
this library is a thin wrapper to it.

Example below shows an example of how to process a network response asynchronously
using a lambda.

The return argument of the API can be used to fine tune the network request.

```
void foo::bar()
{
	QNetworkRequest networRequest( QUrl( "https://example.com" ) ) ;

	networRequest.setRawHeader( "Host","example.com" ) ;
	networRequest.setRawHeader( "Accept-Encoding","text/plain" ) ;

	int timeOut = 10 ;

	m_network.get( timeOut,networRequest,[]( QNetworkReply& e ){

		/*
		 *
		 * Process network response
		 *
		 */
	},[](){
		/*
		 * Handle timeout
		 */
	} ) ;
}

```
