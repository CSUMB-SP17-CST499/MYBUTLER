#! /bin/bash

# GET OpenHAB Cloud FILES
echo ""
echo ""
echo "   --> *DOWNLOADING OPENHAB CLOUD*"
echo ""
cd ~
sudo git clone https://github.com/openhab/openhab-cloud.git OHCloud
echo ""

# INSTALL OpenHAB Cloud
echo ""
echo ""
echo "   --> *INSTALLING OPENHAB CLOUD*"
echo ""
cd ./OHCloud
sudo npm install
echo ""

# ENSURE OpenHAB Cloud HAS CONFIG FILE
echo ""
echo ""
echo "   --> *CREATING INITIAL CONFIGURATION*"
echo ""
sudo cp config-production.json config.json
echo ""
echo "   --> *CONFIGURATION FINISHED*"