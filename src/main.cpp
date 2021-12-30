#include <Arduino.h>
//#include <Adafruit_I2CDevice.h>




#include "gfx.h"
#include "NetReader.h"

Lgfx gfx;
Dparser dp;
NetReader noro;


void setup() {
  Serial.begin(57600);

  noro.ntp();

  noro.ConnectToWiFi();

  gfx.init();

gfx.espUdaje(&dp);
 gfx.napeti(&dp);

 gfx.ruzneUdaje(100, 2323, 222, 1200);
}


void loop(void) {
  Serial.println("cau");
  noro.readData();

  delay(1000 * 20);

}
