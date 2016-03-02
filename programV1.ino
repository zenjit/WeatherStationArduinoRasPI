//
//  WeatherStationArduinoRaspberri
//
//  Created by Corrado Scanavino on 10/7/15
//
//

#include <dht.h> // Temp and Humidity Sensor http://playground.arduino.cc/Main/DHTLib
#include <SFE_BMP180.h> // Pressure Sensor https://github.com/sparkfun/BMP180_Breakout/blob/master/software/Arduino/libraries/SFE_BMP180/examples/SFE_BMP180_example/SFE_BMP180_example.ino
#include <Wire.h> // NO SE http://arduino.cc/en/reference/wire

// Define Pins and Constants
#define DHT21_PIN 5
#define DHT21_PIN2 7
#define RAIN_PIN 6
#define FAN_PWM 9
#define LED_PIN 13
#define ALTITUDE 2863.0

// Classes
SFE_BMP180 pressure;
dht DHT; // Temp and Humidity

// Declarations
double hum = 0.0; // Humidity
double t1 = 0.0; // Temperature form DHT
double t2 = 0.0; // Temperature from BMP180
double t3 = 0.0; // Temperature from BMP180 inside
boolean rain = false;
int raining = 0;
double pres = 0.0;

double tmin = 20.0; // Min temp value to start fan control
double tmax = 45.0; // Max temp value to start fan control, if more than max fan continue at the same speed
unsigned long timeFanOn = 0; // Store the time in milliseconds since fan on
unsigned long timeFanOff = 0; // Store the time in milliseconds since fan on
unsigned long isFanOnSince = 0; // Variable used to know how much time does the fan is on
unsigned long isFanOffSince = 0; // Variable used to know how much time does the fan is off
unsigned long maxTimeFanOn = 3000000; // Max time the fan could be on until rest for a while, in milliseconds (300000=5min)
unsigned long maxTimeFanOff = 600000; // Max time the fan should stay off altought the temp is higher than min (60000=1min)
boolean fanOn = false; // fan on is true, fan off is false
boolean fanQuiet = false; // fan in quiet mode is true, fan not in quiet mode is false
int fanPwm = 0;

void setup() {
  Serial.begin(9600);
  pressure.begin();
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  
  // Main program
  digitalWrite(LED_PIN, HIGH);
  raining = (int) readRainSensor();
  delay(100);
  
  readTempHumSensor();
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  
  readPressTempSensor();
  delay(1000);
  
  raspberryHandshake();
  delay(1000);
  
  readTempHumSensor2();
  fanControl(t3);
}

boolean readRainSensor () {
  if (digitalRead(RAIN_PIN)==HIGH){
    return false;
  } 
  else return true;
}

void fanSpeed(int pwm) {
  if (pwm <200) {
    analogWrite(FAN_PWM, 0);
  }
  else {
    analogWrite(FAN_PWM, pwm);
  }
}

void readTempHumSensor () {

  // READ DATA
  int chk = DHT.read21(DHT21_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
		//Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.print("DHTLib Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.print("DHTLib Time out error,\t"); 
		break;
    default: 
		Serial.print("DHTLib Unknown error,\t"); 
		break;
  }
  hum = DHT.humidity;
  t1 = DHT.temperature;
}

void readTempHumSensor2 () {

  // READ DATA
  int chk = DHT.read21(DHT21_PIN2);
  switch (chk)
  {
    case DHTLIB_OK:  
		//Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.print("DHTLib Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.print("DHTLib Time out error,\t"); 
		break;
    default: 
		Serial.print("DHTLib Unknown error,\t"); 
		break;
  }
  t3 = DHT.temperature;
}

void readPressTempSensor() {
  
  char status;
  
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Note that the measurement is stored in the variable t2.
    // Function returns 1 if successful, 0 if failure.
    status = pressure.getTemperature(t2);
    if (status != 0)
    {    
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      status = pressure.startPressure(2);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
        
        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.
        status = pressure.getPressure(pres,t2);
        if (status == 0) {
          Serial.println("error retrieving pressure\n");
        }
      }
      else Serial.println("error starting pressure\n");
    }
    else Serial.println("error retrieving temperature\n");
  }
  else Serial.println("error starting temperature\n");
}

void raspberryHandshake() {
  
  if (Serial.available()) {
    Serial.flush();
    char c = Serial.read();
    if (c == 'S') {
       delay(1000);
       Serial.println("OK");
       delay(1000);
       if (Serial.read() == 'G'){
         Serial.flush();
         Serial.println(t1, 2);
         Serial.flush();
         delay(2000);
         Serial.println(hum, 2);
         Serial.flush();
         delay(2000);
         Serial.println(raining, DEC);
         Serial.flush();
         delay(2000);
         Serial.println(t2, 2);
         Serial.flush();
         delay(2000);
         Serial.println(pres, 2);
         Serial.flush();
       }
       else exit;
    }
    else exit;
  }
  else Serial.println("Waiting for Serial connection with Raspberry.");
}

void fanControl(double temperature) {
  // If the temperature is higher than tmin
  if (temperature > tmin) {
    // If the fan wasn off and not in quiet mode
    if ((fanOn == false) && (fanQuiet == false)) {
      // Compute time the fan turn on
      timeFanOn = millis();
      // Reset timers
      isFanOnSince, isFanOffSince, timeFanOff = 0;
      // Set fan to witch ON
      fanOn = true;
      fanPwm = 200;
    }
    else if (fanQuiet == true) {
        // Compute the time since the fan is in quiet mode
        isFanOffSince += (millis()-timeFanOff);
    }
    else {
      // Compute the time since the fan in on
      isFanOnSince += (millis()-timeFanOn);
    }
    
    // If fan is on since more than max time then put it into quiet mode
    if (isFanOnSince >= maxTimeFanOn) {
      // Stop fan and start quiet mode
      fanPwm = 0;
      fanOn = false;
      fanQuiet = true;
      // Reset timers
      timeFanOn, isFanOnSince = 0;
      // Compute time the fan turn off
      timeFanOff = millis();
    }

    // If fan is off since more than max time off
    if (isFanOffSince >= maxTimeFanOff) {
      fanQuiet = false;
      fanOn = false;
      isFanOffSince = 0;
    }
    // Set correct speed to the fan, PWM=255 > 5V, PWM=0 > 0V
    fanSpeed(fanPwm);
  }
  else {
    fanSpeed(0);
  }
}

