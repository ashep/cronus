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

## Circuit Diagram

- KiKad sources are in the [schematic](schematic) directory.
- A PDF version is in the [schematic/pdf/Cronus.pdf](schematic/pdf/Cronus.pdf) file.

## Printed Circuit Board

### Manufacturing at the Factory

In the [schematic/gerber](schematic/gerber) directory you can find ready to use Gerber files that you could send to you
favourite PCB manufacturer like JLC PCB or PCB Way.

### Manufacturing at Home

The PCB is simple and single-sided, so the manufacturing process is easy. In the [schematic/gcode](schematic/gcode)
directory, you can find G-code files suitable for producing homemade PCBs using a CNC router..

The files are generated from the [gerber](schematic/gerber) sources by [genpcb.sh](schematic/genpcb.sh) script, which
uses the famous [pcb2gcode](https://github.com/pcb2gcode/pcb2gcode) tool

## Flashing Firmware

To do.

## Development

Clone the repo:

```shell
git clone --recurse-submodules https://github.com/ashep/cronus.git
```

Or update an already cloned one to the latest version:

```shell
git pull
git submodule update --recursive --remote
```

## Changelog

### 0.0.1

First release.

## Authors

- [Oleksandr Shepetko](https://shepetko.com) -- initial work.
