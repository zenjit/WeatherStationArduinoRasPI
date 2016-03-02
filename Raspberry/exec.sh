#!/bin/sh
python /home/pi/Documents/arduinoGetter.py
echo "Sending FicheroUrubamba to ftp server.."
sshpass -p '' scp /home/pi/Documents/FicheroUrubamba.txt nesthinglp@ftp.nesthings.com:www/