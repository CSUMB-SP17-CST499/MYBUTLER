#! /bin/bash

# ADD JAVA WEBUPD8 REPO
echo ""
echo ""
echo "   --> *ADDING JAVA SDK WEBUPD8 REPOSITORY*"
echo ""
echo "deb http://ppa.launchpad.net/webupd8team/java/ubuntu xenial main" | tee /etc/apt/sources.list.d/webupd8team-java.list
echo "deb-src http://ppa.launchpad.net/webupd8team/java/ubuntu xenial main" | tee -a /etc/apt/sources.list.d/webupd8team-java.list
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys EEA14886
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

# JAVA 8 WEBUPD8
echo ""
echo ""
echo "   --> *INSTALLING JAVA 8 ENVIRONMENT*"
echo ""
sudo apt-get install oracle-java8-installer -y
echo ""

