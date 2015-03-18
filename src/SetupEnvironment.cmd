@echo off

rem This script will copy all files you will need to build, run and debug Webinaria.

md ..\bin

xcopy ..\lib\ffmpeg\*.* ..\bin\ffmpeg\ /S /Y
copy ..\lib\htmlayout\htmlayout.dll ..\bin\htmlayout.dll /Y
copy ..\lib\UScreenCapture\UScreenCapture.ax ..\bin\UScreenCapture.ax /Y

copy Webinaria\app.ico ..\bin\app.ico
copy Webinaria\app.ico ..\bin\tray.ico
