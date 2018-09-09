#!/bin/bash
echo Building...
make
echo Building complete
read -sn1 -p "Start \"the homebrew launcher\" and press Y on the 3DS, Then press any key on the keyboard to copy."
echo
echo Copying + starting...
3dslink MathVisualizer.3dsx
echo Done
