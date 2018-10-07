
pushd build
cl /Zi /I ..\code\src /I ..\code\include ..\code\win32_startup.cpp opengl32.lib
popd
