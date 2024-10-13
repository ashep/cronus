#!/bin/bash

mkdir -p ./gcode
rm -f ./gcode/*.ngc

# The board is single sided, currently
# pcb2gcode --config pcb2gcode.ini --front ./gerber/Cronus-F_Cu.gbr --y-offset 0.5
pcb2gcode --config pcb2gcode.ini --back ./gerber/Cronus-B_Cu.gbr --x-offset -71.0 --y-offset 0.5
pcb2gcode --config pcb2gcode.ini --outline ./gerber/Cronus-Edge_Cuts.gbr --x-offset -71.0 --y-offset 0.5
pcb2gcode --config pcb2gcode.ini --drill ./gerber/Cronus-PTH.drl --drill-output drill_1.ngc --x-offset -71.0 --y-offset 0.5
pcb2gcode --config pcb2gcode.ini --drill ./gerber/Cronus-NPTH.drl --drill-output drill_2.ngc --x-offset -71.0 --y-offset 0.5

rm -f ./gcode/*.svg
