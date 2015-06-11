#!/bin/bash

apps=(bees mix)
mods=(waves lines dsyn mix)

synopsis="usage: $0 [-h] [-a <app1>,<app2> ...] [-m <mod1>,<mod2> ...] /path/to/ALEPH"

args=`getopt ha:m: $*`
if [ $? != 0 ]; then
    echo "$synopsis"
    exit 2
fi

set -- $args
for i; do
    case "$i"
    in
	-h)
	    echo "$synopsis"; exit 0;;
	-a)
	    arg_apps=$2; shift 2;;
	-m)
	    arg_mods=$2; shift 2;;
	--)
	    shift; break;;
    esac
done

if [ -z "$1" ]; then
    echo "$synopsis"
    exit 2
fi

install_root=$1

if [ -n "$arg_apps" ]; then
    apps=(`echo $arg_apps | sed -e 's/,/ /g'`)
fi
if [ -n "$arg_mods" ]; then
    mods=(`echo $arg_mods | sed -e 's/,/ /g'`)
fi

echo "=== apps: ${apps[@]}"
echo "=== mods: ${mods[@]}"

# FIXME: eliminate the special case logic for bees below...
mkdir -pv $install_root/app
mkdir -pv $install_root/mod
mkdir -pv $install_root/data
mkdir -pv $install_root/data/bees
mkdir -pv $install_root/data/bees/scalers
mkdir -pv $install_root/data/bees/scenes

cp -v utils/param_scaling/scaler_*.dat $install_root/data/bees/scalers/
cp -v utils/release/scenes/*.scn $install_root/data/bees/scenes/
cp -v utils/release/copy.command $install_root/

# apps
for a in ${apps[@]}; do
    echo "=== app: $a"
    cp -v apps/${a}/aleph-${a}*.hex $install_root/app/
done

# modules
for m in ${mods[@]}; do
    echo "=== mod: $m"
    cp -v modules/${m}/${m}.{ldr,dsc} $install_root/mod/
done
