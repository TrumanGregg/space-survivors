@echo off

if NOT "%Platform%" == "X64" IF NOT "%Platform%" == "x64" (call "shell.bat")

if not exist C:\space-survivors\bin mkdir C:\space-survivors\bin
pushd C:\space-survivors\bin

set CompilerFlags= -nologo -Od -MDd -Zi -WX -W4 -Oi -IC:\raylib\src
set LinkerFlags= -incremental:no -opt:ref -LIBPATH:C:\raylib\src raylib.lib winmm.lib gdi32.lib user32.lib shell32.lib

del *.pdb > NUL 2> NUL
cl %CompilerFlags% C:\space-survivors\src\ss.c -link %LinkerFlags%

popd