#!/bin/sh

cd Alpha_pack/ZIPI_NG
./compilar-zipi.sh
cd ../..
front elcirculo elcirculo.inf $* +language_name=Spanish +include_path=,lib,INFSP6,InformLibrary611,Alpha_pack,Alpha_pack/ZIPI_NG,Damusix
meta.sh

