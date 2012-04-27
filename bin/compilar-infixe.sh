#!/bin/sh

rm -f elcirculo.blb elcirculo.ulx
cd Alpha_pack/ZIPI_NG
./compilar-zipi.sh
cd ../..
bres elcirculo.res elcirculo.blc elcirculo.bli
inform-infixe -E1 -D -X $* \$MAX_OBJ_PROP_COUNT=100 \$MAX_LABELS=2000 +language_name=Spanish +include_path=,lib,INFSP6,InformLibrary611,Alpha_pack,Alpha_pack/ZIPI_NG,Alpha_pack/SGW+DMX_NG,Damusix elcirculo.inf
if [ -f elcirculo.ulx ]; then
#  meta.sh
  blc elcirculo.blc elcirculo.blb
fi
cp -f elcirculo.blb ~/Dropbox/AlphaAventuras/ElCirculo/

