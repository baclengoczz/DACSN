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

echo ...XLCD module...
cd XLCD
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...CCP module...
cd CCP
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (cp1*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (cp2*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (cp3*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (cp4*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (cp5*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
mcc18 -ls --no-extended -ml -p=%1 cap18def.c 
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...PWM module...
cd PWM
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
mcc18 -ls --no-extended -ml -p=%1 pw1open.c 
mcc18 -ls --no-extended -ml -p=%1 pw1setdc.c 
mcc18 -ls --no-extended -ml -p=%1 pw1setoc.c 
mcc18 -ls --no-extended -ml -p=%1 pw1close.c 
mcc18 -ls --no-extended -ml -p=%1 pw2open.c 
mcc18 -ls --no-extended -ml -p=%1 pw2setdc.c 
mcc18 -ls --no-extended -ml -p=%1 pw2setoc.c 
mcc18 -ls --no-extended -ml -p=%1 pw2close.c 
mcc18 -ls --no-extended -ml -p=%1 pw3open.c 
mcc18 -ls --no-extended -ml -p=%1 pw3setdc.c 
mcc18 -ls --no-extended -ml -p=%1 pw3setoc.c 
mcc18 -ls --no-extended -ml -p=%1 pw3close.c 
mcc18 -ls --no-extended -ml -p=%1 pw4open.c 
mcc18 -ls --no-extended -ml -p=%1 pw4setdc.c 
mcc18 -ls --no-extended -ml -p=%1 pw4close.c 
mcc18 -ls --no-extended -ml -p=%1 pw5open.c 
mcc18 -ls --no-extended -ml -p=%1 pw5setdc.c 
mcc18 -ls --no-extended -ml -p=%1 pw5close.c 
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...I2C module...
cd i2c
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (i2c1*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (i2c2*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...Microwire module...
cd mwire
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (mw1*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (mw2*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...SPI module...
cd SPI
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (spi1*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (spi2*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...software SPI module...
cd SW_SPI
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...software UART module...
cd SW_UART
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...software I2C module...
cd SW_I2C
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...timers module...
cd Timers
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (t0*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (t1*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (t2*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (t3*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (t4*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...USART module...
cd USART
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
mcc18 -ls --no-extended -ml -p=%1 u1open.c 
mcc18 -ls --no-extended -ml -p=%1 u1read.c 
mcc18 -ls --no-extended -ml -p=%1 u1write.c 
mcc18 -ls --no-extended -ml -p=%1 u1drdy.c 
mcc18 -ls --no-extended -ml -p=%1 u1busy.c 
mcc18 -ls --no-extended -ml -p=%1 u1gets.c 
mcc18 -ls --no-extended -ml -p=%1 u1puts.c 
mcc18 -ls --no-extended -ml -p=%1 u1putrs.c 
mcc18 -ls --no-extended -ml -p=%1 u1baud.c 
mcc18 -ls --no-extended -ml -p=%1 u1defs.c 
mcc18 -ls --no-extended -ml -p=%1 u1close.c 
mcc18 -ls --no-extended -ml -p=%1 u2open.c 
mcc18 -ls --no-extended -ml -p=%1 u2read.c 
mcc18 -ls --no-extended -ml -p=%1 u2write.c 
mcc18 -ls --no-extended -ml -p=%1 u2drdy.c 
mcc18 -ls --no-extended -ml -p=%1 u2busy.c 
mcc18 -ls --no-extended -ml -p=%1 u2gets.c 
mcc18 -ls --no-extended -ml -p=%1 u2puts.c 
mcc18 -ls --no-extended -ml -p=%1 u2putrs.c 
mcc18 -ls --no-extended -ml -p=%1 u2baud.c 
mcc18 -ls --no-extended -ml -p=%1 u2defs.c 
mcc18 -ls --no-extended -ml -p=%1 u2close.c 
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...ADC module...
cd ADC
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...reset module...
cd reset
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
mcc18 -ls --no-extended -ml -p=%1 isbor.c 
mcc18 -ls --no-extended -ml -p=%1 ismclr.c 
mcc18 -ls --no-extended -ml -p=%1 ispor.c 
mcc18 -ls --no-extended -ml -p=%1 iswdtto.c 
mcc18 -ls --no-extended -ml -p=%1 iswdtwu.c 
mcc18 -ls --no-extended -ml -p=%1 iswu.c 
mcc18 -ls --no-extended -ml -p=%1 statrst.c 
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...PORTB module...
cd PORTB
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
for %%i in (*.o) do mplib /q /r ..\..\p%1.lib %%i

for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f
cd ..

echo ...CAN 2510 module...
cd CAN2510
for %%f in (*.o) do del %%f
for %%f in (*.err) do del %%f
for %%f in (*.lst) do del %%f

for %%i in (*.asm) do mpasmwin /rDEC /l- /o /q /d__LARGE__ /p%1 %%i
for %%f in (*.ERR) do type %%f
for %%i in (*.c) do mcc18 -ls --no-extended -ml -p=%1 %%i
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
