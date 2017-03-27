cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/siripoltik/ )
include_directories( ${PROJECT_BINARY_DIR}/siripoltik/ )

set( zuluPolticMOC_FILES
	zulupolkit.h
	../task.h
 )

set( zuluPolticSRC
	main.cpp
	zulupolkit.cpp
)

if( QT5 )
	find_package( Qt5Core REQUIRED )
	find_package( Qt5Network REQUIRED )

	Qt5_WRAP_CPP( zuluPolticMOC ${zuluPolticMOC_FILES} )
else()
	find_package( Qt4 REQUIRED QtCore QtNetwork )

	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )

	INCLUDE( ${QT_USE_FILE} )
	Qt4_WRAP_CPP( zuluPolticMOC ${zuluPolticMOC_FILES} )
endif()

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( siripoltik ${zuluPolticMOC} ${zuluPolticSRC} )

set_target_properties( siripoltik PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( siripoltik PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
	target_link_libraries( siripoltik ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} )
else()
	target_link_libraries( siripoltik ${QT_LIBRARIES} ${QT_NETWORK} )
endif()

install( TARGETS siripoltik RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
