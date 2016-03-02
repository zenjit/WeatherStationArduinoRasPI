//
//     URL: http://arduino.cc/playground/Main/DHTLib
//

#include <dht.h>

dht DHT;

#define DHT21_PIN 5

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  // READ DATA
  Serial.print("DHT21, \t");
  int chk = DHT.read21(DHT21_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
		Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.print("Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.print("Time out error,\t"); 
		break;
    default: 
		Serial.print("Unknown error,\t"); 
		break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);

  delay(1000);
}
