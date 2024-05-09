cmake_minimum_required(VERSION 3.14)

project(sirikali_cppcryptfs LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(BUILD_WITH_QT6)
        find_package( Qt6 COMPONENTS Core  REQUIRED )
        add_executable(sirikali_cppcryptfs  main.cpp )
        target_link_libraries(sirikali_cppcryptfs Qt6::Core)
else()
        find_package( Qt5 COMPONENTS Core REQUIRED )
        add_executable(sirikali_cppcryptfs  main.cpp )
        target_link_libraries(sirikali_cppcryptfs Qt5::Core)
endif()



