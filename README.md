# Cronus

**Cronus** is a minimalistic digital clock built on ESP32 MCU.

![showcase-01](img/showcase/01.jpg)

<details>
<summary>See more samples</summary>

![showcase-02](img/showcase/02.jpg)
![showcase-03](img/showcase/03.jpg)
![showcase-04](img/showcase/04.jpg)
![showcase-05](img/showcase/05.jpg)
</details>

## Schematic diagram

Look at the [schematic/pdf/Cronus.pdf](schematic/pdf/Cronus.pdf).

## Printed circuit board

### The factory method

To do.

### The CNC method

The PCB is single-sided, so the manufacturing process is relatively easy. In the [schematic/gcode](schematic/gcode)
directory, you can find G-code files suitable for producing homemade PCBs.

The files are generated from the [gerber](schematic/gerber) sources by [genpcb.sh](schematic/genpcb.sh) script, which
uses the famous [pcb2gcode](https://github.com/pcb2gcode/pcb2gcode) tool

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
