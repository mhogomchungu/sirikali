
cmake_minimum_required(VERSION 2.6)

if( WIN32 )
     cmake_policy(SET CMP0020 NEW)
endif()

INCLUDE(FindPkgConfig)

#
# set below variable to "false" if you want to build a static library
#
set(BUILD_SHARED "false")

set(LIBRARY_VERSION "2.0.0")

add_definitions(-I${PROJECT_BINARY_DIR}/frontend)
add_definitions(-I${PROJECT_BINARY_DIR}/backend)
add_definitions(-I${PROJECT_BINARY_DIR}/src/lxqt_wallet/frontend)
add_definitions(-I${PROJECT_BINARY_DIR}/src/lxqt_wallet/backend)
add_definitions(-I${PROJECT_BINARY_DIR})

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

if( APPLE )
	file( WRITE ${PROJECT_BINARY_DIR}/osx_keychain.h "#define OSX_KEYCHAIN 1" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/osx_keychain.h "#define OSX_KEYCHAIN 0" )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/translations_path.h
"
#define TRANSLATIONS_PATH \"${CMAKE_INSTALL_PREFIX}/share/lxqt/translations/lxqt-wallet/\"
")

if(QT5)
	find_package(Qt5Widgets REQUIRED)
	find_package(Qt5Core REQUIRED)
	find_package(Qt5DBus REQUIRED)

	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	include_directories(${Qt5Widgets_INCLUDE_DIRS})

	add_definitions(${Qt5Widgets_DEFINITIONS})

	set( KF5 "true" )
else()
	find_package(Qt4 REQUIRED)

	SET(QT_USE_QTCORE TRUE)
	SET(QT_USE_QTGUI TRUE)

	add_definitions(-I${Qt4_INCLUDE_DIR})
	add_definitions(-I${QT_HEADERS_DIR})

	INCLUDE(${QT_USE_FILE})

	INCLUDE(${QT_USE_FILE})
endif()

