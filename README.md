# url-shortener
URL Shortener in php

Notes
-----
1. These files (except README) are to be placed in Apache's /var/www/ directory.
2. html/htaccess needs to be renamed to html/.htaccess.
3. inc/db.inc needs to be updated to reflect the correct DB_SERVER, DB_USERNAME, and DB_PASSWORD after deployment.
4. In the /etc/httpd/conf/httpd.conf, under <Directory "/var/www/html">, set AllowOverride to All.
