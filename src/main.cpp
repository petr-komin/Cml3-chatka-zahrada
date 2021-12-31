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




 //gfx.napeti(&dp);

 //gfx.ruzneUdaje(100, 2323, 222, 1200);

    Serial2.begin(4800);
    Serial2.setTimeout(100);

}

long et;
String batbuf="";

int kokon = 330;

void loop(void) {


    while (Serial2.available()) {
        char c = Serial2.read();
        batbuf+=c;
        if (c=='\n'){
            Serial.println("Bat Line:"+batbuf);
            dp.parseBateryData(batbuf);
            gfx.napeti(&dp);
            batbuf="";

        }
    }





    if (kokon>=200) {
        kokon=0;
        Serial.println("cau  t=");
        String data = noro.readData("/t");
        Serial.println(data.length());
        Serial.println(data);
        if (data.length()>0){
            dp.parseKadiTime(data)
        }

        Serial.println("Kadiba");

        data = noro.readData("/data/2021-12-31/6.txt");

        Serial.println(data.length());
        Serial.println(data);
        dp.parseKadibouda(data);
        gfx.espUdaje(&dp);
    }

  delay(100 );
  kokon++;

}
