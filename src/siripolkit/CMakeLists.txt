cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/siripoltik/ )
include_directories( ${PROJECT_BINARY_DIR}/siripoltik/ )

set( siripolkitMOC_FILES
	zulupolkit.h
	../task.h
 )

set( siripolkitSRC
	main.cpp
	zulupolkit.cpp
)

if( QT5 )
	find_package( Qt5Core REQUIRED )
	find_package( Qt5Network REQUIRED )

	Qt5_WRAP_CPP( siripolkitMOC ${siripolkitMOC_FILES} )
else()
	find_package( Qt4 REQUIRED QtCore QtNetwork )

	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )

	INCLUDE( ${QT_USE_FILE} )
	Qt4_WRAP_CPP( siripolkitMOC ${siripolkitMOC_FILES} )
endif()

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( siripolkit ${siripolkitMOC} ${siripolkitSRC} )

set_target_properties( siripolkit PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( siripolkit PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
	target_link_libraries( siripolkit ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} )
else()
	target_link_libraries( siripolkit ${QT_LIBRARIES} ${QT_NETWORK} )
endif()

install( TARGETS siripolkit RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
