# Cronus

## Schematic diagram

Look at the [schematic/pdf/Cronus.pdf](schematic/pdf/Cronus.pdf).

## Printed circuit board

### The factory method

To do.

### The CNC method

The PCB is single-sided, so the manufacturing process is relatively easy. In the [schematic/gcode](schematic/gcode) directory, you can find G-code files suitable for producing homemade PCBs.

The files are generated from the [gerber](schematic/gerber) sources by [genpcb.sh](schematic/genpcb.sh) script, which uses the famous [pcb2gcode](https://github.com/pcb2gcode/pcb2gcode) tool

## Set up

```shell
git clone --recurse-submodules https://github.com/ashep/cronus.git
```

## Update

```shell
git pull
git submodule update --recursive --remote
```

## Changelog

### 0.0.1

First release.

## Authors

- [Oleksandr Shepetko](https://shepetko.com) -- initial work.
