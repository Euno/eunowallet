#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/euno.ico

convert ../../src/qt/res/icons/euno-16.png ../../src/qt/res/icons/euno-80.png ${ICON_DST}
