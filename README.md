# EVSE Device Chargemaster Demo

Electric vehicle supply equipment device controller.

Currently uses mbed OS and started on a Nucleo STM32F767ZI

This is a fork of the MQTT enabled code that has been cut down for the chargemaster Smart Charger demo.

# Setup

So far this has been developed using the mbed CLI tools and the GNU GCC ARM cross-compiler, both of which you will need to install;

* [mbed CLI](https://docs.mbed.com/docs/mbed-os-handbook/en/latest/dev_tools/cli/)
* [GCC ARM](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
* `mbed deploy` in this repo's checkout directory to download extra libraries
* optionally set default toolchain
* optionally set mbed default target

# Building

If you haven't set the particular machine and toolchain you will need to specify them;

`bash$ mbed compile -m <machine> -t <toolchain> --profile ./profiles/custom-develop.json`

Otherwise it just needs the correct profile to set up C++ standards.

`bash$ mbed compile --profile ./profiles/custom-develop.json`

# Installing

Copy bin file onto device, example;

`bash$ cp ./BUILD/NUCLEO_F767ZI/GCC_ARM/evse-device.bin /media/wiggly/NODE_F767ZI/main.bin`

# Testing

TBD
"# Chargemaster" 
