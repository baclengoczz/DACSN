@echo off

echo Building startup modules...
cd startup
for %%f in (*.o) do del %%f 
for %%f in (*.err) do del %%f 
mcc18 -ls --no-extended -ml -p=18cxx c018i.c
mcc18 -ls --no-extended -ml -p=18cxx c018iz.c
mcc18 -ls --no-extended -ml -p=18cxx c018.c
echo Installing startup modules...
if EXIST ..\..\..\lib\c018i.o del ..\..\..\lib\c018i.o
if EXIST ..\..\..\lib\c018iz.o del ..\..\..\lib\c018iz.o
if EXIST ..\..\..\lib\c018.o del ..\..\..\lib\c018.o
move c018i.o ..\..\..\lib
move c018iz.o ..\..\..\lib
move c018.o ..\..\..\lib
for %%f in (*.err) do del %%f 
cd ..
