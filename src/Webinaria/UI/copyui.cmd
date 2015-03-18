@echo off

pushd

cd %1

call common\UI\copyui.cmd

md ..\bin\UI\Recorder

xcopy Webinaria\UI\*.* ..\bin\UI\Recorder /S /Y /EXCLUDE:common\UI\exclude.files

popd

exit 0
