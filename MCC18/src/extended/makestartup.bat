@echo off

echo Building startup modules...
cd startup
for %%f in (*.o) do del %%f 
for %%f in (*.err) do del %%f 
mcc18 -ls --extended -ml -p=18cxx c018i_e.c
mcc18 -ls --extended -ml -p=18cxx c018iz_e.c
mcc18 -ls --extended -ml -p=18cxx c018_e.c
echo Installing startup modules...
if EXIST ..\..\..\lib\c018i_e.o del ..\..\..\lib\c018i_e.o
if EXIST ..\..\..\lib\c018iz_e.o del ..\..\..\lib\c018iz_e.o
if EXIST ..\..\..\lib\c018_e.o del ..\..\..\lib\c018_e.o
move c018i_e.o ..\..\..\lib
move c018iz_e.o ..\..\..\lib
move c018_e.o ..\..\..\lib
for %%f in (*.err) do del %%f 
cd ..
