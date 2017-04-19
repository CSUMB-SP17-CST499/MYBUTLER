#! /bin/bash

# GO HOME
USER_HOME=$(getent passwd $SUDO_USER | cut -d: -f6)
cd $USER_HOME


# INSTALLING OpenHAB 2 SERVICE
echo ""
echo ""
echo "  --> *INSTALLING SERVICE FILE*"
echo ""
sudo cp $USER_HOME/MYBUTLER/Software/Configuration/openhab2.service /lib/systemd/system/openhab2.service
echo ""

# CONFIGURING LINUX TO START OpenHAB 2 AT BOOT
echo ""
echo ""
echo "  ---> *ENABLING OpenHAB 2 AS SERVICE*"
echo ""
sudo systemctl daemon-reload
sudo systemctl enable openhab2.service
echo ""

# CHECKING STATUS OF OpenHAB 2 RUNTIME
echo ""
echo ""
echo "  --> *STARTING AND CHECKING SERVICE*"
echo ""
sudo systemctl start openhab2.service
sudo systemctl status openhab2.service
echo ""
