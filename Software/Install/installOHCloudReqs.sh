#! /bin/bash

# UPDATE APT WITH NEW REPOSITORY PACKAGES
echo ""
echo ""
echo "  --> *UPDATING APT*"
echo ""
sudo apt-get update
echo ""

# INSTALL REQUIREMENTS
echo ""
echo ""
echo "  --> *INSTALLING REQUIREMENTS*"
echo ""
sudo apt-get install build-essential curl redis-server mongodb python git python-software-properties -y
echo ""

# NODE JS VERSION CONFIGURATION
echo ""
echo ""
echo "  --> *CONFIGURING NODEJS INSTALL VERSION*"
echo ""
sudo curl -sL https://deb.nodesource.com/setup_7.x | sudo -E bash -
echo ""

# NPM INSTALLATION
echo ""
echo ""
echo "  --> *INSTALLING NPM*"
echo ""
sudo apt-get install npm -y
echo ""

# NODE JS INSTALLATION
echo ""
echo ""
echo "  --> *INSTALLING NODE*"
echo ""
sudo apt-get install nodejs -y
echo ""





