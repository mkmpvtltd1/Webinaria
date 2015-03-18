@echo off

pushd

cd %1

md ..\bin\UI

xcopy common\UI\*.* ..\bin\UI /S /Y /EXCLUDE:common\UI\exclude.files

popd
