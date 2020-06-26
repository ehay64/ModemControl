# ModemControl
Proof of concept use of the QMI Lite SDK for Sierra Wireless modems.

## Purpose:
The purpose of this repository is for me to play around with the QMI Lite SDK for Sierra Wireless modems. There may or may not be anything useful here.

## Details:
The QMI Lite SDK is a SDK published by Sierra Wireless that can be used to interact with their linup of modems. The SDK can be used in addition to AT commands.

## Issues:
The QMI SDK is published as a statically linkable library. It's been compiled for several popular architectures, but for some reason not ARM hard float. To use this SDK on a Raspberry Pi or similar system that uses the hard float ABI it's possible to cross compile this on a separate system with an ARM soft float cross compiler and statically link all libraries. This may or may not work.
