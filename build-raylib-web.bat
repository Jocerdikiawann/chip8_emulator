@REM cd build/external/raylib-master/src

@REM mingw32-make PLATFORM=PLATFORM_WEB -B

@REM cd ../../../../

emcc -o ./web/main.html ./src/main.c ./src/emulator.c ./src/render.c -Os -Wall -Wextra ./build/external/raylib-master/src/libraylib.web.a -I. -I./include/ -I. -I./build/external/raylib-master/src/ -L. -L./build/external/raylib-master/src/ -s USE_GLFW=3 --shell-file ./build/external/raylib-master/src/shell.html -DPLATFORM_WEB --preload-file ./resources/tetris.ch8 --preload-file ./resources/brix.ch8 -s ASYNCIFY

emrun ./web/main.html