if(NOSECRETSUPPORT)
	SET(SECRET_SUPPORT "false")
	file(WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
else()
	pkg_check_modules(LIBSECRET libsecret-1)

	if(LIBSECRET_FOUND)
		add_subdirectory(libsecret)
		SET(SECRET_SUPPORT "true")
		file(WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 1\n")
	else()
		SET(SECRET_SUPPORT "false")
		file(WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
	endif()
endif()

add_definitions(-std=c++14)

if(NOKDESUPPORT)
	SET(KDE_SUPPORT "false")
	SET(KWALLET_SUPPORT "false")
	file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
else()
	if(KF5)
		find_package(Qt5Widgets REQUIRED)
		find_package(Qt5Core REQUIRED)

		find_package(KF5Wallet QUIET)
		find_package(KF5Notifications QUIET)

		if(KF5Wallet_FOUND AND KF5Notifications_FOUND )
			file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
			SET(KDE_SUPPORT "true")
			SET(KWALLET_SUPPORT "true")
		else()
			file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
			SET(KDE_SUPPORT "false")
			SET(KWALLET_SUPPORT "false")
		endif()
	else()
		find_package(KDE4 QUIET)
		if(KDE4_FOUND)
			include(KDE4Defaults)
			include_directories(${KDE4_INCLUDES})
			add_definitions(-I${KDE4_INCLUDE_DIR})

			add_definitions(-I${Qt4_INCLUDE_DIR})
			add_definitions(-I${QT_HEADERS_DIR})

			SET(KDE_SUPPORT "true")

			if(NOKWALLETSUPPORT)
				SET(KWALLET_SUPPORT "false")
				file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
			else()
				find_library(LIBKWALLETBACKEND libkwalletbackend.so)
				if(NOT LIBKWALLETBACKEND)
					# some distributions do not have libkwalletbackend.so for reasons i currently do not know
					# look for libkwalletbackend.sp.4 when the above happen
					find_library(LIBKWALLETBACKEND libkwalletbackend.so.4)
					if(NOT LIBKWALLETBACKEND)
						file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
						SET(KWALLET_SUPPORT "false")
					else()
						SET(KWALLET_SUPPORT "true")
						file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
					endif()
				else()
					SET(KWALLET_SUPPORT "true")
					file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
				endif()
			endif()

			find_library(LIBKDEUI libkdeui.so)
			if(NOT LIBKDEUI)
				# adding this one just in case the same case as above happen
				find_library(LIBKDEUI libkdeui.so.5)
			endif()

			if(NOT LIBKDEUI)
				SET(KDE_SUPPORT "false")
				message(STATUS "could not find libkdeui.so or libkdeui.so.5,please make sure you have kde development libraries installed and try again")
			endif()

			find_library(LIBKDECORE libkdecore.so)
			if(NOT LIBKDECORE)
				# adding this one just in case the same case as above happen
				find_library(LIBKDECORE libkdecore.so.5)
			endif()

			if(NOT LIBKDECORE)
				SET(KDE_SUPPORT "false")
				message(STATUS "could not find libkdecore.so or libkdecore.so.5,please make sure you have kde development libraries installed and try again")
			endif()
		else()
			file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
			SET(KDE_SUPPORT "false")
			SET(KWALLET_SUPPORT "false")
		endif()
	endif()

endif()

if(KWALLET_SUPPORT)
	message(STATUS "\n--------------------------------------------------------------------------")
	message(STATUS "kwallet support found,will build kwallet functionality")
	message(STATUS "-----------------------------------------------------------------------")
else()
	message(STATUS "\n--------------------------------------------------------------------------")
	message(STATUS "kwallet support NOT found,will not build kwallet functionality")
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

set(UI_FILES 	changepassworddialog.ui password_dialog.ui)

set(MOC_FILES 	changepassworddialog.h password_dialog.h task.h)
set(MOC_FILES_1 changepassworddialog.h password_dialog.h lxqt_kwallet.h task.h)
set(MOC_FILES_2 changepassworddialog.h password_dialog.h lxqt_kwallet.h task.h)
set(MOC_FILES_3 changepassworddialog.h password_dialog.h task.h)

if( WIN32 )
	set(SRC 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_wallet.cpp)
	set(SRC_1 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_wallet.cpp lxqt_kwallet.cpp)
	set(SRC_2 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_wallet.cpp lxqt_kwallet.cpp lxqt_libsecret.cpp)
	set(SRC_3 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_wallet.cpp lxqt_libsecret.cpp)
else()
	set(SRC 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp)
	set(SRC_1 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_kwallet.cpp)
	set(SRC_2 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_kwallet.cpp lxqt_libsecret.cpp)
	set(SRC_3 	lxqt_windows_dpapi.cpp lxqt_osx_keychain.cpp changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_libsecret.cpp)

endif()

if(BUILD_SHARED)
	set( LIBRARY_TYPE "SHARED")
else()
	set( LIBRARY_TYPE "STATIC")
endif()

if(QT5)

Qt5_WRAP_UI(UI ${UI_FILES})

if(KDE_SUPPORT)
	if(KWALLET_SUPPORT)
		if(SECRET_SUPPORT)
			Qt5_WRAP_CPP(MOC ${MOC_FILES_2})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_2} ${MOC})
			target_link_libraries(lxqt-wallet secretlib)
			target_link_libraries(lxqt-wallet "${Qt5Widgets_LIBRARIES}")
			target_link_libraries(lxqt-wallet "${Qt5Core_LIBRARIES}")
			target_link_libraries(lxqt-wallet KF5::Wallet KF5::Notifications)
		else()
			Qt5_WRAP_CPP(MOC ${MOC_FILES_1})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_1} ${MOC})
			target_link_libraries(lxqt-wallet "${Qt5Widgets_LIBRARIES}")
			target_link_libraries(lxqt-wallet "${Qt5Core_LIBRARIES}")
			target_link_libraries(lxqt-wallet KF5::Wallet KF5::Notifications)
		endif()
	else()
		if(SECRET_SUPPORT)
			Qt5_WRAP_CPP(MOC ${MOC_FILES_3})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_3} ${MOC})
			target_link_libraries(lxqt-wallet secretlib)
			target_link_libraries(lxqt-wallet "${Qt5Widgets_LIBRARIES}")
			target_link_libraries(lxqt-wallet "${QtCore_LIBRARIES}")
		else()
			Qt5_WRAP_CPP(MOC ${MOC_FILES})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC} ${MOC})
			target_link_libraries(lxqt-wallet "${Qt5Widgets_LIBRARIES}")
			target_link_libraries(lxqt-wallet "${QtCore_LIBRARIES}")
			target_link_libraries(lxqt-wallet "${LIBKDEUI}")
			target_link_libraries(lxqt-wallet "${LIBKDECORE}")
		endif()
	endif()
