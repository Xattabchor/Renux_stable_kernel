#!/bin/sh
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
SDK_ROOT=${1:-"$HERE/../../SDK_BIN/Graphics_SDK_setuplinux_4_03_00_02/GFX_Linux_SDK"}
SOURCE="$SDK_ROOT/OGLES2/SDKPackage/Demos/ChameleonMan/OGLES2"
DEST="$HERE/OGLES/Media"

if [ ! -d "$SOURCE" ]; then
    echo "Chameleon Man source assets not found: $SOURCE" >&2
    echo "Pass the extracted GFX_Linux_SDK directory as the first argument." >&2
    exit 1
fi

mkdir -p "$DEST"
for asset in \
    ChameleonScene.pod \
    FinalChameleonManHeadBody.pvr FinalChameleonManLegs.pvr \
    Tang_space_BodyMap.pvr Tang_space_LegsMap.pvr Tang_space_BeltMap.pvr \
    skyline.pvr Wall_diffuse_baked.pvr lamp.pvr ChameleonBelt.pvr
do
    if [ ! -f "$SOURCE/$asset" ]; then
        echo "Missing Chameleon Man SDK asset: $SOURCE/$asset" >&2
        exit 1
    fi
    cp -f "$SOURCE/$asset" "$DEST/$asset"
done

echo "Copied the OGLES2 demo's POD and texture assets into $DEST"
