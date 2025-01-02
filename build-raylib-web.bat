@echo off

set BUILD_RAYLIB=false

:parse_args
if "%1"=="-with" (
    if /i "%2"=="raylib" (
        set BUILD_RAYLIB=true
    )
    shift
)
shift
if not "%1"=="" goto parse_args

if %BUILD_RAYLIB%==true (
    cd build/external/raylib-master/src
    mingw32-make PLATFORM=PLATFORM_WEB -B
    cd ../../../../
)

emcc -o ./web/main.html ./src/main.c ./src/emulator.c ./src/render.c -Os -Wall -Wextra ^
    ./build/external/raylib-master/src/libraylib.web.a -I. -I./include/ -I. ^
    -I./build/external/raylib-master/src/ -L. -L./build/external/raylib-master/src/ ^
    -s USE_GLFW=3 --shell-file ./build/external/raylib-master/src/shell.html -DPLATFORM_WEB ^
    --preload-file ./resources/tetris.ch8 --preload-file ./resources/brix.ch8 ^
    -s ASYNCIFY -s FORCE_FILESYTEM=1 --profiling

if %ERRORLEVEL% EQU 0 (
    start emrun ./web/main.html
)