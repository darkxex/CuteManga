@RD /S /Q "C:\respaldo2017\CuteMangaNX\build"
xcopy "C:\respaldo2017\C++\CuteManga\test\*.cpp" "C:\respaldo2017\CuteMangaNX\source\*.cpp" /y
make
nxlink -s "out/CuteManga.nro"