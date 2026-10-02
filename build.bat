@echo off
del build/*.o >nul
del build/main.exe >nul
mingw32-make -j8 all
cd build
main.exe