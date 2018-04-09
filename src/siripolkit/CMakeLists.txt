cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++14 -I${PROJECT_BINARY_DIR}/siripoltik/ )
include_directories( ${PROJECT_BINARY_DIR}/siripoltik/ )

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

set( sirikali.pkexecMOC_FILES
	zulupolkit.h
 )

set( sirikali.pkexecSRC
	main.cpp
	zulupolkit.cpp
)

if( QT5 )
	find_package( Qt5Core REQUIRED )
	find_package( Qt5Network REQUIRED )

	Qt5_WRAP_CPP( sirikali.pkexecMOC ${sirikali.pkexecMOC_FILES} )
else()
	find_package( Qt4 REQUIRED QtCore QtNetwork )

	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )

	INCLUDE( ${QT_USE_FILE} )
	Qt4_WRAP_CPP( sirikali.pkexecMOC ${sirikali.pkexecMOC_FILES} )
endif()

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( sirikali.pkexec ${sirikali.pkexecMOC} ${sirikali.pkexecSRC} )

#set_target_properties( sirikali.pkexec PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( sirikali.pkexec PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
	target_link_libraries( sirikali.pkexec ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} mhogomchungu_task )
else()
	target_link_libraries( sirikali.pkexec ${QT_LIBRARIES} ${QT_NETWORK} mhogomchungu_task )
endif()

install( TARGETS sirikali.pkexec RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
