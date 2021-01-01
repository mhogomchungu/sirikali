cmake_minimum_required(VERSION 3.0.0)

INCLUDE(FindPkgConfig)

include_directories(${PROJECT_BINARY_DIR}/frontend)
include_directories(${PROJECT_BINARY_DIR}/backend)
include_directories(${PROJECT_BINARY_DIR})

include_directories(${CMAKE_BINARY_DIR})

if(APPLE)
    file( WRITE ${PROJECT_BINARY_DIR}/osx_keychain.h "#define OSX_KEYCHAIN 1" )
else()
    file( WRITE ${PROJECT_BINARY_DIR}/osx_keychain.h "#define OSX_KEYCHAIN 0" )
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS}})
include_directories(${Qt5DBus_INCLUDE_DIRS}})

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})

if(NOSECRETSUPPORT)
    SET(SECRET_SUPPORT "false")
    file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
else()
    pkg_check_modules(LIBSECRET libsecret-1 QUIET)
    if(LIBSECRET_FOUND)
        add_subdirectory(libsecret)
        SET(SECRET_SUPPORT "true")
        file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 1\n")
    else()
        SET(SECRET_SUPPORT "false")
        file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0 )
	set( CMAKE_CXX_STANDARD 20 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"  AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
	set( CMAKE_CXX_STANDARD 20 )
else()
	set( CMAKE_CXX_STANDARD 14 )
endif()

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

if(NOKDESUPPORT)
    SET(KWALLET_SUPPORT "false")
    file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
else()
    message(STATUS "Checking for modules 'KF5Wallet'")
    find_package(KF5Wallet QUIET)
    if(KF5Wallet_FOUND )
        message(STATUS "  Found KF5Wallet, version ${KF5Wallet_VERSION}")
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
        SET(KWALLET_SUPPORT "true")
    else()
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
        SET(KWALLET_SUPPORT "false")
    endif()
endif()

if(KWALLET_SUPPORT)
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "kwallet support found,will build kwallet functionality")
    message(STATUS "-----------------------------------------------------------------------")
else()
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "kwallet support NOT found, support will be discovered at runtime through dbus")
    message(STATUS "-----------------------------------------------------------------------")
endif()

if(SECRET_SUPPORT)
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "libsecret support found,will build libsecret functionality")
    message(STATUS "-----------------------------------------------------------------------")
else()
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "libsecret support NOT found,will not build libsecret functionality")
    message(STATUS "-----------------------------------------------------------------------")
endif()

set(COMMON_UI_FILES  changepassworddialog.ui password_dialog.ui)
set(COMMON_MOC_FILES task.h changepassworddialog.h password_dialog.h)
set(COMMON_SRC_FILES changepassworddialog.cpp password_dialog.cpp lxqt_windows_dpapi.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_osx_keychain.cpp)

Qt5_WRAP_UI(UI ${COMMON_UI_FILES})
Qt5_WRAP_CPP(MOC ${COMMON_MOC_FILES})

if(KWALLET_SUPPORT)
    Qt5_WRAP_CPP(MOC_KWALLET lxqt_kwallet.h)
    set(KWALLET_COMMON lxqt_kwallet.cpp)
    if(SECRET_SUPPORT)
	add_library(lxqt-wallet STATIC ${KWALLET_COMMON} lxqt_libsecret.cpp ${UI} ${COMMON_SRC_FILES} ${MOC} ${MOC_KWALLET})
	target_link_libraries(lxqt-wallet secretlib)
    else()
	add_library(lxqt-wallet STATIC ${KWALLET_COMMON} ${UI} ${COMMON_SRC_FILES} ${MOC} ${MOC_KWALLET})
    endif()
    target_link_libraries(lxqt-wallet KF5::Wallet)
else()
    Qt5_WRAP_CPP(MOC_KWALLET_DBUS lxqt_kwallet-dbus.h)
    set(KWALLET_DBUS lxqt_kwallet-dbus.cpp)
    if(SECRET_SUPPORT)
	add_library(lxqt-wallet STATIC lxqt_libsecret.cpp ${KWALLET_DBUS} ${UI} ${COMMON_SRC_FILES} ${MOC} ${MOC_KWALLET_DBUS})
	target_link_libraries(lxqt-wallet secretlib)
    else()
	add_library(lxqt-wallet STATIC ${UI} ${COMMON_SRC_FILES} ${KWALLET_DBUS} ${MOC} ${MOC_KWALLET_DBUS})
    endif()
endif()

target_link_libraries(lxqt-wallet lxqtwallet-backend)

target_link_libraries(lxqt-wallet ${Qt5Widgets_LIBRARIES})
target_link_libraries(lxqt-wallet ${Qt5Core_LIBRARIES})

if(WIN32)
    target_link_libraries(lxqt-wallet -lcrypt32 -lbcrypt)
else()
    target_link_libraries(lxqt-wallet ${Qt5DBus_LIBRARIES})
endif()

if(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(SECURITY_LIBRARY Security REQUIRED)
    target_link_libraries(lxqt-wallet ${COREFOUNDATION_LIBRARY} ${SECURITY_LIBRARY})
endif()

set_target_properties(lxqt-wallet PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic ")
set_target_properties(lxqt-wallet PROPERTIES SOVERSION "${LIBRARY_VERSION}")

file(WRITE ${PROJECT_BINARY_DIR}/translations_path.h
"
#define TRANSLATIONS_PATH \"${CMAKE_INSTALL_PREFIX}/share/lxqt/translations/lxqt-wallet/\"
")

SET(TRANSLATIONS
   ../translations/lxqt-wallet_nl_NL.ts
   ../translations/lxqt-wallet_fr_FR.ts
   ../translations/lxqt-wallet_de_DE.ts
   ../translations/lxqt-wallet_gr_GR.ts
   ../translations/lxqt-wallet_es_ES.ts
   ../translations/lxqt-wallet_pl_PL.ts
)
