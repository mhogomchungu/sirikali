
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

include_directories( ${LIBSECRET_INCLUDE_DIRS} )
set( SECRETSERVICE_LIBRARY "secretlib" )
add_library( secretlib STATIC ../libsecret.c )
target_link_libraries( secretlib ${LIBSECRET_LIBRARIES} )
set_target_properties( secretlib PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic " )
