#include <Arduino.h>
//#include <Adafruit_I2CDevice.h>


#include <DallasTemperature.h>
#include <OneWire.h>

#include "gfx.h"
#include "NetReader.h"

Lgfx gfx;
Dparser dp;
NetReader noro;

const int oneWireBus = 4;

OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);


void setup() {
  Serial.begin(57600);
//    Wire.begin(); // Wire communication begin
  gfx.init();

  noro.ntp();
  //noro.ConnectToWiFi();
 //gfx.napeti(&dp);
 //gfx.ruzneUdaje(100, 2323, 222, 1200);

    Serial2.begin(4800);
    Serial2.setTimeout(100);

    sensors.begin();
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");
    delay(1000);
}

long et;
String batbuf="";
float teplota1;
int kokon = 330;
float tlak = -100;

float teplota2;
float teplota3;

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


    if (kokon % 10 ==0) {
        Serial.print("`");

        teplota1 = -100;  //bmp.readTemperature();
        tlak = -100; //(bmp.readPressure()/100.00) + korekce;
        uint8_t devcount = sensors.getDeviceCount();
        sensors.requestTemperatures(); // Send the command to get temperature
        teplota2 = sensors.getTempCByIndex(0);
        teplota3 = sensors.getTempCByIndex(1);
        gfx.ruzneUdaje(teplota1, teplota2, teplota3, tlak);
        Serial.print("!");
    }



    if (kokon>=200) {
        kokon=0;
        Serial.println("cau  t=");
        String data = noro.readData("/t");
        Serial.println(data.length());
        Serial.println(data);
        if (data.length()>0){
            dp.parseKadiTime(data);
        }

        if (dp.datareqPath.length()>4) {
            Serial.println("Kadiba");

            data = noro.readData(dp.datareqPath ); // "/data/2021-12-31/6.txt"

            Serial.println(data.length());
            Serial.println(data);
            dp.parseKadibouda(data);
            gfx.espUdaje(&dp);
        }
    }

  delay(100 );
  kokon++;

}
