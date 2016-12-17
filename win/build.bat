@echo off
call "../../vc_x86.bat"
call "clean.bat"
@echo on
cl ../src/win32_sluice.c /nologo /Zi /LD ^
/link /incremental:no /out:sluice.dll
