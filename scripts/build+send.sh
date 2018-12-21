#!/bin/bash
make
if [ $? -eq 0 ]
then
    3dslink MathVisualizer.3dsx
else
    echo "There was an error while building, not sending to 3DS"
fi