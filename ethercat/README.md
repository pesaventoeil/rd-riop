# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## Remote IO Platform: ECAT Sample Application
This application showcases the analog and digital I/O features supported by the Remote IO Platform in combination with the EtherCAT industrial protocol. The platform communicates over ethernet with CODESYS application running on a PC.<br />

### The features include:<br />
<ul>
    <li>Signal generator</li>
    <li>MCU Digital Input pin control</li>
    <li>MCU Digital Output pin control</li>
    <li>AFE Digital I/Os control</li>
    <li>External Voltage Signal Measurement (HVSIG)</li>
    <li>Current Measurement</li>
    <li>Temperature Measurement</li>
    <li>Internal Voltage References measurement (LVSIG)</li>
    <li>Voltage Calibration</li>
    <li>Resistance Calibration</li>
</ul><br />

This README contains simplified information. Please, refer to the <a href="https://www.nxp.com/design/design-center/development-boards-and-designs/REMOTE-IO-PLATFORM?tid=vanRIOP">User Guide</a> for comprehensive instructions on what tools to download, how to configure the setup and run the application.

#### Boards: Custom Board
#### Categories: Sensor, Industrial, RTOS, Analog Front End
#### Peripherals: CAN, FLASH, PWM, SPI, UART, PINCTRL, TIMER, CLOCKS, DMA, ETHERNET, GPIO, ADC
#### Toolchains: MCUXpresso IDE, VS Code

## Table of Contents
1. [Software](#step1)
2. [Setup](#step2)
3. [Release Notes](#step3)

## 1. Software<a name="step1"></a>
The application supports VS Code with the MCUXpresso for VS Code extension version 25.03 or newer.

### IDE Download:
<ul>
    <li><a href="https://code.visualstudio.com/">VS Code IDE</a></li>
    <li><a href="https://marketplace.visualstudio.com/items?itemName=NXPSemiconductors.mcuxpresso">MCUXpresso for VS Code extension 25.03+</a></li>
</ul>

Additional SW tools are needed to build, flash and use the application. These are the SEC Tool version 25.09 or newer and CODESYS v3.5 SP16 or newer.

### Tools Download:
<ul>
    <li><a href="https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-secure-provisioning-tool:MCUXPRESSO-SECURE-PROVISIONING">Secure Provisioning Tool v25.09+</a></li>
    <li><a href="https://store.codesys.com/en/codesys-installer.html">CODESYS Installer</a></li>
    
</ul>

MCUxpresso SDK MIMXRT1189xxxxx 25.09.00 is required to enable project importing to VS Code.

### SDK Download:
<ul>
    <li>When using VS Code, download the MCUX 25.09.00 repository using Import Repository feature in the MCUXpresso for VS Code extension</li>
    
</ul>

## 2. Setup<a name="step3"></a>
<b>This README contains simplified information. Please, refer to the <a href="https://www.nxp.com/design/design-center/development-boards-and-designs/REMOTE-IO-PLATFORM?tid=vanRIOP">User Guide</a> for comprehensive instructions on what tools to download, how to configure the setup and run the application.</b>


## 3. Release Notes<a name="step5"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0.0   | Initial release on Application Code Hub        | June 15<sup>th</sup> 2025   |
| 1.5.0   | SDK 25.09.00 based release with GOAL           | Dec 18<sup>th</sup> 2025    |
| 1.8.0   | SDK 25.09.00 based release with GOAL           | April 20<sup>th</sup> 2026    |

