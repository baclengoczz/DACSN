@echo off

if "%CD%"=="" set MCC_INCLUDE=..\..\..\..\h
if "%MCC_INCLUDE%"=="" set MCC_INCLUDE=%CD%\..\..\h
if "%1"=="" (
  echo usage: makeonep.bat procname
  goto EOF
)

if exist p%1.lib del p%1.lib
mplib /q /c p%1.lib

echo Building modules for p%1...

REM Using a "for" loop to delete files is cleaner than using a wildcard
REM because if there are no files to delete, no error message will be
REM displayed.


echo ...processor definition module...
cd proc
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 p%1.asm
for %%f in (*.ERR) do type %%f
mplib /q /r ..\p%1.lib p%1.o

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

cd pmc

echo ...USART module...
cd USART
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

cd ..

echo Installing library p%1.lib...
cd ..\..\lib
IF EXIST p%1.lib del p%1.lib
move ..\src\traditional\p%1.lib .
cd ..\src\traditional

:EOF