else()
	if(SECRET_SUPPORT)
		Qt5_WRAP_CPP(MOC ${MOC_FILES_3})
		add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_3} ${MOC})
		target_link_libraries(lxqt-wallet secretlib)
		target_link_libraries(lxqt-wallet "${Qt5Widgets_LIBRARIES}")
		target_link_libraries(lxqt-wallet "${QtCore_LIBRARIES}")
	else()
		Qt5_WRAP_CPP(MOC ${MOC_FILES})
		add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC} ${MOC})
		target_link_libraries(lxqt-wallet "${Qt5Widgets_LIBRARIES}")
		target_link_libraries(lxqt-wallet "${QtCore_LIBRARIES}")
	endif()
endif()

else()

Qt4_WRAP_UI(UI ${UI_FILES})

if(KDE_SUPPORT)
	if(KWALLET_SUPPORT)
		if(SECRET_SUPPORT)
			Qt4_WRAP_CPP(MOC ${MOC_FILES_2})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_2} ${MOC})
			target_link_libraries(lxqt-wallet secretlib)
			set(link_libraries "-L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE} ${LIBKWALLETBACKEND}")
		else()
			Qt4_WRAP_CPP(MOC ${MOC_FILES_1})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_1} ${MOC})
			set(link_libraries "-L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE} ${LIBKWALLETBACKEND}")
		endif()
	else()
		if(SECRET_SUPPORT)
			Qt4_WRAP_CPP(MOC ${MOC_FILES_3})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_3} ${MOC})
			target_link_libraries(lxqt-wallet secretlib)
			set(link_libraries "-L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR}")
		else()
			Qt4_WRAP_CPP(MOC ${MOC_FILES})
			add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC} ${MOC})
			set(link_libraries "-L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE}")
		endif()

	endif()
else()
	if(SECRET_SUPPORT)
		Qt4_WRAP_CPP(MOC ${MOC_FILES_3})
		add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC_3} ${MOC})
		target_link_libraries(lxqt-wallet secretlib)
		set(link_libraries "-L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR}")
	else()
		Qt4_WRAP_CPP(MOC ${MOC_FILES})
		add_library(lxqt-wallet ${LIBRARY_TYPE} ${UI} ${SRC} ${MOC})
	endif()
endif()

endif()

if( WIN32 )
    set_target_properties(lxqt-wallet PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic ")
else()
    set_target_properties(lxqt-wallet PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic ")
endif()

set_target_properties(lxqt-wallet PROPERTIES SOVERSION "${LIBRARY_VERSION}")

if( WIN32 )

	TARGET_LINK_LIBRARIES(lxqt-wallet -lcrypt32 -lbcrypt "${link_libraries}" lxqt-wallet-backend)
else()
	TARGET_LINK_LIBRARIES(lxqt-wallet "${link_libraries}" lxqt-wallet-backend)
endif()


if( APPLE )
	find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
	find_library(SECURITY_LIBRARY Security REQUIRED)
	TARGET_LINK_LIBRARIES(lxqt-wallet ${COREFOUNDATION_LIBRARY} ${SECURITY_LIBRARY})
endif()

if(BUILD_SHARED)
	install(TARGETS lxqt-wallet RUNTIME LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

	install(FILES lxqt_wallet.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lxqt")

	file( WRITE ${PROJECT_BINARY_DIR}/lxqt-wallet.pc
"prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_FULL_LIBDIR}
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}/lxqt

Name: lxqt_wallet
Description: lxqt secure storage system
Version: ${LIBRARY_VERSION}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -lQtGui -lQtCore -lgcrypt
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}/lxqt
\n")

	install(FILES ${PROJECT_BINARY_DIR}/lxqt-wallet.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
endif()
