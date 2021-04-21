cmake_minimum_required( VERSION 3.0 )

set_property( GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON )

project( sirikali )

set( PGR_VERSION "1.4.8" )
set( COPYRIGHT_YEARS "2016-2020" )

add_definitions( -Wextra -Wall -pedantic )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0 )
	set( CMAKE_CXX_STANDARD 20 )
	MESSAGE( STATUS "Setting C++ version to C++20" )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
	set( CMAKE_CXX_STANDARD 20 )
	MESSAGE( STATUS "Setting C++ version to C++20" )
else()
	set( CMAKE_CXX_STANDARD 14 )
	MESSAGE( STATUS "Setting C++ version to C++14" )
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Qunused-arguments)
endif()

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

if( APPLE )
        file( WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_PREFIX}/sirikali.app/Contents/Resources/\"\n")
else()
	file( WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_FULL_DATADIR}/sirikali/translations/\"\n")
endif()

file( WRITE  ${PROJECT_BINARY_DIR}/install_prefix.h "\n#define INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}/\"\n" )

if( NOT DEFINED INTERNAL_LXQT_WALLET )
	set( INTERNAL_LXQT_WALLET "false" )
endif()

if( DEFINED QT5 AND NOT QT5 )
	message( FATAL_ERROR "Qt4 Build is no longer supported" )
endif()

if(NOT DEFINED NONETWORKSUPPORT)
   set( NONETWORKSUPPORT "true" )
endif()

set( QT5 "true" )

if( WIN32 )
    set( INTERNAL_LXQT_WALLET "true" )
else()

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
endif()

if( UNIX AND NOT APPLE )

	find_package( Qt5 COMPONENTS Core Widgets Network DBus REQUIRED )

	message( STATUS "Found Qt5Widgets, version  ${Qt5Widgets_VERSION}" )
	message( STATUS "Found Qt5Core, version  ${Qt5Core_VERSION}" )
	message( STATUS "Found Qt5Network, version  ${Qt5Network_VERSION}" )
	message( STATUS "Found Qt5DBus, version  ${Qt5DBus_VERSION}" )
else()
	find_package( Qt5 COMPONENTS Core Widgets Network REQUIRED )

	message( STATUS "Found Qt5Widgets, version  ${Qt5Widgets_VERSION}" )
	message( STATUS "Found Qt5Core, version  ${Qt5Core_VERSION}" )
	message( STATUS "Found Qt5Network, version  ${Qt5Network_VERSION}" )
endif()

pkg_check_modules( MCHUNGU_NETWORK mhogomchungu_network )
#pkg_check_modules( MCHUNGU_TASK mhogomchungu_task )

file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef THIS_VERSION
#define THIS_VERSION \"${PGR_VERSION}\"
#endif
\n
#ifndef SIRIKALI_VERSION
#define SIRIKALI_VERSION
#define VERSION_STRING \"Version  : ${PGR_VERSION} (Compiled with Qt ${Qt5Core_VERSION})\\nCopyright: ${COPYRIGHT_YEARS} Francis Banyikwa,mhogomchungu@gmail.com\\nLicense  : GPLv2+\"
#endif
\n" )

if(NONETWORKSUPPORT)
        file(WRITE  ${PROJECT_BINARY_DIR}/network_support.h "\n#define HAS_NETWORK_SUPPORT 0\n")
	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "network support is disabled" )
	message( STATUS "---------------------------------------------------------------------------" )
else()
        file(WRITE  ${PROJECT_BINARY_DIR}/network_support.h "\n#define HAS_NETWORK_SUPPORT 1\n")

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

		include_directories( ${PROJECT_SOURCE_DIR}/src/3rdParty/lxqt_wallet/frontend )
		include_directories( ${PROJECT_BINARY_DIR}/src/3rdParty/lxqt_wallet/frontend )

		ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
	else()
	        if( LXQT_WALLET_VERSION VERSION_GREATER "3.1.0"  )

		        include_directories( ${LXQT_WALLET_INCLUDEDIR} )
			link_directories( ${LXQT_WALLET_LIBDIR} )

			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an external library" )
			message( STATUS "---------------------------------------------------------------------------" )
		else()
		        message( STATUS "---------------------------------------------------------------------------" )

			message( FATAL_ERROR "---------------------------------------------------------------------------\nInstalled version of lxqt_wallet is too old(<3.2.0)\nPlease install a more receant version from: https://github.com/lxqt/lxqt_wallet\n---------------------------------------------------------------------------" )

		endif()
	endif()
endif()

