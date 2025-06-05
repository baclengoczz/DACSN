@echo off

if "%CD%"=="" set MCC_INCLUDE=..\..\..\..\h
if "%MCC_INCLUDE%"=="" set MCC_INCLUDE=%CD%\..\..\h
if "%1"=="" (
  echo usage: makeonep.bat procname
  goto EOF
)

if exist p%1_e.lib del p%1_e.lib
mplib /q /c p%1_e.lib

echo Building modules for p%1 (Extended Mode)...

REM Using a "for" loop to delete files is cleaner than using a wildcard
REM because if there are no files to delete, no error message will be
REM displayed.


echo ...processor definition module...
cd proc
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 p%1.asm
for %%f in (*.ERR) do type %%f
mplib /q /r ..\p%1_e.lib p%1.o

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

cd pmc

echo ...XLCD module...
cd XLCD
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...CCP module...
cd CCP
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (cp1*.c) do mcc18 -ls --extended -ml -p=%1 %%i
mcc18 -ls --extended -ml -p=%1 cap18def.c 
if "%1"=="18f2480" goto skip1
if "%1"=="18f2580" goto skip1
if "%1"=="18f2585" goto skip1
if "%1"=="18f2680" goto skip1
for %%i in (ep*.c) do mcc18 -ls --extended -ml -p=%1 %%i
:skip1
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...PWM module...
cd PWM
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
mcc18 -ls --extended -ml -p=%1 pw1open.c 
mcc18 -ls --extended -ml -p=%1 pw1setdc.c 
mcc18 -ls --extended -ml -p=%1 pw1close.c 
if "%1"=="18f2480" goto skip2
if "%1"=="18f2580" goto skip2
if "%1"=="18f2585" goto skip2
if "%1"=="18f2680" goto skip2
mcc18 -ls --extended -ml -p=%1 ew1open.c 
mcc18 -ls --extended -ml -p=%1 ew1setdc.c 
mcc18 -ls --extended -ml -p=%1 ew1setoc.c 
mcc18 -ls --extended -ml -p=%1 ew1close.c 
:skip2
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...I2C module...
cd i2c
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (i2c_*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...Microwire module...
cd mwire
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (mw_*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...SPI module...
cd SPI
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (spi_*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...software SPI module...
cd SW_SPI
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...software UART module...
cd SW_UART
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...software I2C module...
cd SW_I2C
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...timers module...
cd Timers
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (t0*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (t1*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (t2*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (t3*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...USART module...
cd USART
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
mcc18 -ls --extended -ml -p=%1 uopen.c 
mcc18 -ls --extended -ml -p=%1 uread.c 
mcc18 -ls --extended -ml -p=%1 uwrite.c 
mcc18 -ls --extended -ml -p=%1 udrdy.c 
mcc18 -ls --extended -ml -p=%1 ubusy.c 
mcc18 -ls --extended -ml -p=%1 ugets.c 
mcc18 -ls --extended -ml -p=%1 uputs.c 
mcc18 -ls --extended -ml -p=%1 uputrs.c 
mcc18 -ls --extended -ml -p=%1 ubaud.c 
mcc18 -ls --extended -ml -p=%1 usartd.c 
mcc18 -ls --extended -ml -p=%1 uclose.c 
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...ADC module...
cd ADC
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...reset module...
cd reset
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...PORTB module...
cd PORTB
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...CAN 2510 module...
cd CAN2510
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasm /y /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1_e.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

cd ..

echo Installing library p%1_e.lib...
cd ..\..\lib
IF EXIST p%1_e.lib del p%1_e.lib
move ..\src\extended\p%1_e.lib .
cd ..\src\extended

:EOF
