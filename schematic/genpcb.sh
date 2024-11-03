#!/bin/bash

mkdir -p ./gcode
rm -f ./gcode/*.ngc

pcb2gcode --config pcb2gcode.ini --back ./gerber/Cronus-B_Cu.gbr --x-offset -72.0

pcb2gcode --config pcb2gcode.ini --drill ./gerber/Cronus-PTH.drl --drill-output drill_0_9mm.ngc --x-offset -72.0 --drill-feed 250

pcb2gcode --config pcb2gcode.ini --drill ./gerber/Cronus-NPTH.drl --drill-output drill_2_2mm.ngc --x-offset -72.0 --drill-feed 50

pcb2gcode --config pcb2gcode.ini --outline ./gerber/Cronus-Edge_Cuts.gbr --x-offset -72.0

rm -f ./gcode/*.svg
