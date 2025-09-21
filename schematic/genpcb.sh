#!/bin/bash

set -e

mkdir -p ./gcode
rm -f ./gcode/*.ngc

# Back drills
pcb2gcode --config pcb2gcode.ini --drill ./gerber/Cronus-PTH.drl \
    --drill-side back \
    --x-offset -72.0 \
    --drill-output drill-back.ngc \
    --drills-available 0.9,2.2 \
    --drill-feed 50

# Back mount holes, 2.2mm
./splitdrill.py ./gcode/drill-back.ngc ./gcode/01-back-drill-2.2mm.ngc T4
rm gcode/drill-back.ngc

# Front drills
pcb2gcode --config pcb2gcode.ini --drill ./gerber/Cronus-PTH.drl \
    --drill-side front \
    --drill-output drill-front.ngc \
    --drills-available 0.9,2.2 \
    --drill-feed 250

# Front holes, 0.9mm
./splitdrill.py ./gcode/drill-front.ngc ./gcode/04-front-drill-0.9mm.ngc T1
rm gcode/drill-front.ngc

# Back traces
pcb2gcode --config pcb2gcode.ini --back ./gerber/Cronus-B_Cu.gbr --back-output 02-back-mill.ngc --x-offset -72.0

# Front traces
pcb2gcode --config pcb2gcode.ini --front ./gerber/Cronus-F_Cu.gbr --front-output 03-front-mill.ngc

# Outline
pcb2gcode --config pcb2gcode.ini --outline ./gerber/Cronus-Edge_Cuts.gbr --outline-output 05-front-outline.ngc

rm -f ./gcode/*.svg
