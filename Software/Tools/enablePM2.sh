#! /bin/bash

# GO HOME
USER_HOME=$(getent passwd $SUDO_USER | cut -d: -f6)
cd $USER_HOME


# INSTALLING PM2
echo ""
echo ""
echo "  --> *INSTALLING PM2*"
echo ""
sudo npm install pm2 -g
echo ""

# CONFIGURING PM2 TO START OpenHAB
echo ""
echo ""
echo "  ---> *STARTING OpenHAB WITH PM2*"
echo ""
sudo pm2 start "$USER_HOME/OHCloud/app.js"
echo ""

# CONFIGURING PM2 TO START AT RUNTIME
echo ""
echo ""
echo "  --> *ENABLING PM2 AS SERVICE*"
echo ""
sudo pm2 startup
echo ""

# VERIFYING PM2 STATUS
echo ""
echo ""
echo " --> *VERIFYING PM2 IS RUNNING AND ENABLED*"
echo ""
sudo systemctl status pm2-root