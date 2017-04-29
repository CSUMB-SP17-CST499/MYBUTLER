#! /bin/bash

# GET OpenHAB Cloud FILES
echo ""
echo ""
echo "   --> *DOWNLOADING OPENHAB CLOUD*"
echo ""
cd $USER_HOME
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

# ENSURE OpenHAB Cloud HAS WORKING app.js (CSRF fix)
echo ""
echo ""
echo "   --> *USING LEGACY app.js TO FIX CSRF*"
echo ""
sudo cp $USER_HOME/MYBUTLER/Software/Configuration/app.js ./
echo ""
echo "   --> *CONFIGURATION FINISHED*"

# ENSURE OpenHAB Cloud HAS WORKING index.ejs (HTML fix)
echo ""
echo ""
echo "   --> *USING MODIFIED index.ejs TO FIX HTML*"
echo ""
sudo cp $USER_HOME/MYBUTLER/Software/Configuration/index.ejs ./views/
echo ""
echo "   --> *CONFIGURATION FINISHED*"


