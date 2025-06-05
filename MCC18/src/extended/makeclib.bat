@echo off

if "%CD%"=="" set MCC_INCLUDE=..\..\..\h
if "%MCC_INCLUDE%"=="" set MCC_INCLUDE=%CD%\..\..\h

if exist clib_e.lib del clib_e.lib
mplib /q /c clib_e.lib

REM Using a "for" loop to delete files is cleaner than using a wildcard
REM because if there are no files to delete, no error message will be
REM displayed.

echo Building library modules...

echo ...math module... 
cd math
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /w1 /y /rDEC /l- /o /q /d__LARGE__ /p18cxx %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=18cxx %%i
for %%i in (*.o) do mplib /q /r ..\clib_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...standard C library module...
cd stdclib
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /w1 /y /rDEC /l- /o /q /d__LARGE__ /p18cxx %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=18cxx %%i
for %%i in (*.o) do mplib /q /r ..\clib_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...delay module...
cd delays
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /w1 /y /rDEC /l- /o /q /d__LARGE__ /p18cxx %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=18cxx %%i
for %%i in (*.o) do mplib /q /r ..\clib_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo Installing library...
cd ..\..\lib
IF EXIST clib_e.lib del clib_e.lib
move ..\src\extended\clib_e.lib .
cd ..\src\extended

