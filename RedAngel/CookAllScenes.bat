@echo off
setlocal enabledelayedexpansion

set APP_ROOT=%~dp0
set ASSET_TYPE=scene
set SCENE_DIR=%APP_ROOT%\Assets\Scene
set COOKER=%APP_ROOT%..\noodle\Lib\x64\Debug\noodleCooker.exe

for /R %SCENE_DIR% %%f in (*.json) do (
	echo Cooking %%f
	"%COOKER%" %ASSET_TYPE% %APP_ROOT% Scene\%%~nf
)

echo Done.
pause