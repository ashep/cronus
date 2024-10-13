#!/bin/bash

rm -f gcode/*.ngc

pcb2gcode --config pcb2gcode.ini --front ./gerber/Gerber_TopLayer.GTL --back ./gerber/Gerber_BottomLayer.GBL --outline ./gerber/Gerber_BoardOutlineLayer.GKO
pcb2gcode --config pcb2gcode.ini --drill ./gerber/Drill_PTH_Through.DRL --drill-output drill_1.ngc
pcb2gcode --config pcb2gcode.ini --drill ./gerber/Drill_NPTH_Through.DRL --drill-output drill_2.ngc

rm -rf ./gcode/*.svg