@echo off
:: -------------------------------------------------------
:: Windows build script — MinGW-w64 or NMake (MSVC)
:: Usage:
::   buildWin.bat          — auto-detect (MinGW preferred)
::   buildWin.bat mingw    — force MinGW Makefiles
::   buildWin.bat nmake    — force NMake Makefiles
:: -------------------------------------------------------

set MODE=%1

if /i "%MODE%"=="mingw" goto USE_MINGW
if /i "%MODE%"=="nmake" goto USE_NMAKE

:: Auto-detect: prefer MinGW if gcc is on PATH
where gcc >nul 2>&1
if %ERRORLEVEL%==0 goto USE_MINGW
goto USE_NMAKE

:: -------------------------------------------------------
:USE_MINGW
echo [buildWin] Generator: MinGW Makefiles
cmake -S . -B build ^
      -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_C_COMPILER=gcc ^
      -DCMAKE_CXX_COMPILER=g++
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cmake --build build
goto END

:: -------------------------------------------------------
:USE_NMAKE
echo [buildWin] Generator: NMake Makefiles
cmake -S . -B build ^
      -G "NMake Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cmake --build build
goto END

:: -------------------------------------------------------
:END
echo.
echo [buildWin] Done: build\CSplatoon.exe
