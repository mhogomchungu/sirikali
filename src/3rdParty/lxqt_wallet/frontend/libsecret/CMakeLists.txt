cmake_minimum_required(VERSION 3.0.0)

include_directories( ${LIBSECRET_INCLUDE_DIRS} )
link_directories( ${LIBSECRET_LIBDIR} )
add_definitions( -I${LIBSECRET_INCLUDEDIR} )

set( SECRETSERVICE_LIBRARY "secretlib" )
add_library( secretlib STATIC ../libsecret.c )
target_link_libraries( secretlib ${LIBSECRET_LIBRARIES} -L${LIBSECRET_LIBDIR} )
set_target_properties( secretlib PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic" )
