import serial
from serial import SerialException
import time
import datetime

port='/dev/ttyACM0'

print ("Connecting to Arduino")

try:
	arduino = serial.Serial(port, 9600, timeout=10) #Open Serial Comunication with Arduino
	connected = True;
	exit
except SerialException:
	connected = False;
	print ("Error connecting to Arduino ",i)

if (connected):
	print ("Connected to Arduino, start handshaking")
	time.sleep(2) #wait 2 seconds between Open Serial and Write Serial

	i = 0
	done = False
	while ((i < 5) and done == False):

		arduino.write("S") #Send S to Arduino to start handshaking
		arduino.flush() #Wait until the write command has been sent 
		arduino.flushInput() #Clean the Serial input clipboard
		time.sleep(1.5) #Wait 1.5 seconds
		hsk = arduino.readline() #Read response from Arduino, should be "OK"
		hsk = hsk.rstrip() #Remove unwanted line separator
		arduino.flushOutput() #Clean the Serial output clipboard

		if (hsk == 'OK'):
			#time.sleep(1)
			arduino.write("G") #Send S to Arduino to start handshaking
			arduino.flush() #Wait until the write command has been sent 
			arduino.flushInput() #Clean input Serial clipboard
			print ("Reading Temperature Sensor")
			time.sleep(2)
			arduino.flushOutput()		

			temp = arduino.read(arduino.inWaiting())
			temp = temp.rstrip()
			print ("Temperature: ")
			print (temp)

			arduino.flush()
			arduino.flushInput()
			print ("Reading Humidity Sensor:")
			time.sleep(2)
			hum = arduino.readline(arduino.inWaiting())
			hum = hum.rstrip()
			print ("Humidity: ")
			print (hum)

			arduino.flush()
			arduino.flushInput()
			print ("Reading Rain Sensor:")
			time.sleep(2)
			rain = arduino.read(arduino.inWaiting())
			rain = rain.strip()
			print ("Raining: ")
			print (rain)

			arduino.flush()
			arduino.flushInput()
			print ("Reading Temperature (2) Sensor:")
			time.sleep(2)
			temp2 = arduino.read(arduino.inWaiting())
			temp2 = temp2.rstrip()
			print ("Temperature2: ")
			print (temp2)

			arduino.flush()
			arduino.flushInput()
			print ("reading Pression Sensor:")
			time.sleep(2)
			pres = arduino.read(arduino.inWaiting())
			pres = pres.rstrip()
			print ("pression: ")
			print (pres)

			#first = False
			done = True

		else: 
			i += 1
			print ("Error retrieving information from Arduino ",i)

	print ("Closing Connection")
	arduino.close()

	if done :
		print ("Generating TXT..")

		xmltmp = open("/home/pi/Documents/template.xml", 'r')
		xmlout = open("/home/pi/Documents/FicheroUrubamba.txt", 'w')

		xmlparse = xmltmp.read()

		datanow = str(datetime.datetime.now().strftime("%d-%m-%Y %H:%M:%S"))
		xmlsubst = xmlparse.replace("stime", datanow)
		xmlsubst = xmlsubst.replace("spres", pres)
		xmlsubst = xmlsubst.replace("stemp2", temp2)
		xmlsubst = xmlsubst.replace("shum", hum)
		xmlsubst = xmlsubst.replace("stemp", temp)
		xmlsubst = xmlsubst.replace("srain", rain)

		#print xmlsubst
		xmlout.write(xmlsubst)
		xmltmp.close()
		xmlout.close()
	
		print ("TXT done.")

else: print ("Arduino not reachable!")

print ("END")

 

