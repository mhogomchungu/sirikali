

$QT_PATH="C:\Qt\5.15.2\mingw81_32"

$QT_PATH_BIN="$QT_PATH\bin"

$GCC_PATH="C:\Qt\Tools\mingw810_32\bin"
$NINJA_PATH="C:\Qt\Tools\Ninja"
$CMAKE_PATH="C:\Qt\Tools\CMake_64\bin"
$GIT_PATH="C:\Program Files (x86)\Git\bin"
$INNO_PATH="C:\Program Files (x86)\Inno Setup 6"
$SEVEN_ZIP_PATH="C:\projects\7-Zip"

$SRC_LOCATION=$PSScriptRoot

$BUILD_PATH="$SRC_LOCATION/BUILD"

$env:Path = "$GCC_PATH;$SEVEN_ZIP_PATH;$QT_PATH_BIN;$NINJA_PATH;$CMAKE_PATH;$INNO_PATH;$GIT_PATH" + $env:Path

Set-Location -Path $SRC_LOCATION

if(Test-Path -Path $BUILD_PATH/SiriKali)
{
   Remove-Item -Path $BUILD_PATH/SiriKali -Recurse
}

New-Item -ItemType Directory -Path $BUILD_PATH/SiriKali

cmake.exe -DCMAKE_VERBOSE_MAKEFILE=FALSE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_PREFIX_PATH="$QT_PATH" -DLIBRARIES_LOCATION="$BUILD_PATH" -DOUTPUT_PATH="$BUILD_PATH" -DSOURCE_PATH="$SRC_LOCATION" -G "CodeBlocks - MinGW Makefiles" -S "$SRC_LOCATION" -B "$BUILD_PATH/SiriKali"

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

$data = [System.IO.File]::ReadAllText("$BUILD_PATH/SiriKali/version.h")

$versionInfoLines = $data.Split([Environment]::NewLine, [System.StringSplitOptions]::RemoveEmptyEntries)

$versionInfoFirstLine = $versionInfoLines[1]

$versionInfo = $versionInfoFirstLine.Split(" ",[System.StringSplitOptions]::RemoveEmptyEntries)[2]

$VERSION = $versionInfo.Replace(".git_tag","")

$VERSION = $VERSION.Replace('"',"")

cmake.exe -DCMAKE_VERBOSE_MAKEFILE=FALSE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_PREFIX_PATH="$QT_PATH" -DLIBRARIES_LOCATION="$BUILD_PATH/SiriKali-$VERSION" -DOUTPUT_PATH="$BUILD_PATH" -DSOURCE_PATH="$SRC_LOCATION" -G "CodeBlocks - MinGW Makefiles" -S "$SRC_LOCATION" -B "$BUILD_PATH/SiriKali"

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

if(Test-Path -Path $BUILD_PATH/SiriKali-$VERSION)
{
   Remove-Item -Path $BUILD_PATH/SiriKali-$VERSION -Recurse
}

cmake --build $BUILD_PATH/SiriKali

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

New-Item -ItemType Directory -Path $BUILD_PATH/SiriKali-$VERSION

Copy-Item -Path $SRC_LOCATION/translations -Destination $BUILD_PATH/SiriKali-$VERSION -Recurse
Copy-Item -Path $BUILD_PATH/SiriKali/sirikali.exe -Destination $BUILD_PATH/SiriKali-$VERSION
Copy-Item -Path $BUILD_PATH/SiriKali/src/sirikali_cppcryptfs/sirikali_cppcryptfs.exe -Destination $BUILD_PATH/SiriKali-$VERSION

windeployqt.exe $BUILD_PATH/SiriKali-$VERSION/sirikali.exe --libdir $BUILD_PATH/SiriKali-$VERSION --plugindir $BUILD_PATH/SiriKali-$VERSION

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

iscc $BUILD_PATH\SiriKali\sirikali_windows_installer_Qt5.iss

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

Write-Output "$VERSION" | Out-File -FilePath $BUILD_PATH/SiriKali-$VERSION/version_info.txt -Encoding utf8 -Append

New-Item -ItemType Directory -Path $BUILD_PATH/SiriKali-$VERSION/local

7z.exe a $BUILD_PATH/SiriKali-$VERSION.zip $BUILD_PATH/SiriKali-$VERSION

if($?)
{
   Read-Host "SUCCESS!! Press Enter To Exit"
}
else
{
   Read-Host "Press Enter To Exit"
}
