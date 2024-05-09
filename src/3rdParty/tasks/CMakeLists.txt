if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.0)
endif()

project( mhogomchungu_task )

set( LIB_VERSION "1.2.3" )
set( SO_VERSION "1.0.0" )

add_definitions( -Wextra -Wall -pedantic)

set( CMAKE_CXX_STANDARD 14 )

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

INCLUDE( GNUInstallDirs )

INCLUDE( CMakeDependentOption )

set( MOC_LIBRARY task.hpp )

if(BUILD_WITH_QT6)
	include_directories( ${Qt6Core_INCLUDE_DIRS} )
else()
	include_directories( ${Qt5Core_INCLUDE_DIRS} )
endif()


if( MCHUNGU_TASK_INSTALL )
	add_library( mhogomchungu_task SHARED ${MOC_LIBRARY} )
	set_target_properties(mhogomchungu_task PROPERTIES SOVERSION "${SO_VERSION}")
else()
	add_library( mhogomchungu_task STATIC ${MOC_LIBRARY} )
endif()

if(BUILD_WITH_QT6)
	target_link_libraries( mhogomchungu_task ${Qt6Core_LIBRARIES} )
else()
	target_link_libraries( mhogomchungu_task ${Qt5Core_LIBRARIES} )
endif()

if( WIN32 )
	set_target_properties( mhogomchungu_task PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic " )
else()
	set_target_properties( mhogomchungu_task PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )
endif()

if( MCHUNGU_TASK_DEBUG )
	set( MOC_EXE example.h )
	add_executable( example example.cpp main.cpp ${MOC_EXE} )
	target_link_libraries( example mhogomchungu_task )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/mhogomchungu_task.pc
"prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_FULL_LIBDIR}
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}/mhogomchungu

Name: mhogomchungu_task
Description: A Qt5 library that offers task based programming using modern C++
Version: ${LIB_VERSION}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -l${Qt5Core_LIBRARIES}
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}/mhogomchungu
\n")

if( MCHUNGU_TASK_INSTALL )
	install( FILES ${PROJECT_BINARY_DIR}/mhogomchungu_task.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
	install( FILES task.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mhogomchungu )
	install( TARGETS mhogomchungu_task LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
endif()

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
