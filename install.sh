#! /bin/sh

mkdir $1/app
mkdir $1/mod
mkdir $1/data
mkdir $1/data/bees
mkdir $1/data/bees/scalers
mkdir $1/data/bees/scenes/

cp utils/param_scaling/scaler_*.dat $1/data/bees/scalers/
cp apps/bees/aleph-bees*.hex $1/app/
cp modules/waves/aleph-waves.ldr $1/mod/
cp modules/waves/aleph-waves.dsc $1/mod/
cp modules/waves/aleph-lines.ldr $1/mod/
cp modules/waves/aleph-lines.dsc $1/mod/