@echo off
setlocal

pushd "%~dp0"

where.exe mingw32-make >nul 2>&1
if %errorlevel%==0 (
    mingw32-make
) else (
    echo mingw32-make was not found in PATH.
    popd
    exit /b 1
)

if errorlevel 1 (
    popd
    exit /b %errorlevel%
)

.\bin\graphe.exe .\res\graphe.gph
set "exit_code=%errorlevel%"

popd
exit /b %exit_code%