set( UI_FILES 	src/sirikali.ui
		src/keydialog.ui
		src/dialogmsg.ui
		src/plugin.ui
		src/createbackendwindow.ui
		src/favorites2.ui
		src/readonlywarning.ui
		src/dialogok.ui
		src/help.ui
		src/filemanager.ui
		src/debugwindow.ui
		src/configfileoption.ui
		src/configoptions.ui
		src/checkforupdateswindow.ui
		src/engines/options.ui
		src/engines/customcreateoptions.ui
		src/engines/gocryptfscreateoptions.ui
		src/engines/ecryptfscreateoptions.ui
		src/engines/securefscreateoptions.ui
		src/engines/encfscreateoptions.ui
		src/engines/cryfscreateoptions.ui
		src/engines/fscryptcreateoptions.ui
)

set( MOC_FILES
		src/configoptions.h
		src/configfileoption.h
		src/dialogok.h
		src/help.h
		src/createbackendwindow.h
		src/sirikali.h
		src/oneinstance.h
		src/keydialog.h
		src/dialogmsg.h
		src/plugin.h
		src/favorites2.h
		src/readonlywarning.h
		src/filemanager.h
		src/mountinfo.h
		src/debugwindow.h
		src/runinthread.h
		src/checkforupdateswindow.h
		src/engines/options.h
		src/engines/customcreateoptions.h
		src/engines/gocryptfscreateoptions.h
		src/engines/ecryptfscreateoptions.h
		src/engines/securefscreateoptions.h
		src/engines/encfscreateoptions.h
		src/engines/cryfscreateoptions.h
		src/engines/fscryptcreateoptions.h
 )

set( SRC 	src/dialogok.cpp
		src/main.cpp
		src/systemsignalhandler.cpp
		src/crypto.cpp
		src/runinthread.cpp
		src/debugwindow.cpp
		src/configoptions.cpp
		src/configfileoption.cpp
		src/sirikali.cpp
		src/checkforupdateswindow.cpp
		src/keydialog.cpp
		src/siritask.cpp
		src/help.cpp
		src/createbackendwindow.cpp
		src/engines.cpp
		src/oneinstance.cpp
		src/mountinfo.cpp
		src/utility.cpp
		src/dialogmsg.cpp
		src/favorites.cpp
		src/favorites2.cpp
		src/checkforupdates.cpp
		src/plugin.cpp
		src/tablewidget.cpp
		src/readonlywarning.cpp
		src/secrets.cpp
		src/filemanager.cpp
		src/utility2.cpp
		src/win.cpp
		src/processManager.cpp
		src/settings.cpp
		src/engines/options.cpp
		src/engines/cryfs.cpp
		src/engines/ecryptfs.cpp
		src/engines/gocryptfs.cpp
		src/engines/securefs.cpp
		src/engines/sshfs.cpp
		src/engines/customcreateoptions.cpp
		src/engines/encfs.cpp
		src/engines/unknown.cpp
		src/engines/custom.cpp
		src/engines/gocryptfscreateoptions.cpp
		src/engines/ecryptfscreateoptions.cpp
		src/engines/securefscreateoptions.cpp
		src/engines/encfscreateoptions.cpp
		src/engines/cryfscreateoptions.cpp
		src/engines/fscrypt.cpp
		src/engines/fscryptcreateoptions.cpp
		src/engines/cryptomator.cpp
)

