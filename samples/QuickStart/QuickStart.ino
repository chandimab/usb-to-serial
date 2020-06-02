#include <CustomSoftwareSerial.h>

CustomSoftwareSerial* customSerial;               // Declare serial

// Init value
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.write("Hello World");

  customSerial = new CustomSoftwareSerial(9, 10); // rx, tx
  customSerial->begin(9600, CSERIAL_8N1);         // Baud rate: 9600, configuration: CSERIAL_8N1
  
}

void loop() {
  if (customSerial->available())
    Serial.write(customSerial->read());

}
