cmake_minimum_required( VERSION 2.6 )
cmake_policy( SET CMP0017 OLD )
cmake_policy( SET CMP0002 OLD )

set_property( GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON )

project( sirikali )

set( PGR_VERSION "1.2.5" )

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic )

include_directories( ${PROJECT_BINARY_DIR} )

INCLUDE( GNUInstallDirs )

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

INCLUDE(CMakeDependentOption)
INCLUDE(FindPkgConfig)

add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all --param ssp-buffer-size=4 )
include_directories( ${PROJECT_BINARY_DIR} )

if( LIB_SUFFIX )
	set( CMAKE_INSTALL_LIBDIR "${LIB_SUFFIX}" )
	set( CMAKE_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX}" )
endif()

pkg_check_modules( PWQUALITY pwquality )

if( PWQUALITY_FOUND )

	find_file( header_pwquality pwquality.h PATHS ${PWQUALITY_INCLUDE_DIRS} )
	find_library( library_pwquality pwquality )

	if( library_pwquality )
		if( header_pwquality )
			file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 1\n" )
			file( APPEND ${PROJECT_BINARY_DIR}/can_build_pwquality.h "extern \"C\"\n{\n#include <${header_pwquality}>\n}" )
		else()
			file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
		endif()
	else( library_pwquality )
		file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
	endif()
else()
	file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_FULL_DATADIR}/sirikali/translations/\"\n")

file( WRITE  ${PROJECT_BINARY_DIR}/install_prefix.h "\n#define INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}/\"\n" )

file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef THIS_VERSION
#define THIS_VERSION \"${PGR_VERSION}\"
#endif
\n
#ifndef SIRIKALI_VERSION
#define SIRIKALI_VERSION
#define VERSION_STRING \"Version  : ${PGR_VERSION}\\nCopyright: 2016-2017 Francis Banyikwa,mhogomchungu@gmail.com\\nLicense  : GPLv2+\"
#endif
\n" )

if( NOT DEFINED INTERNAL_LXQT_WALLET )
	set( INTERNAL_LXQT_WALLET "false" )
endif()

if( NOT DEFINED QT5 )
	set( QT5 "true" )
endif()

find_file( GCRYPT_INCLUDE_FILE gcrypt.h )
find_path( GCRYPT_INCLUDE_PATH gcrypt.h )

add_definitions( -I${GCRYPT_INCLUDE_PATH} )

find_library( GCRYPT_LIBRARY gcrypt )

if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "could not find gcrypt header file" )
else()
	MESSAGE( STATUS "found gcrypt header file: ${GCRYPT_INCLUDE_FILE}" )
endif()

if( NOT GCRYPT_LIBRARY )
	MESSAGE( FATAL_ERROR "could not find gcrypt library(libgcrypt.so)" )
else()
	get_filename_component( GCRYPT_LIBRARY_PATH ${GCRYPT_LIBRARY} DIRECTORY )

	MESSAGE( STATUS "found gcrypt library: ${GCRYPT_LIBRARY}" )
	MESSAGE( STATUS "adding library search path of: ${GCRYPT_LIBRARY_PATH}" )

endif()


if( INTERNAL_LXQT_WALLET )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "lxqt wallet support will be provided by an internal library" )
	message( STATUS "---------------------------------------------------------------------------" )

	ADD_SUBDIRECTORY( src/lxqt_wallet )
	include_directories( ${PROJECT_SOURCE_DIR}/src/lxqt_wallet/frontend )
	include_directories( ${PROJECT_BINARY_DIR}/src/lxqt_wallet/frontend )
else()
	pkg_check_modules( LXQT_WALLET lxqt-wallet )

	if( NOT LXQT_WALLET_FOUND )

		message( STATUS "ERROR: could not find lxqt_wallet package" )
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "lxqt wallet support will be provided by an internal library" )
		message( STATUS "---------------------------------------------------------------------------" )

		ADD_SUBDIRECTORY( src/lxqt_wallet )
		include_directories( ${PROJECT_SOURCE_DIR}/src/lxqt_wallet/frontend )
		include_directories( ${PROJECT_BINARY_DIR}/src/lxqt_wallet/frontend )
	else()
		if( LXQT_WALLET_VERSION VERSION_GREATER "2.0.0" AND QT5 )

			include_directories( ${LXQT_WALLET_INCLUDEDIR} )
			link_directories( ${LXQT_WALLET_LIBDIR} )

			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an external library" )
			message( STATUS "---------------------------------------------------------------------------" )
		else()
			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an internal library" )
			message( STATUS "because external library is too old(<= 2.0.0) or cant build " )
			message( STATUS "with external lxqt_wallet while building with Qt4" )
			message( STATUS "---------------------------------------------------------------------------" )

			ADD_SUBDIRECTORY( src/lxqt_wallet )
			include_directories( ${PROJECT_SOURCE_DIR}/src/lxqt_wallet/frontend )
			include_directories( ${PROJECT_BINARY_DIR}/src/lxqt_wallet/frontend )
		endif()
	endif()
endif()

add_definitions( -std=c++11 )

set( UI_FILES 	src/sirikali.ui
		src/keydialog.ui
		src/dialogmsg.ui
		src/plugin.ui
		src/favorites.ui
		src/readonlywarning.ui
		src/walletconfig.ui
		src/walletconfiginput.ui
		src/options.ui
		src/dialogok.ui
)

