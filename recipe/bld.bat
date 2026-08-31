@echo on
setlocal enabledelayedexpansion

if not exist "%LIBRARY_INC%" mkdir "%LIBRARY_INC%"
if not exist "%LIBRARY_LIB%" mkdir "%LIBRARY_LIB%"
if not exist "%LIBRARY_BIN%" mkdir "%LIBRARY_BIN%"

xcopy /E /I /Y include "%LIBRARY_INC%"
if errorlevel 1 exit 1

copy /Y lib\wasmtime.dll "%LIBRARY_BIN%\wasmtime.dll"
if errorlevel 1 exit 1

REM wasmtime.dll.lib is the IMPORT library and wasmtime.lib is the static
REM one. It is installed under the name a consumer looks for, so
REM find_library(wasmtime) resolves to the shared build -- which is the
REM one this package ships.
copy /Y lib\wasmtime.dll.lib "%LIBRARY_LIB%\wasmtime.lib"
if errorlevel 1 exit 1
