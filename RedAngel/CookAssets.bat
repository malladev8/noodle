@echo off
setlocal enabledelayedexpansion

set APP_ROOT=%~dp0
set ENGINE_ROOT=%APP_ROOT%../noodle/
set SCENE_DIR=%APP_ROOT%/Assets/Scene
set COOKER=%APP_ROOT%../noodleCooker/Cooker/x64/Debug/noodleCooker.exe

rem Cook scenes.
for /R %SCENE_DIR% %%f in (*.json) do (
    echo Cooking %%f
    "%COOKER%" scene %APP_ROOT% %APP_ROOT% Scene/%%~nf
)

rem Cook engine debug shader.
echo Cooking DebugAABB shader
"%COOKER%" shader %ENGINE_ROOT% %APP_ROOT% Shader/DebugBoxShader

echo Done.
pause