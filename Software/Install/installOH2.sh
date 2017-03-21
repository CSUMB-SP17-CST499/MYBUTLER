#! /bin/bash

# ADD NEW USER FOR OpenHAB SYSTEM
echo ""
echo ""
echo "  --> *ADDING OPENHAB USER TO SYSTEM*"
echo ""
sudo adduser --system --no-create-home --group --disabled-login openhab
echo ""

# GET OpenHAB FILES
echo ""
echo ""
echo "  --> *GETTING OPENHAB RUNTIME*"
echo ""
cd /tmp
wget -O openhab-download.zip https://bintray.com/openhab/mvn/download_file?file_path=org%2Fopenhab%2Fdistro%2Fopenhab%2F2.0.0%2Fopenhab-2.0.0.zip
echo ""

# INSTALL OpenHAB RUNTIME
echo ""
echo ""
echo "  --> *UNPACKING OPENHAB RUNTIME*"
echo ""
sudo unzip openhab-download.zip -d /opt/openhab2
rm openhab-download.zip
echo ""

# CONFIGURE USER PERMISSIONS FOR OpenHAB
echo ""
echo ""
echo "  --> *SETTING USER PERMISSIONS*"
echo ""
sudo chown -hR openhab:openhab /opt/openhab2
echo ""