if( APPLE )

	SET( MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/MacOSXBundleInfo.plist.in" )
	SET( MACOSX_BUNDLE_BUNDLE_NAME "SiriKali" )
	SET( MACOSX_BUNDLE_COPYRIGHT "(c)${COPYRIGHT_YEARS},Francis Banyikwa,mhogomchungu@gmail.com" )
	SET( MACOSX_BUNDLE_BUNDLE_VERSION "${PGR_VERSION}" )
	SET( MACOSX_BUNDLE_GUI_IDENTIFIER "com.crypto.SiriKali" )
	SET( MACOSX_BUNDLE_SHORT_VERSION_STRING "${PGR_VERSION}" )
	SET( MACOSX_BUNDLE_LONG_VERSION_STRING "${PGR_VERSION}" )
	SET( MACOSX_BUNDLE_ICON_FILE sirikali.icns )
	SET_SOURCE_FILES_PROPERTIES( icons/256x256/sirikali.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET_SOURCE_FILES_PROPERTIES( translations/en_US.qm PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET_SOURCE_FILES_PROPERTIES( translations/fr_FR.qm PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
endif()

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

		#add_executable( sirikali WIN32 ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} src/win_icon.rc )
	else()
                add_executable( sirikali ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )
	endif()
endif()

link_directories( ${GCRYPT_LIBRARY_PATH} )

if( WIN32 )
    TARGET_LINK_LIBRARIES( sirikali ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} mhogomchungu_task lxqt-wallet )
endif()

if( APPLE )
    TARGET_LINK_LIBRARIES( sirikali ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${library_pwquality} ${GCRYPT_LIBRARY} mhogomchungu_task lxqt-wallet -L${GCRYPT_LIBRARY_PATH})
endif()

if( UNIX AND NOT APPLE )
    TARGET_LINK_LIBRARIES( sirikali ${Qt5DBus_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${library_pwquality} ${GCRYPT_LIBRARY} mhogomchungu_task lxqt-wallet -L${GCRYPT_LIBRARY_PATH} -Wl,-znow)
endif()

if(NOT NONETWORKSUPPORT)
    TARGET_LINK_LIBRARIES(sirikali mhogomchungu_network)
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

	install( TARGETS sirikali BUNDLE DESTINATION . COMPONENT Runtime )

	file( COPY icons/256x256/sirikali.icns DESTINATION "sirikali.app/Contents/Resources/" )
	file( COPY translations/fr_FR.qm DESTINATION "sirikali.app/Contents/Resources/" )
	file( COPY translations/ru_RU.qm DESTINATION "sirikali.app/Contents/Resources/" )
	file( COPY translations/en_US.qm DESTINATION "sirikali.app/Contents/Resources/" )

	#file( COPY translations/de_DE.qm DESTINATION "sirikali.app/Contents/Resources/" )
	#file( COPY translations/es_MX.qm DESTINATION "sirikali.app/Contents/Resources/" )

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

	install( FILES translations/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
	install( FILES translations/ru_RU.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
	install( FILES translations/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )

	#install( FILES translations/es_MX.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )
	#install( FILES translations/de_DE.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/sirikali/translations )

	install( FILES src/sirikali.1.gz DESTINATION ${CMAKE_INSTALL_DATADIR}/man/man1/ )
	install( FILES src/sirikali.pkexec.1.gz DESTINATION ${CMAKE_INSTALL_DATADIR}/man/man1/ )

	install( FILES src/sirikali.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo/ )

endif()

if( NOT APPLE )
#install( FILES sirikali.png DESTINATION share/icons/hicolor/32x32/apps )

# desktop file section
file( WRITE ${PROJECT_BINARY_DIR}/io.github.mhogomchungu.sirikali.desktop

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
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")

install( FILES ${PROJECT_BINARY_DIR}/io.github.mhogomchungu.sirikali.desktop
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

if( UNIX AND NOT APPLE )
	if( "${POLKIT_SUPPORT}" STREQUAL "" )
		SET( POLKIT_SUPPORT "true" )
	endif()

	if( POLKIT_SUPPORT )
		ADD_SUBDIRECTORY( src/siripolkit )
	endif()
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

if( UNIX AND NOT APPLE )
	if( "${POLKIT_SUPPORT}" STREQUAL "" )
		SET( POLKIT_SUPPORT "true" )
	endif()

	if( POLKIT_SUPPORT )
		install ( FILES ${PROJECT_BINARY_DIR}/org.sirikali.pkexec.policy DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions )
	endif()
endif()

endif()

if( WIN32 )

    find_path( libstdc_PATH libstdc++-6.dll )

    find_path( gcc_PATH gcc.exe )

    find_path( windeployqt_PATH windeployqt.exe PATHS "${CMAKE_PREFIX_PATH}/bin" )

file( WRITE ${PROJECT_BINARY_DIR}/sirikali_windows_installer_commands.txt
"
set PATH=${gcc_PATH};${libstdc_PATH};%PATH%\r\n
${windeployqt_PATH}/windeployqt --release sirikali.exe\r\n
")



file( WRITE ${PROJECT_BINARY_DIR}/sirikali_windows_installer.iss
"
; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName \"SiriKali\"
#define MyAppVersion \"${PGR_VERSION}\"
#define MyAppPublisher \"Francis Banyikwa\"
#define MyAppURL \"https://mhogomchungu.github.io/sirikali\"
#define MyAppExeName \"sirikali.exe\"
#define MyLibrariesLocation \"C:\\projects\\SiriKali.libraries.Qt-${Qt5Core_VERSION}\\\"
#define MyEXELocation \"${PROJECT_BINARY_DIR}\\sirikali.exe\"
#define MyOutPutPath  \"C:\\projects\\SiriKali.installer\\Qt-${Qt5Core_VERSION}\"
#define MyGcryptLibraryPath \"C:\\projects\\sirikali\\libs\\lib\\\"
#define MySourcesPath \"C:\\projects\\sirikali\\\"

[Setup]
AppId={{677BCC48-2AC2-4CBC-B826-A94324C26988}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\\{#MyAppName}
LicenseFile={#MySourcesPath}\\LICENSE.txt
OutputDir={#MyOutPutPath}
SetupIconFile={#MySourcesPath}\\icons\\256x256\\sirikali.ico
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes
OutputBaseFilename=SiriKali-${PGR_VERSION}.setup

[Tasks]
Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked
Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: {#MyEXELocation}; DestDir: {app}; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\*.dll; DestDir: {app} ; Flags: ignoreversion

Source: {#MySourcesPath}\\LICENSE.txt; DestDir: {app}; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\bearer\\* ; DestDir: {app}\\bearer; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\iconengines\\* ; DestDir: {app}\\iconengines; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\imageformats\\*; DestDir: {app}\\imageformats; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\styles\\*; DestDir: {app}\\styles; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\platforms\\*; DestDir: {app}\\platforms; Flags: ignoreversion

Source: {#MyLibrariesLocation}\\translations\\*; DestDir: {app}\\translations; Flags: ignoreversion

Source: {#MySourcesPath}\\translations\\*.qm; DestDir: {app}\\translations; Flags: ignoreversion

Source: {#MySourcesPath}\\icons\\256x256\\sirikali.ico; DestDir: {app}\\icons; Flags: ignoreversion

; NOTE: Don't use \"Flags: ignoreversion\" on any shared system files

[Icons]
Name: \"{commonprograms}\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\sirikali.ico\"; WorkingDir: \"{app}\"
;Name: \"{commonprograms}\\Uninstall {#MyAppName}\"; Filename: \"{uninstallexe}\"; IconFilename: \"{app}\\icons\\sirikali.ico\"; WorkingDir: \"{app}\"

Name: \"{commondesktop}\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\sirikali.ico\"; WorkingDir: \"{app}\"; Tasks: desktopicon
Name: \"{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\sirikali.ico\"; WorkingDir: \"{app}\"; Tasks: quicklaunchicon

[Run]
Filename: \"{app}\\{#MyAppExeName}\"; Description: \"{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}\"; Flags: nowait postinstall skipifsilent

[Languages]
Name: \"english\"; MessagesFile: \"compiler:Default.isl\"
Name: \"brazilianportuguese\"; MessagesFile: \"compiler:Languages\\BrazilianPortuguese.isl\"
Name: \"catalan\"; MessagesFile: \"compiler:Languages\\Catalan.isl\"
Name: \"corsican\"; MessagesFile: \"compiler:Languages\\Corsican.isl\"
Name: \"czech\"; MessagesFile: \"compiler:Languages\\Czech.isl\"
Name: \"danish\"; MessagesFile: \"compiler:Languages\\Danish.isl\"
Name: \"dutch\"; MessagesFile: \"compiler:Languages\\Dutch.isl\"
Name: \"finnish\"; MessagesFile: \"compiler:Languages\\Finnish.isl\"
Name: \"french\"; MessagesFile: \"compiler:Languages\\French.isl\"
Name: \"german\"; MessagesFile: \"compiler:Languages\\German.isl\"
Name: \"hebrew\"; MessagesFile: \"compiler:Languages\\Hebrew.isl\"
Name: \"italian\"; MessagesFile: \"compiler:Languages\\Italian.isl\"
Name: \"japanese\"; MessagesFile: \"compiler:Languages\\Japanese.isl\"
Name: \"norwegian\"; MessagesFile: \"compiler:Languages\\Norwegian.isl\"
Name: \"polish\"; MessagesFile: \"compiler:Languages\\Polish.isl\"
Name: \"portuguese\"; MessagesFile: \"compiler:Languages\\Portuguese.isl\"
Name: \"russian\"; MessagesFile: \"compiler:Languages\\Russian.isl\"
Name: \"slovenian\"; MessagesFile: \"compiler:Languages\\Slovenian.isl\"
Name: \"spanish\"; MessagesFile: \"compiler:Languages\\Spanish.isl\"
Name: \"turkish\"; MessagesFile: \"compiler:Languages\\Turkish.isl\"
Name: \"ukrainian\"; MessagesFile: \"compiler:Languages\\Ukrainian.isl\"
")

endif()
