#!/bin/sh

echo "Capturing photo.."
raspistill -t 2000 -o /home/pi/Documents/FotoUrubamba$(date +_\%Y\%m\%d_\%H).jpg -w 640 -h 480

echo "Copying photo to ftp server.."
sshpass -p '' scp /home/pi/Documents/FotoUrubamba$(date +_\%Y\%m\%d_\%H).jpg nesthinglp@ftp.nesthings.com:www/