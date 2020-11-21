cmake_minimum_required(VERSION 3.1.0)

include_directories( ${LIBSECRET_INCLUDE_DIRS} )
link_directories( ${LIBSECRET_LIBDIR} )
add_definitions( -I${LIBSECRET_INCLUDEDIR} )

set( SECRETSERVICE_LIBRARY "secretlib" )
add_library( secretlib STATIC ../libsecret.c )
target_link_libraries( secretlib ${LIBSECRET_LIBRARIES} -L${LIBSECRET_LIBDIR} )
set_target_properties( secretlib PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )
