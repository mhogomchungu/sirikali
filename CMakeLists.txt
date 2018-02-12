cmake_minimum_required( VERSION 3.0 )

set_property( GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON )

project( sirikali )

set( PGR_VERSION "1.3.3" )

add_definitions( -Wextra -Wall -pedantic )

if( WIN32 )
    include_directories( "C:\\Program Files (x86)\\WinFsp\\inc" )
    link_directories( "C:\\Program Files (x86)\\WinFsp\\lib" )
endif()

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

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

if( WIN32 )
    add_definitions( -D_DEFAULT_SOURCE )
else()
    add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all --param ssp-buffer-size=4 )
endif()

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
#define VERSION_STRING \"Version  : ${PGR_VERSION}\\nCopyright: 2016-2018 Francis Banyikwa,mhogomchungu@gmail.com\\nLicense  : GPLv2+\"
#endif
\n" )

if( NOT DEFINED INTERNAL_LXQT_WALLET )
	set( INTERNAL_LXQT_WALLET "false" )
endif()

if( DEFINED QT5 AND NOT QT5 )
	message( FATAL_ERROR "Qt4 Build is no longer supported" )
endif()

set( QT5 "true" )

find_file( JSON_HEADER_PATH "nlohmann/json.hpp" )

if( NOT JSON_HEADER_PATH )
	set( JSON_HEADER_PATH "${PROJECT_SOURCE_DIR}/src/3rdParty/json/nlohmann/json.hpp" )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/json.h "\n#include \"${JSON_HEADER_PATH}\"" )
MESSAGE( STATUS "found json.hpp header file at: ${JSON_HEADER_PATH}" )

find_file( GCRYPT_INCLUDE_FILE gcrypt.h )
find_path( GCRYPT_INCLUDE_PATH gcrypt.h )

add_definitions( -I${GCRYPT_INCLUDE_PATH} )

find_library( GCRYPT_LIBRARY gcrypt )

if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "could not find gcrypt header file" )
else()
	MESSAGE( STATUS "found gcrypt header file at: ${GCRYPT_INCLUDE_FILE}" )
endif()

if( NOT GCRYPT_LIBRARY )
	MESSAGE( FATAL_ERROR "could not find gcrypt library" )
else()
	get_filename_component( GCRYPT_LIBRARY_PATH ${GCRYPT_LIBRARY} DIRECTORY )

	MESSAGE( STATUS "found gcrypt library at: ${GCRYPT_LIBRARY}" )
	MESSAGE( STATUS "adding library search path of: ${GCRYPT_LIBRARY_PATH}" )

endif()

pkg_check_modules( MCHUNGU_NETWORK mhogomchungu_network )
pkg_check_modules( MCHUNGU_TASK mhogomchungu_task )

if( INTERNAL_LXQT_WALLET )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "lxqt wallet support will be provided by an internal library" )
	message( STATUS "---------------------------------------------------------------------------" )

	ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
	include_directories( ${PROJECT_SOURCE_DIR}/src/3rdParty/lxqt_wallet/frontend )
	include_directories( ${PROJECT_BINARY_DIR}/src/3rdParty/lxqt_wallet/frontend )
else()
	pkg_check_modules( LXQT_WALLET lxqt-wallet )

	if( NOT LXQT_WALLET_FOUND )

		message( STATUS "ERROR: could not find lxqt_wallet package" )
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "lxqt wallet support will be provided by an internal library" )
		message( STATUS "---------------------------------------------------------------------------" )

		ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
		include_directories( ${PROJECT_SOURCE_DIR}/src/3rdParty/lxqt_wallet/frontend )
		include_directories( ${PROJECT_BINARY_DIR}/src/3rdParty/lxqt_wallet/frontend )
	else()
		if( LXQT_WALLET_VERSION VERSION_GREATER "3.0.0" AND QT5 )

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

			ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
			include_directories( ${PROJECT_SOURCE_DIR}/src/3rdParty/lxqt_wallet/frontend )
			include_directories( ${PROJECT_BINARY_DIR}/src/3rdParty/lxqt_wallet/frontend )
		endif()
	endif()
endif()

if( MCHUNGU_NETWORK_FOUND )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "mhogomchungu_network library will be provided by an external library" )
	message( STATUS "---------------------------------------------------------------------------" )

	include_directories( "${MCHUNGU_NETWORK_INCLUDEDIR}" )
	link_directories( "${MCHUNGU_NETWORK_LIBDIR}" )
else()
	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "mhogomchungu_network library will be provided by an internal library" )
	message( STATUS "---------------------------------------------------------------------------" )
	ADD_SUBDIRECTORY( src/3rdParty/NetworkAccessManager )
	include_directories( src/3rdParty/NetworkAccessManager )
endif()

if( MCHUNGU_TASK_FOUND )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "mhogomchungu_task library will be provided by an external library" )
	message( STATUS "---------------------------------------------------------------------------" )

	include_directories( "${MCHUNGU_TASK_INCLUDEDIR}" )
	link_directories( "${MCHUNGU_TASK_LIBDIR}" )
else()
	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "mhogomchungu_task library will be provided by an internal library" )
	message( STATUS "---------------------------------------------------------------------------" )
	include_directories( src/3rdParty/tasks )
	ADD_SUBDIRECTORY( src/3rdParty/tasks )
endif()

add_definitions( -std=c++14 )

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
		src/help.ui
		src/gocryptfscreateoptions.ui
		src/filemanager.ui
		src/securefscreateoptions.ui
		src/cryfscreateoptions.ui
                src/configfileoption.ui
                src/configoptions.ui
                src/ecryptfscreateoptions.ui
)

set( MOC_FILES
                src/configoptions.h
                src/configfileoption.h
                src/checkforupdates.h
                src/dialogok.h
                src/help.h
		src/sirikali.h
		src/keydialog.h
		src/oneinstance.h
		src/dialogmsg.h
		src/plugin.h
		src/favorites.h
		src/gocryptfscreateoptions.h
		src/readonlywarning.h
		src/walletconfig.h
		src/walletconfiginput.h
		src/options.h
		src/filemanager.h
		src/mountinfo.h
		src/securefscreateoptions.h
		src/cryfscreateoptions.h
		src/ecryptfscreateoptions.h
 )

set( SRC 	src/dialogok.cpp
		src/main.cpp
		src/gocryptfscreateoptions.cpp
		src/configoptions.cpp
                src/configfileoption.cpp
		src/sirikali.cpp
		src/keydialog.cpp
		src/siritask.cpp
		src/help.cpp
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
		src/filemanager.cpp
		src/securefscreateoptions.cpp
		src/cryfscreateoptions.cpp
		src/utility2.cpp
		src/ecryptfscreateoptions.cpp
		src/winfsp.cpp
)

