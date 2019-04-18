cmake_minimum_required( VERSION 2.6 )

find_package( PkgConfig )

if( WIN32 )
    add_library( lxqt-wallet-backend STATIC backend/fake_wallet_backend.c )

    set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -pedantic" )
else()
    add_subdirectory( backend )
endif()

add_subdirectory( frontend )


