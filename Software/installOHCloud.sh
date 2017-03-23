#! /bin/bash

# INSTALL ALL SUPPORTING SOFTWARE
echo ""
echo ""
echo ""
echo " -----###############################-----"
echo " -----### INSTALLING REQUIREMENTS ###-----"
echo " -----###############################-----"
echo ""
sudo bash ~/MYBUTLER/Software/Install/installOHCloudReqs.sh
echo ""

# INSTALL OpenHAB Cloud
echo ""
echo ""
echo ""
echo " -----################################-----"
echo " -----### INSTALLING OPENHAB CLOUD ###-----"
echo " -----################################-----"
echo ""
sudo bash ~/MYBUTLER/Software/Install/installOHCloud.sh
echo ""
