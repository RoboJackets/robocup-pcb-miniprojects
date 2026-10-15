# Daisy-Chained Sensor Suite PCB

## Overview

You will be planning, designing, and laying out the PCB for a daisy-chainable sensor suite. The core features of this board will be a RP2040 MCU, Micro USB programming/power input, SPI I/O, and six I2C peripherals.

## Requirements

Additional design specifications can be found in the Hardware Reference Manual (HRM).

1. Documentation
    - While most major parts are pre-selected for you, the documentation for these has not been. You will need to finish filling out the missing sections in the HRM for each component.
    - If you add any new components with a major impact on the final product, make sure to include them as well
    - Overall, keep your HRM as up to date as possible with as much important information as possible so your firmware team can progress along side you
2. PCB
    - Must be manufacturable
        - See JLCPCB capabilities [HERE](https://jlcpcb.com/capabilities/pcb-capabilities)
        - *Hint hint nudge nudge an option for board settings is to import from another board, and Robocup might have boards with similar design requirements [HERE](https://github.com/RoboJackets/robocup-pcb)*
    - Four layers max
        - Going below four is not recommended
    - Must pass DRC
        - Will be checked with the Robocup standard DRC suite (if you followed the hint this likely will not be an issue)
    - Must consider signal integrity
    - Must have a legible and correct silkscreen
        - Labels by components or otherwise noted
        - Location for serial number
        - Identifier for all those who worked on it
        - Bonus points for fitting the Robojackets logo
    - Must meet overall required specifications
    - Must have 4 mounting holes (M2)
3. Schematic
    - Must utilize a hierarchy of sheets
        - Do not even ask for a design review of a schematic all crammed on one sheet it is not feasible for the expected complexity
        - Also do not excessively break into sub-sheets, consider what is seperate/repeatable enough to place in a sub-sheet.
    - Notate important characteristics in the schematic
        - General comments
        - Section functionality
        - Placement location requirements
        - Component relevant information
        - The less you have to check the HRM the better
    - Must fill out all sheet information in the right corner
        - Include an additional note for designer name
    - Symbol fields
        - You must fill out all default fields of major components. Non-specialized passives just need footprint and value.
        - You must add the field "Vender P/N" to every specific component with the value set to the Digikey part number
    - When you need to make/import new symbols/footprints/3D models, add them to a new custom library called "local" and store their files if needed in [lib](./lib/)
4. Power
    - Power must be able to come from both the USB port and the previous board's SPI line and they must never touch
        - See power MUXing
    - Your power must have appropriate protection for issues such as reverse voltage, ESD, shorts, etc.
    - Your total power draw, and expected power draw for six connected boards must be noted in your schematic and reflected in your protection circuits 
        - Note only one board with an LED array will be active at once
    - There must be some indication that power is connected to the board
5. I2C Devices
    - Must have all six I2C devices and their complementary circuits
        1. IMU
        2. Temperature Sensor
        3. Hall Effect Sensor
        4. GPIO expander
            - Connected to 8 switch DIP switch
            - Resistor network pullup
        5. Color Sensor
        6. LED PWM Controller
            - 8 RGB LEDs
    - Each device must have have different I2C lines
        - However they can share I2C buses
6. Interface
    - Must have at least two general purpose buttons
    - Must have at least three general purpose LEDs
    - Must have test points for all important voltage/signal sources
7. Overall
    - All parts must be in-stock at Digikey at time of completion
        - Yes, if stock runs out during the duration of the project this is still on you to catch
        - You can disregard this rule for parts from the Part Selection specification
    


## Specifications

### Part Selection

These parts have already been selected for you

- MCU: RP2040
    - 2648-SC0914(13)TR-ND
- 2MB Flash: W25Q16JVUXIQ TR
    - 256-W25Q16JVUXIQTR-ND
- 12MHz Crystal: ABM8-272-T3
    - 535-ABM8-272-T3TR-ND
- IMU: ICM-42605
    - 1428-1152-2-ND
- Color Sensor: VEML3328
    - 751-VEML3328TR-ND
- Hall Effect Sensor: TMAG5273C1QDBVR
    - 296-TMAG5273C1QDBVRTR-ND
- Temperature Sensor: LM75ADP
    - 1428-1152-2-ND
- GPIO Expander: TCA9534PWR
    - 296-40574-2-ND
- DIP Switch: DS01C-254-L-08BE
    - 2223-DS01C-254-L-08BE-ND
- 10kOhm Resistor Network: YC158TJR-0710KL
    - 13-YC158TJR-0710KLTR-ND
- LED Controller: IS31FL3254-QFLS4-TR
    - 2521-IS31FL3254-QFLS4-TR-ND
- RGB LEDs: CLMVC-FKA-CL1D1L71BB7C3C3
    - CLMVC-FKA-CL1D1L71BB7C3C3TR-ND
- Micro USB Connector: 10118194-0001LF
    - 609-4618-2-ND
- SPI Connector: S6B-PH-K-S
    - 455-1723-ND
- 3.3V Linear Regulator: AP2112K-3.3
    - AP2112K-3.3TRG1DITR-ND
- OR Controller: LM66100DCKR
    - 296-53541-2-ND
- General Purpose LEDs (3)
    - 0603 LEDs
- General Purpose Buttons (2): SKRPABE010
    - 4809-SKRPABE010TR-ND
    