if( APPLE )

	SET( MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/MacOSXBundleInfo.plist.in" )
	SET( MACOSX_BUNDLE_BUNDLE_NAME "SiriKali" )
	SET( MACOSX_BUNDLE_COPYRIGHT "(c)2016-2018,Francis Banyikwa,mhogomchungu@gmail.com" )
	SET( MACOSX_BUNDLE_BUNDLE_VERSION "${PGR_VERSION}" )
	SET( MACOSX_BUNDLE_GUI_IDENTIFIER "com.crypto.SiriKali" )
	SET( MACOSX_BUNDLE_SHORT_VERSION_STRING "${PGR_VERSION}" )
	SET( MACOSX_BUNDLE_LONG_VERSION_STRING "${PGR_VERSION}" )
	SET( MACOSX_BUNDLE_ICON_FILE sirikali.icns )
	SET_SOURCE_FILES_PROPERTIES( icons/256x256/sirikali.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET_SOURCE_FILES_PROPERTIES( translations/en_US.qm PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET_SOURCE_FILES_PROPERTIES( translations/fr_FR.qm PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
endif()

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

if( APPLE )
	add_executable( sirikali MACOSX_BUNDLE ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} ${PROJECT_SOURCE_DIR}/icons/256x256/sirikali.icns )
else()
        if( WIN32)
                add_executable( sirikali WIN32 ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )
	else()
                add_executable( sirikali ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )
	endif()
endif()

link_directories( ${GCRYPT_LIBRARY_PATH} )

if( WIN32 )
    TARGET_LINK_LIBRARIES( sirikali winfsp-x86 ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${library_pwquality} ${GCRYPT_LIBRARY} lxqt-wallet mhogomchungu_task mhogomchungu_network -L${GCRYPT_LIBRARY_PATH})
else()
    TARGET_LINK_LIBRARIES( sirikali ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${library_pwquality} ${GCRYPT_LIBRARY} lxqt-wallet mhogomchungu_task mhogomchungu_network -L${GCRYPT_LIBRARY_PATH})
endif()

message( STATUS "---------------------------------------------------------------------------" )
message( STATUS "Building GUI components using Qt5" )
message( STATUS "---------------------------------------------------------------------------\n\n" )

if( WIN32 )
    set_target_properties( sirikali PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic" )
else()
    set_target_properties( sirikali PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic" )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/siriPolkit.h "\n#define siriPolkitPath \"${CMAKE_INSTALL_PREFIX}/bin/sirikali.pkexec\"" )

if( APPLE )

SET( plugin_dest_dir sirikali.app/Contents/MacOS )
SET( qtconf_dest_dir sirikali.app/Contents/Resources )
SET( APPS "\${CMAKE_INSTALL_PREFIX}/sirikali.app" )

set_target_properties( sirikali PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/MacOSXBundleInfo.plist.in" )

install( TARGETS sirikali BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime )

file( COPY icons/256x256/sirikali.icns DESTINATION "sirikali.app/Contents/Resources/" )
file( COPY translations/fr_FR.qm DESTINATION "sirikali.app/Contents/Resources/" )
file( COPY translations/en_US.qm DESTINATION "sirikali.app/Contents/Resources/" )
file( COPY translations/de_DE.qm DESTINATION "sirikali.app/Contents/Resources/" )
file( COPY translations/es_MX.qm DESTINATION "sirikali.app/Contents/Resources/" )

file( COPY changelog DESTINATION "sirikali.app/Contents/Resources/" )

#install( DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime )

#install( CODE " file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")" COMPONENT Runtime )

#SET( DIRS ${QT_LIBRARY_DIRS} )

#INSTALL(CODE "
#    file(GLOB_RECURSE QTPLUGINS
#      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#    include(BundleUtilities)
#   fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
#    " COMPONENT Runtime)

#set( CPACK_BINARY_DRAGNDROP ON )
#include( CPack )

else()

install( TARGETS sirikali RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

install( FILES icons/256x256/sirikali.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps/ )
install( FILES icons/48x48/sirikali.png DESTINATION   ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/ )
install( FILES src/sirikali.png DESTINATION           ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES src/sirikali.png DESTINATION           ${CMAKE_INSTALL_DATAROOTDIR}/icons )

install( FILES translations/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
install( FILES translations/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
install( FILES translations/es_MX.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
install( FILES translations/de_DE.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )

install( FILES src/sirikali.1.gz DESTINATION ${CMAKE_INSTALL_DATADIR}/man/man1/ )
install( FILES src/sirikali.pkexec.1.gz DESTINATION ${CMAKE_INSTALL_DATADIR}/man/man1/ )

install( FILES src/sirikali.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo/ )

endif()

if( NOT APPLE )
#install( FILES sirikali.png DESTINATION share/icons/hicolor/32x32/apps )

# desktop file section
file( WRITE ${PROJECT_BINARY_DIR}/sirikali.desktop

"[Desktop Entry]
Comment[en_US]=Manage Encrypted Volumes Hosted In Folders
Comment=Manage Encrypted Volumes Hosted In Folders
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
MimeType=inode/directory;
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")

install( FILES ${PROJECT_BINARY_DIR}/sirikali.desktop
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

if( UNIX AND NOT APPLE )
    ADD_SUBDIRECTORY( src/siripolkit )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/org.sirikali.pkexec.policy
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE policyconfig PUBLIC
 \"-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN\"
 \"http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd\">
<policyconfig>

  <action id=\"org.sirikali.pkexec\">
    <message>Authentication is required to complete requested operation.</message>
    <icon_name>sirikali</icon_name>
    <defaults>
      <allow_any>auth_admin</allow_any>
      <allow_inactive>auth_admin</allow_inactive>
      <allow_active>auth_admin</allow_active>
    </defaults>
    <annotate key=\"org.freedesktop.policykit.exec.path\">${CMAKE_INSTALL_PREFIX}/bin/sirikali.pkexec</annotate>
  </action>

</policyconfig>
")

install ( FILES ${PROJECT_BINARY_DIR}/org.sirikali.pkexec.policy DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions )

endif()
