# Robocup 2026 Onboarding Project

## Description

This is the main repo for the 2026 onboarding season of Robojackets Robocup.

## Setup

Have ONE member of your team do this:

```sh
git clone https://github.com/RoboJackets/robocup-pcb-miniprojects
cd robocup-pcb-miniprojects
git checkout -b <your-team-name>
git push -u origin <your-team-name>
```

Now ALL members can:

```sh
git clone https://github.com/RoboJackets/robocup-pcb-miniprojects
cd robocup-pcb-miniprojects
git checkout <your-team-name>
```

This branch is where all work for your project should be done. Do not attempt to modify main, and especially do not try to modify other teams' branches.

## Overview

Your task is to create a daisy-chained sensor suite. Powered by the RP2040, this should be able to take a data packet from an SPI input, add to this packet, and pass it to the next board via SPI. Information for this packet is to be collected from the six distinct I2C devices on the board.

### PCB
Your KiCAD project and all other relevant files should be stored in [pcb](/pcb/).

See the [README](/pcb/README.md) for specific information.

### Firmware
Your Pico SDK project and all other relevant files should be stored in [firmware](/firmware/).

See the [README](/firmware/README.md) for specific information.

### Documentation

All live documentation (Hardware Reference Manuals, Technical Overviews, etc) should be stored in your team's drive folder which can be found [HERE](https://drive.google.com/drive/folders/1aSuq1g-BM3DlAOSG3E911sSWYeNv8i6k?usp=drive_link).

Start by copying the two template files from the project template folder [HERE](https://drive.google.com/drive/folders/1uaQHIUm1E482NEQLdI7As42oW3_wIS3i?usp=drive_link).

## Additional Notes

Project updates will be added here as needed