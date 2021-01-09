cmake_minimum_required( VERSION 3.0 )

project( MhogomchunguNetwork )

set( LIB_VERSION "1.0.0" )
set( SO_VERSION "1.0.0" )

add_definitions( -Wextra -Wall -pedantic)

if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
	set( CMAKE_CXX_STANDARD 20 )
else()
	set( CMAKE_CXX_STANDARD 14 )
endif()

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

INCLUDE( GNUInstallDirs )

INCLUDE( CMakeDependentOption )

include_directories( ${Qt5Core_INCLUDE_DIRS} )

QT5_WRAP_CPP( MOC_LIBRARY network_access_manager.hpp )

if( MHOGOMCHUNGU_NETWORK_INSTALL )

	add_library( mhogomchungu_network SHARED ${MOC_LIBRARY})
	set_target_properties(mhogomchungu_network PROPERTIES SOVERSION "${SO_VERSION}")
else()
	add_library( mhogomchungu_network STATIC ${MOC_LIBRARY} )
endif()

target_link_libraries( mhogomchungu_network ${Qt5Core_LIBRARIES} )

if( WIN32 )
	set_target_properties( mhogomchungu_network PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic " )
else()
	set_target_properties( mhogomchungu_network PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/mhogomchungu_network.pc
"prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_FULL_LIBDIR}
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}/mhogomchungu

Name: mhogomchungu_network
Description: A Qt5 library that modenizes QNetworkAccessManager with moderm c++ constructs
Version: ${LIB_VERSION}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -l${Qt5Core_LIBRARIES} -l${Qt5Network_LIBRARIES}
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}/mhogomchungu
\n")

if( MHOGOMCHUNGU_NETWORK_INSTALL )
	install( FILES ${PROJECT_BINARY_DIR}/mhogomchungu_network.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
	install( FILES network_access_manager.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mhogomchungu )
	install( TARGETS mhogomchungu_network LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
endif()

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
