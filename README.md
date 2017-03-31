![Image][logo]

# Welcome To The MY-BUTLER Project


## Concept

This project is an attempt to create a modern and complete home-automation solution in the Open Source realm by combining several other OSS projects and forming a set of hardware components to match. The following is a general overview of all the separate pieces that form the MYBUTLER system.


## Components

### Software

#### Overview
The software environment for the MYBUTLER project spans both the local networks, as well as the cloud. The local network (OpenHAB 2) software must have an always-on server running inside the house, preferably a low-energy unit like a Raspberry Pi, etc. This server is responsible for the command and control structure, including the item/map configurations for each house and the web GUI for accessing the system. The MQTT broker and ESP8266 IDE are also both mandatory for completing the MYBUTLER system, but the cloud service can be omitted if the user wishes. Forgoing OpenHAB Cloud will remove a large amount of functionality when the user is not connected to their home network, however, and installation/configuration of this service is highly encouraged.

#### *OpenHAB 2*
The OpenHAB 2 software (Open Home Automation Bus) is Open Source Software designed to serve as a command & control application for a wide variety of home-automation products. The OpenHAB 2 software must run from the local network on a relatively dedicated server (OH2 has very low resouce requirements once started and running) and be left running in order to provide service. The system is capable of integrating with a large number of existing home-automation products and services, including Z-Wave, Phillips HUE, Insteon, and of course - the MYBUTLER hardware.

#### *OpenHAB Cloud*
The OpenHAB Cloud is a cloud based service that provides a user-authentication and reverse-proxy module for the OpenHAB 2 client, while also allowing integration with other cloud services. This is all done without needing any firewall or security configuration on the local-network, allowing non-technical users to still take advantage of the benefits of cloud integration. OpenHAB Cloud is used to integrate the MYBUTLER system with the following services:
 * Amazon Voice Service 
   * (Alexa Voice Companion)
 * If This Then That 
   * (Rules Based Actions)
 * Google Firebase Messaging 
   * (Real-Time Carrier-Agnostic SMS)
 * Apple Push Notifications 
   * (Real-Time iPhone/iPad Notifications)
 * XMPP 
   * (Messaging Platform for User Applications)
 * OAuth2
   * (Highly Secure User Authentication)

#### *MQTT Broker/Client*
MYBUTLER uses an inter-process and inter-device messaging protocol to enable communication between the OpenHAB 2 installation and the hardware deployed as home-automation units. Each unit deployed in the system has a unique set of "channels" (aka "topics") that can both broadcast status updates to, and receive commands from the OpenHAB 2 runtime. The chosen package for this project is Mosquitto, as it is both small and fast, yet has enough utilities to quickly debug the MQTT toolchain if necessary. This can be replaced with any software implementing a standard MQTT broker service.

#### *ESP8266 IDE*
The MYBUTLER hardware runs on ESP8266 micro-controllers and several modern IDE options exist that allow programming these chips. For the MYBUTLER project it was decided to employ an enhanced-version of the Arduino IDE, allowing users familiar with micro-processor development to quickly get up to speed when developing scripts for the MYBUTLER hardware units. The [Arduino IDE files necessary](https://github.com/esp8266/Arduino) to allow ESP8266 module development are all that is needed to enable the official Arduino IDE to provide ESP8266 programming capability. The Arduino IDE can be replaced with any other IDE capable of development on the ESP8266 platform, other options can be found easily on the web.

### Hardware

#### Overview
The hardware created for the MYBUTLER project is designed to leverage low-cost ESP8266 microprocessor units and integrate them into an ecosystem which is adaptable to a large variety of end-use cases. The two main units used in the project utilize the same micro-processor and instruction-set, but come in two different formats with two very different use-cases. The ESP01 module is designed to be as small as possible and although it is "header-ready", it only has two non-general I/O ports available.

##### *ESP01 Breakout*
The ESP01 breakout is designed to provide a full (manual) reset and programing protocol while also allowing a single IN and single OUT port. The two ports are built with floating pins allowing module boot, with float resistances large enough to allow full digital control once the module has finished booting. This means that while the module is powered off, the I/O lines do not provide a stable environment for peripheral devices, and measures such as pullup/pulldown resistors may be required to use those devices in a coherent fashion while also allowing the ESP8266 to operate as normal.

##### *ESP12 Breakout*
When a single port each for I/O is not enough, or the requirements of the attached device dictate that no floating is allowed on control pins, the ESP12(F) breakout provides a much larger set of functions. The breakout can be used to interface with a large number of electronic sensors, and several pins (BUT NOT THE ADC) can be reconfigured in software to provide even more flexibility for demanding applications.
 The ESP12(F) breakout is designed to provide the following features:
 * Power management system - converts from system-wide 5v to ESP8266 required 3.3v
 * Fully [datasheet compliant interfaces,](http://espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf#page=17) including:
   * GPIO x2
     * No pins float - all control I/O pinned to line voltage
   * ADC x1
     * Support for 1024 levels, max @ 1.0 volts
   * Serial (TX, RX)
   * SPI (MOSI, MISO, CLK, CS)
   * I2C (SDA, SCL, VCC, GND)
   * IR (TX, RX)
     * Learning remote capable
 * Power and Activity/Heartbeat LEDs
 * Full (manual) reset and programming methods
 * Voltage splitting circuit for wide-voltage ADC handling

##### *SDR Breakout*
The SDR relay breakout integrates a high-ampacity mechanical relay with a terminal block, opto-coupler, and triggering transistor to provide a high-power switching solution with common-rail status reporting. This allows the AC common-rail to also function as a sort of DC "output", to integrate the ESP8266 logically with a chain of other controllers, when necessary.  As with 3-way switching, tracking the physical state of the attached device is important to avoid switching the relay to an undesired state when a command is received.

##### *HLK-PM01 Breakout*
The HLK-PM01 forms the necessary AC/DC rectifying and buffering to provide 5v power to the MYBUTLER system. Each unit can provide up to 600ma of continuous current, and the system is self-contained and UL certified. The breakout for this board only provides a buffering capacitor and trace-routing to allow for a small fanout from the unit's pins to the MYBUTLER system's power wiring.

[logo]:/Documentation/GHPages/MYBUTLER.png?raw=true "MYBUTLER"
