@echo off
echo BUILDING EXTENDED MODE LIBRARIES
cd extended
call makeall.bat
cd ..

echo BUILDING TRADITIONAL MODE LIBRARIES
cd traditional
call makeall.bat
cd ..


