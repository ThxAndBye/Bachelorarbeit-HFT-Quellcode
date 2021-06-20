ReadMe.txt for the Nordic Semiconductor nRF52840 start project.

This project was built for SEGGER Embedded Studio V5.20a.

Supported hardware:
===================
The sample project for the nRF52840 is prepared to run on
a Nordic Semiconductor nRF52840 Development Kit. Using
different target hardware may require modifications.

Configurations:
===============
- Debug:
  This configuration is prepared for download into internal
  Flash using J-Link. An embOS debug and profiling library
  is used.
  To use SEGGER SystemView with this configuration, configure
  SystemViewer for nRF52840_xxAA as target device and SWD at
  4000 kHz as target interface.

- Release:
  This configuration is prepared for download into internal
  Flash using J-Link. An embOS release library is used.

Ozone:
======
Additionally to the project, this BSP contains an Ozone *.jdebug
project which can be used to debug and trace the debug configuration
output of the Embedded Studio project.

The Ozone project was built for and tested with Ozone V3.20e.

The trace header needs to be soldered manually to P25.
