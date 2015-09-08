@echo off
if not exist "%~1\configure" (
	echo DDX-QT: First argument must be Qt source directory!
	exit /b 1
) else set "ddxqt=%~1"
if not exist "%~2\openssl.doxy" (
	echo DDX-QT: Second argument must be OpenSSL directory!
	exit /b 1
) else set "ddxssl=%~2"
if not exist "%~3" (
	echo DDX-QT: Third argument must be the install directory!
	exit /b 1
) else set "ddxinstall=%~3"
if "%~4"=="/j" (set "ddxmake=jom") else (set "ddxmake=nmake")

echo DDX-QT: About to build Qt; this will overwrite existing DDX-QT builds
echo DDX-QT: Google "ActivePerl" and download for perl
echo DDX-QT: Jom will speed up builds: http://wiki.qt.io/Jom
echo.
echo CURRENTLY ONLY QT 5.5 IS SUPPORTED
echo.
if "%ddxmake%"=="jom" (
	echo Using jom; ensure it is in PATH
) else (
	echo Using nmake; specify "/j" as fourth argument for jom
)
echo.
pause
echo.
set "ddxreturndir=%CD%"
set "ddxexitcode=1"
cd /d "%ddxqt%"
set "PATH=%PATH%;%ddxssl%"
rem jom distclean
rem goto :skipReset
if exist "ddx-build" rmdir /s /q "ddx-build"
if exist "ddx-build" (
	echo DDX-QT: Could not delete ddx-build directory from %ddxqt%
	goto quit
)
mkdir "ddx-build"
if not exist "ddx-build" (
	echo DDX-QT: Could not make the build directory in %ddxqt%
	goto quit
)
:skipReset
cd "ddx-build"
echo DDX-QT: Building win32 qt
echo DDX-QT: Configuring...
call "%ddxqt%\configure" -prefix "%ddxinstall%\DDX-QT\ddxqt-win32" -platform win32-g++ -xplatform win32-g++ -openssl-linked -nomake examples -nomake tests -opensource
echo DDX-QT: Making...
jom
echo DDX-QT: Installing...
jom install






echo DDX-QT: Done!
set "ddxexitcode=0"
:quit
cd /d "%ddxreturndir%"
exit /b %ddxexitcode%


"
exit /b 1

SET _ROOT=C:\qt\qt-5
SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;%PATH%
REM Uncomment the below line when using a git checkout of the source repository
REM SET PATH=%_ROOT%\qtrepotools\bin;%PATH%
SET QMAKESPEC=win32-msvc2013
SET _ROOT=
10 4
15 6