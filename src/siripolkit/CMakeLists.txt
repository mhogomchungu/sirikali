if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.0)
endif()

add_definitions( -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR}/siripoltik/ )
include_directories( ${PROJECT_BINARY_DIR}/siripoltik/ )


if(${CMAKE_VERSION} VERSION_LESS "3.1.0")
        add_definitions(-std=c++14)
else()
        set( CMAKE_CXX_STANDARD 14 )
endif()

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set( sirikali.pkexecSRC
	main.cpp
	zulupolkit.cpp
)

if(BUILD_WITH_QT6)
        find_package( Qt6 COMPONENTS Core Network REQUIRED )
else()
        find_package( Qt5 COMPONENTS Core Network REQUIRED )
endif()


INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( sirikali.pkexec ${sirikali.pkexecMOC} ${sirikali.pkexecSRC} )

#set_target_properties( sirikali.pkexec PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( sirikali.pkexec PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic" )

if(BUILD_WITH_QT6)
        target_link_libraries( sirikali.pkexec ${Qt6Core_LIBRARIES} ${Qt6Network_LIBRARIES} mhogomchungu_task -Wl,-znow )
else()
        target_link_libraries( sirikali.pkexec ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} mhogomchungu_task -Wl,-znow )
endif()

install( TARGETS sirikali.pkexec RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
