#!/bin/sh

echo "Compilando ZIPI..."
rm -f lasonda.ztm~
LANG=C awk -f zipi.awk lasonda.ztm > Pistas.h
#cp -f Pistas.h ..
#cd ..
#ln -sf ZIPI_NG/Pistas.h .
#cd ..
#./compilar.sh

