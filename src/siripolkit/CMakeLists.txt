cmake_minimum_required( VERSION 3.0 )

add_definitions( -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR}/siripoltik/ )
include_directories( ${PROJECT_BINARY_DIR}/siripoltik/ )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0 )
	set( CMAKE_CXX_STANDARD 20 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
	set( CMAKE_CXX_STANDARD 20 )
else()
	set( CMAKE_CXX_STANDARD 14 )
endif()

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

set( sirikali.pkexecSRC
	main.cpp
	zulupolkit.cpp
)

find_package( Qt5 COMPONENTS Core Network REQUIRED )


INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( sirikali.pkexec ${sirikali.pkexecMOC} ${sirikali.pkexecSRC} )

#set_target_properties( sirikali.pkexec PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( sirikali.pkexec PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
        target_link_libraries( sirikali.pkexec ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} mhogomchungu_task -Wl,-znow )
else()
        target_link_libraries( sirikali.pkexec ${QT_LIBRARIES} ${QT_NETWORK} mhogomchungu_task -Wl,-znow )
endif()

install( TARGETS sirikali.pkexec RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
