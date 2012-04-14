#!/bin/sh
blc meta.blc meta.blb
bmerge meta.blb elcirculo.blb elcirculo.gblorb
rm elcirculo.blb
mv elcirculo.gblorb elcirculo.blb

