cmake_minimum_required( VERSION 3.0 )

project( networkAccessManager )

add_definitions( -Wextra -Wall -pedantic )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

INCLUDE(CMakeDependentOption)

find_package( Qt5Core REQUIRED )

QT5_WRAP_CPP( MOC_LIBRARY networkAccessManager.hpp )

include_directories( ${Qt5Core_INCLUDE_DIRS} )

add_library( networkAccessManager STATIC ${MOC_LIBRARY} )

TARGET_LINK_LIBRARIES( networkAccessManager ${Qt5Core_LIBRARIES} )

if( WIN32 )
	set_target_properties( networkAccessManager PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic " )
else()
	set_target_properties( networkAccessManager PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )
endif()
