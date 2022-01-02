//
// Created by manx on 30.12.21.
//

#include "NetReader.h"


#include "WiFi.h"
#include <HTTPClient.h>


#include <WiFiUdp.h>


String serverName = "http://192.168.8.1";
const char cml_ssid[] = "Kadibouda";
WiFiUDP ntpUDP;


void ConnectInternet(Lgfx * gfx) {
    const char ssid[] = "OK1BE";
    const char pass[] = "WIFI_PASSWORD_REMOVED";
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    gfx->connecting("Connecting OK1BE");

    Serial.println("Connecting  "+String(ssid) );

    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i<15) {
        Serial.print('.');
        delay(500);

        if ((++i % 16) == 0) {
            Serial.print(F(" still trying to connect "));
            Serial.println (cml_ssid);
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
        gfx->connecting("Připojeno");
        Serial.print(F("Connected. My IP address is: "));
        Serial.println(WiFi.localIP());
    }else{
        gfx->connecting("Bez připojení");
    }
}



String NetReader::ntp(Lgfx * gfx){

    ConnectInternet(gfx);
    NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0, 60000);
    if(WiFi.status()== WL_CONNECTED) {
        Serial.println("timeclient ...");
        gfx->connecting("Načítám přesný čas");

        timeClient.begin();
        timeClient.update();
        Serial.println( timeClient.getFormattedTime());
        Serial.println( timeClient.getFormattedDate());
        timeClient.end();
    }else{
        Serial.println("WL not connected for NTP");
        return "";
    }
    WiFi.disconnect();
    return timeClient.getFormattedDate();
}

void NetReader::ConnectToWiFi() {

    WiFi.mode(WIFI_STA);
    WiFi.begin("Kadibouda");
    Serial.println("Connecting  Kadibouda");

    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 31) {
        Serial.print('.');
        delay(500);

        if ((++i % 16) == 0) {
            Serial.println(F(" still trying to connect Kadibouda"));
        }
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.print(F("Connected. My IP address is: "));
        Serial.println(WiFi.localIP());
    }else{
        Serial.print(F("WL not connected Kadibouda"));
    }

}

String NetReader::readData(String path) {
    Serial.println("GET data "+path);
    String payload ="";
    if (WiFi.status() != WL_CONNECTED){
        ConnectToWiFi();
    }

    if(WiFi.status()== WL_CONNECTED){
        //return socketGet(path);
        HTTPClient http;
        String serverPath = serverName + path;
        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());
        //http.useHTTP10(true);
        int httpResponseCode = http.GET();

        if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            payload = http.getString();
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }


    //WiFi.disconnect();
    return payload;

}
