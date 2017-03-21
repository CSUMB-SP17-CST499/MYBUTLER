#! /bin/bash

# ADD MQTT REPO
echo ""
echo ""
echo "   --> *ADDING MOSQUITTO REPOSITORY*"
echo ""
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa -y
echo ""

# ADD JAVA WEBUPD8 REPO
echo ""
echo ""
echo "   --> *ADDING JAVA SDK WEBUPD8 REPOSITORY*"
echo ""
sudo add-apt-repository ppa:webupd8team/java -y
echo ""

# UPDATE APT WITH NEW REPOSITORY PACKAGES
echo ""
echo ""
echo "   --> *UPDATING APT*"
echo ""
sudo apt-get update
echo ""

# MQTT BROKER/CLIENT
echo ""
echo ""
echo "   --> *INSTALLING MOSQUITTO MQTT ENVIRONMENT*"
echo ""
sudo apt-get install mosquitto libmosquitto-dev mosquitto-clients -y
echo ""

# JAVA 8 JRE INSTALLATION
echo ""
echo ""
echo "   --> *INSTALLING JAVA 8*"
echo ""
sudo apt-get install oracle-java8-installer -y
echo ""

# JAVA 8 JRE CONFIGURATION
echo ""
echo ""
echo "   --> *CONFIGURING JAVA 8*"
echo ""
sudo apt-get install oracle-java8-set-default -y
echo ""
