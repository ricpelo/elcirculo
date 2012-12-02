#!/bin/sh

compilar-infixe.sh
cd ~/src/garglk/build/dist
export LD_LIBRARY_PATH=.
./gargoyle ~/IF/INFSP6/ElCirculo/elcirculo.blb

