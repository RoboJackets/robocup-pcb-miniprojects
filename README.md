# Robocup 2026 Onboarding Project

## Description

This is the main repo for the 2026 onboarding season of Robojackets Robocup.

## Setup

Have ONE member of your team do this:

```sh
git clone https://github.com/RoboJackets/robocup-pcb-miniproject
git checkout -b <your-team-name>
git push -u origin <your-team-name>
```

Now ALL members can:

```sh
git clone https://github.com/RoboJackets/robocup-pcb-miniproject
git checkout <your-team-name>
```

This branch is where all work for your project should be done. Do not attempt to modify main, and especially do not try to modify other teams' branches.

## Overview

Your task is to create a daisy-chained sensor suite. Powered by the RP2040, this should be able to take a data packet from an SPI input, add to this packet, and pass it to the next board via SPI. Information for this packet is to be collected from the 6 distinct I2C devices on the board.

### PCB
Your KiCAD project and all other relevant files should be stored in [pcb](/pcb/).

See the [README](/pcb/README.md) for additional information.

### Firmware
Your Platformio project and all other relevant files should be stored in [firmware](/firmware/).

See the [README](/firmware/README.md) for additional information.


## Additional Notes

Project updates will be added here as needed