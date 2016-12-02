@echo off
call "../../vc_x86.bat"
call "clean.bat"
@echo on
set sluicedefs= /DFORMAT_LADSPA
cl ../src/win32_sluice.cpp %sluicedefs% /nologo /LD /Zi /Fesluice.dll /link /incremental:no