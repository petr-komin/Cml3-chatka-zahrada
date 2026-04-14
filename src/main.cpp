#include <Arduino.h>
//#include <Adafruit_I2CDevice.h>

#include <Adafruit_BMP280.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "gfx.h"
#include "NetReader.h"
#include "secrets.h"

Lgfx gfx;
Dparser dp;
NetReader noro;

#include "RTClib.h"

#define BMP280_ADRESA (0x76)


int korekce = 32;

RTC_DS1307 rtc;

const int oneWireBus = 4;

OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// Dallas DS18B20 ROM adresy ze secrets.h
DeviceAddress addrTady  = DALLAS_ADDR_TADY;
DeviceAddress addrVoda  = DALLAS_ADDR_VODA;
DeviceAddress addrVenku = DALLAS_ADDR_VENKU;

// Precte teplotu ze senzoru podle ROM adresy.
// Vraci NAN pokud senzor neni pripojen.
float readDallas(DeviceAddress addr) {
    float t = sensors.getTempC(addr);
    if (t == DEVICE_DISCONNECTED_C) return NAN;
    return t;
}


void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print(' ');
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);

    Serial.print(" = ");
    Serial.print(dt.unixtime());
    Serial.print("s / ");
    Serial.print(dt.unixtime() / 86400L);
    Serial.print("d since 1970");

    Serial.println();
}


#define I2C_Freq 100000



#define I2C_SDA 33
#define I2C_SCL 32

TwoWire I2CBME = TwoWire(0);

Adafruit_BMP280 bmp = Adafruit_BMP280(&I2CBME);

void setup() {
  Serial.begin(57600);
  //Wire.begin(SDA_1, SCL_1); // Wire communication begin

  gfx.init();




  //delay(3000);

    I2CBME.begin(I2C_SDA, I2C_SCL, 100000);

    if (! rtc.begin(&I2CBME)) {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        delay(1000);
    }

    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(2022, 1,1,10,0,0))  ;
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

        String formattedDatetime = noro.ntp(&gfx);


        if (formattedDatetime.length()>5){
            char s[33];
            formattedDatetime.toCharArray(s, 33);
            DateTime dntp = DateTime(s);
            showDate(">>>>> ",dntp);
            rtc.adjust(dntp);
        }
        //delay(5000);


        gfx.connecting("");
        gfx.drawBatteryBox();
        gfx.napeti(&dp);  // vykreslí ?.?? dokud neprijdou realna data

    Serial2.begin(4800);
    Serial2.setTimeout(100);

    if (!bmp.begin(BMP280_ADRESA)) {
        Serial.println("BMP280 senzor nenalezen, zkontrolujte zapojeni!");
        delay(1000);
    }

    sensors.begin();
    int sensorCount = sensors.getDeviceCount();
    Serial.print("Found ");
    Serial.print(sensorCount, DEC);
    Serial.println(" Dallas devices.");

    // Vypis ROM adres vsech nalezenych Dallas senzoru
    // Pouzij tyto adresy pro definici DALLAS_ADDR_* v secrets.h
    DeviceAddress addr;
    for (int i = 0; i < sensorCount; i++) {
        if (sensors.getAddress(addr, i)) {
            Serial.printf("  Dallas[%d] adresa: { ", i);
            for (int b = 0; b < 8; b++) {
                Serial.printf("0x%02X", addr[b]);
                if (b < 7) Serial.print(", ");
            }
            Serial.println(" }");
        } else {
            Serial.printf("  Dallas[%d] adresu nelze precist\n", i);
        }
    }

}

long et;
String batbuf="";
float teplota1;
int kokon = 330;
float tlak = -100;

float teplota2;
float teplota3;
DateTime now;

long last_datadend=0;

void loop(void) {


    while (Serial2.available()) {
        char c = Serial2.read();
        batbuf+=c;
        if (c=='\n'){
            //Serial.println("Bat Line:"+batbuf);
            dp.parseBateryData(batbuf);
            gfx.napeti(&dp);
            batbuf="";

        }
    }


    if (kokon % 10 ==0) {

        now = rtc.now();
        now = now +  TimeSpan(3600+3600);

        gfx.printDateTime(&now);

        Serial.print("`");

        teplota1 = bmp.readTemperature();
        tlak = (bmp.readPressure()/100.00) + korekce;

        sensors.requestTemperatures();
        teplota2 = readDallas(addrTady);
        teplota3 = readDallas(addrVoda);
        float teplota_venku = readDallas(addrVenku);
        gfx.ruzneUdaje(teplota1, teplota2, teplota3, teplota_venku, tlak);
        Serial.print("!");
    }



    if (kokon>=200) {
        kokon=0;

        // TODO: Kadibouda server neni aktivni - odkomentovat az bude server spusteny
        /*
        Serial.println("cau  t=");
        String data = noro.readData("/t");
        Serial.println(data.length());
        Serial.println(data);
        if (data.length()>0){
            dp.parseKadiTime(data);
        }
        */

        now = rtc.now();
        showDate("RTC  datum cas ", now);
        long t = now.unixtime();

        Serial.println("         unix t=" + String(t) + "    dt=" + String(  t - last_datadend ));

        // TODO: Kadibouda server neni aktivni - odkomentovat az bude server spusteny
        /*
        if (dp.datareqPath.length()>4) {
            Serial.println("Kadiba");

            data = noro.readData(dp.datareqPath ); // "/data/2021-12-31/6.txt"
            Serial.println(data.length());
            //Serial.println(data);
            dp.parseKadibouda(data);
            Serial.println("Reprint "+ String(dp.sklenik)+" "+dp.sud);


            if (t - last_datadend  > 60*10  && dp.volty[0]>0 && dp.volty[4]>0) {
                noro.zapis(teplota1, teplota2, teplota3, tlak, &dp);
                last_datadend = t;
            }
            gfx.espUdaje(&dp);
        }
        */
    }

  delay(100 );
  kokon++;

}
