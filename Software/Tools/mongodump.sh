#! /bin/sh

# GO HOME
USER_HOME=$(getent passwd $SUDO_USER | cut -d: -f6)
cd $USER_HOME

# GO TO OHCloud INSTALLATION
cd $USER_HOME/OHCloud

mongodump

for f in $USER_HOME/OHCloud/dump/openhab/*bson
	do bsondump "$f" > "$f.json"
done

cat $USER_HOME/OHCloud/dump/openhab/emailverifications.bson.json

echo ""
echo "To activate, follow link format below and submit from HTML browser:"
echo "  http<s>://<IP/DNS>/verify?code=<CODE>"
echo ""
echo ""