set( MOC_FILES 	src/dialogok.h
		src/sirikali.h
		src/keydialog.h
		src/oneinstance.h
		src/mountinfo.h
		src/dialogmsg.h
		src/plugin.h
		src/favorites.h
		src/readonlywarning.h
		src/walletconfig.h
		src/walletconfiginput.h
		src/utility.h
		src/options.h
		src/networkAccessManager.hpp
		src/task.h
 )

set( SRC 	src/dialogok.cpp
		src/main.cpp
		src/sirikali.cpp
		src/keydialog.cpp
		src/siritask.cpp
		src/oneinstance.cpp
		src/mountinfo.cpp
		src/utility.cpp
		src/dialogmsg.cpp
		src/favorites.cpp
		src/checkforupdates.cpp
		src/plugin.cpp
		src/tablewidget.cpp
		src/readonlywarning.cpp
		src/walletconfig.cpp
		src/walletconfiginput.cpp
		src/options.cpp
		src/secrets.cpp
)

if( QT5 )
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Core REQUIRED )
	find_package( Qt5Network REQUIRED )

	set( CMAKE_INCLUDE_CURRENT_DIR ON )
	include_directories( ${Qt5Widgets_INCLUDE_DIRS} )
	include_directories( ${Qt5Network_INCLUDE_DIRS} )

	add_definitions( ${Qt5Widgets_DEFINITIONS} )

	QT5_WRAP_UI( UI ${UI_FILES} )
	QT5_WRAP_CPP( MOC ${MOC_FILES} )
	QT5_ADD_RESOURCES( TRAY_RC_SRCS src/icon.qrc )

	INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
	INCLUDE_DIRECTORIES( ${GCRYPT_INCLUDE_PATH} )

	add_executable( sirikali ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )

	link_directories( ${GCRYPT_LIBRARY_PATH} )

	TARGET_LINK_LIBRARIES( sirikali ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${library_pwquality} ${GCRYPT_LIBRARY} lxqt-wallet -L${GCRYPT_LIBRARY_PATH})

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "Building GUI components using Qt5" )
	message( STATUS "---------------------------------------------------------------------------\n\n" )
else()
	find_package( Qt4 REQUIRED )

	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )

	INCLUDE( ${QT_USE_FILE} )
	INCLUDE( ${QT_USE_FILE} )

	INCLUDE( ${QT_USE_FILE} )

	QT4_WRAP_UI( UI ${UI_FILES} )
	QT4_WRAP_CPP( MOC ${MOC_FILES} )
	QT4_ADD_RESOURCES( TRAY_RC_SRCS src/icon.qrc )

	INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
	INCLUDE_DIRECTORIES( ${GCRYPT_INCLUDE_PATH} )

	link_directories( ${GCRYPT_LIBRARY_PATH} )

	add_executable( sirikali ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( sirikali -lQtCore -lQtGui -lQtNetwork ${library_pwquality} ${GCRYPT_LIBRARY} lxqt-wallet -L${GCRYPT_LIBRARY_PATH} )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "Building GUI components using Qt4" )
	message( STATUS "---------------------------------------------------------------------------\n\n" )
endif()

set_target_properties( sirikali PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic" )

install( TARGETS sirikali RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

install( FILES ${PROJECT_BINARY_DIR}/sirikali.desktop
DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications

PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install( FILES icons/256x256/sirikali.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps/ )
install( FILES icons/48x48/sirikali.png DESTINATION   ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/ )
install( FILES src/sirikali.png DESTINATION           ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )

install( FILES translations/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
install( FILES translations/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )

#install( FILES sirikali.png DESTINATION share/icons/hicolor/32x32/apps )

if( USE_POLKIT )

file( WRITE ${PROJECT_BINARY_DIR}/sirikali.policy
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE policyconfig PUBLIC
 \"-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN\"
 \"http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd\">
<policyconfig>

  <action id=\"org.debian.pkexec.sirikali\">
    <message>Authentication is required to run SiriKali</message>
    <icon_name>sirikali.xpm</icon_name>
    <defaults>
      <allow_any>auth_admin</allow_any>
      <allow_inactive>auth_admin</allow_inactive>
      <allow_active>auth_admin</allow_active>
    </defaults>
    <annotate key=\"org.freedesktop.policykit.exec.path\">${CMAKE_INSTALL_PREFIX}/bin/sirikali</annotate>
    <annotate key=\"org.freedesktop.policykit.exec.allow_gui\">true</annotate>
  </action>

</policyconfig>
")

file( WRITE ${PROJECT_BINARY_DIR}/sirikali.pkexec
"#!/bin/bash
pkexec ${CMAKE_INSTALL_FULL_BINDIR}/sirikali -K $UID $*
")

# desktop file section
file( WRITE ${PROJECT_BINARY_DIR}/sirikali.desktop

"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_FULL_BINDIR}/sirikali.pkexec -d %U
GenericName[en_US]=Encrypted Folder Manager
GenericName=Encrypted Folder Manager
Icon=sirikali
Name[en_US]=SiriKali
Name=SiriKali
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")

install ( FILES ${PROJECT_BINARY_DIR}/sirikali.policy DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions )

install ( FILES ${PROJECT_BINARY_DIR}/sirikali.pkexec
          DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

else()

# desktop file section
file( WRITE ${PROJECT_BINARY_DIR}/sirikali.desktop

"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_FULL_BINDIR}/sirikali -d %U
GenericName[en_US]=Encrypted Folder Manager
GenericName=Encrypted Folder Manager
Icon=sirikali
Name[en_US]=SiriKali
Name=SiriKali
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")

endif()
