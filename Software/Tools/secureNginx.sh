#!/bin/bash

# GO HOME
USER_HOME=$(getent passwd $SUDO_USER | cut -d: -f6)
cd $USER_HOME

# GET DNS NAME FROM USER
echo "OK, enter your dns name"
read DNS_NAME
echo "OK, installing nginx and letsencrypt"

# INSTALL OpenSSL, Nginx, AND LetsEncrypt
sudo apt-get update
sudo apt-get install openssl nginx letsencrypt

# REMOVE AND RE-CREATE DEFAULT SITES FOR LetsEncrypt
sudo rm -f /etc/nginx/sites-available/default
sudo touch /etc/nginx/sites-available/default
echo "
server {
	
	# Pass all web requests in, straight through the front door
    listen 80 default_server;
    listen [::]:80 default_server;
    server_name $DNS_NAME www.$DNS_NAME;

	# Logging activity from LetsEncrypt sessions
	charset utf-8;
	access_log /var/log/nginx/letsencrypt-access.log;
	error_log /var/log/nginx/letsencrypt-error.log;
	client_max_body_size 300m;

	# Redirect folder for holding page
    root /var/www/html;
	# Redirect folder for DNS/Domain Authorizaion
    location ~ /.well-known {
		allow all;
    }



}" > /etc/nginx/sites-available/default
sudo systemctl restart nginx

# START LetsEncrypt PROCESS AND LIST RESULTING KEYS FOR CURRENT DOMAIN
sudo letsencrypt certonly -a webroot --webroot-path=/var/www/html -d "$DNS_NAME" -d www."$DNS_NAME"
sudo ls -l /etc/letsencrypt/live/"$DNS_NAME"

# CREATE DIFFIE-HELMAN PARAMETER SET FOR SSL CRYPTO
sudo openssl dhparam -out /etc/ssl/certs/dhparam.pem 2048
sudo touch /etc/nginx/snippets/ssl-"$DNS_NAME".conf

# CREATE SSL CONFIGURATION FOR CURRENT DOMAIN
sudo echo "ssl_certificate /etc/letsencrypt/live/$DNS_NAME/fullchain.pem;
ssl_certificate_key /etc/letsencrypt/live/$DNS_NAME/privkey.pem;" > /etc/nginx/snippets/ssl-"$DNS_NAME".conf

# COPY OVER GENERAL SSL PARAMETERS FOR Nginx
sudo cp ./MYBUTLER/Software/Configuration/ssl-params.conf /etc/nginx/snippets/ssl-params.conf

# REMOVE AND RE-CREATE DEFAULT SITES FOR OpenHAB-Cloud
sudo rm -f /etc/nginx/sites-available/default
sudo touch /etc/nginx/sites-available/default
echo "
server {
	
	# Redirect all requests to secure versions
    listen 80 default_server;
    listen [::]:80 default_server;
    server_name $DNS_NAME www.$DNS_NAME;
    return 301 https://\$server_name\$request_uri;
}

server {

    # SSL configuration
    listen 443 ssl http2 default_server;
    listen [::]:443 ssl http2 default_server;
    include snippets/ssl-$DNS_NAME.conf;
    include snippets/ssl-params.conf;

	# Logging
	charset utf-8;
	access_log /var/log/nginx/openhabcloud.org-access.log;
	error_log /var/log/nginx/openhabcloud-error.log;
	client_max_body_size 300m;

	# Redirect Folders
	location /css {
		alias  /home/ubuntu/OHCloud/public/css;
		}
	location /js {
		alias /home/ubuntu/OHCloud/public/js;
		}
	location /img {
		alias /home/ubuntu/OHCloud/public/img;
		}
	location /bootstrap {
		alias /home/ubuntu/OHCloud/public/bootstrap;
		}
	location /font-icons {
		alias /home/ubuntu/OHCloud/public/font-icons;
		}
	location /fonts {
		alias /home/ubuntu/OHCloud/public/fonts;
		}
	location /js-plugin {
		alias /home/ubuntu/OHCloud/public/js-plugin;
		}
	location /downloads {
		alias /home/ubuntu/OHCloud/public/downloads;
		}
	# Proxy Redirect for Core Route
	location / {
		proxy_pass http://localhost:3000;
		proxy_redirect off;
		proxy_http_version 1.1;
		proxy_set_header Host \$host;
		proxy_set_header Upgrade \$http_upgrade;
		proxy_set_header Connection \"upgrade\";
		proxy_set_header X-Real-IP \$remote_addr ;
		proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for ;
		proxy_set_header X-Forwarded-Proto https;
	}

	# Error Pages
	error_page 404 /404.html;
	error_page 500 502 503 504 /50x.html;
	location = /50x.html {
		root html;
	}
}" > /etc/nginx/sites-available/default
sudo systemctl restart nginx

# PRINT OUT SUCCESS VERIFICATION METHODS
echo "Please visit: https://www.ssllabs.com/ssltest/analyze.html?d=$DNS_NAME to verify installation."