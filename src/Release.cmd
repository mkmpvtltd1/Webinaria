@echo off

rem The script builds and packages Release configuration of Webinaria.
rem Refer to buildlog.txt for compilation results.

call "%VS80COMNTOOLS%\vsvars32.bat"

echo BUILDING Webinaria...

set WebinariaConfiguration=Release
echo Build configuration: %WebinariaConfiguration%

msbuild /p:Configuration=%WebinariaConfiguration% /t:Rebuild > ..\bin\buildlog.txt

set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 GOTO BuildFailed

:Package

pushd WebinariaSetup

echo PACKAGING...
"C:\Program Files\Inno Setup 5\ISCC.exe" WebinariaSetup.iss

popd

echo Build SUCCESSFULLY finished!

pause

GOTO Stop

:BuildFailed
echo Build FAILED, cannot continue.

:Stop
