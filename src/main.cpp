#include <Arduino.h>
//#include <Adafruit_I2CDevice.h>




#include "gfx.h"
#include "NetReader.h"

Lgfx gfx;
Dparser dp;
NetReader noro;


void setup() {
  Serial.begin(57600);
  gfx.init();

  noro.ntp();

  //noro.ConnectToWiFi();



 gfx.espUdaje(&dp);
 //gfx.napeti(&dp);

 //gfx.ruzneUdaje(100, 2323, 222, 1200);

}


void loop(void) {
  Serial.println("cau  t=");


    String data = noro.readData("/t");
    Serial.println( data.length() );
    Serial.println(data);

    Serial.println("Kadiba");

   data = noro.readData("/data/2021-12-30/20.txt");

  Serial.println( data.length() );
  Serial.println(data);
  dp.parseKadibouda(data);

  delay(1000 * 20);

}
