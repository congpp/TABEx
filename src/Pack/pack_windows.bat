@echo off
set QT_BIN=D:\Qt\5.11.1\msvc2015\bin
if not exist "%QT_BIN%\windeployqt.exe" (
    echo Qt binary dir not found: %QT_BIN%
    echo Abort~
    goto :error
)

echo Setting up environment for Qt usage...
set PATH=%QT_BIN%;%PATH%

set EDITOR_PATH=%~dp0..\bin\build-TABProject-Desktop_Qt_5_11_1_MSVC2015_32bit-Release\Editor\release\
set EDITOR_EXE=Editor.exe
set PLAYER_PATH=%~dp0..\bin\build-TABProject-Desktop_Qt_5_11_1_MSVC2015_32bit-Release\Player\release\
set PLAYER_EXE=Player.exe
set PACK_DIR=pack_windows

if exist %PACK_DIR% rd /s /q %PACK_DIR%
mkdir pack_windows
pushd "%~dp0%PACK_DIR%"

copy "%EDITOR_PATH%%EDITOR_EXE%" %EDITOR_EXE%
if %errorlevel% neq 0 goto :error
windeployqt.exe %EDITOR_EXE%
if %errorlevel% neq 0 goto :error


copy "%PLAYER_PATH%%PLAYER_EXE%" %PLAYER_EXE%
if %errorlevel% neq 0 goto :error
windeployqt.exe "%PLAYER_EXE%"
if %errorlevel% neq 0 goto :error

goto :ok

:error
echo ========================================================
echo PACK ERROR
popd
pause
goto :eof

:ok
echo ========================================================
echo OK
goto :eof