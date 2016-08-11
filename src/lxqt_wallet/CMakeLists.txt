cmake_minimum_required( VERSION 2.6 )
cmake_policy( SET CMP0017 OLD )

find_package( PkgConfig )

add_subdirectory( backend )
add_subdirectory( frontend )
