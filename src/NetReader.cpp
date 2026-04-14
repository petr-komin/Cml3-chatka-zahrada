//
// Created by manx on 30.12.21.
//

#include "NetReader.h"
#include "secrets.h"


#include "WiFi.h"
#include <HTTPClient.h>


#include <WiFiUdp.h>


String serverName = SERVER_URL;
const char cml_ssid[] = WIFI_SSID_CML;
WiFiUDP ntpUDP;


static String wifiStatusText(wl_status_t s) {
    switch (s) {
        case WL_IDLE_STATUS:     return "idle...";
        case WL_NO_SSID_AVAIL:   return "SSID nenalezeno!";
        case WL_SCAN_COMPLETED:  return "scan ok";
        case WL_CONNECTED:       return "pripojeno";
        case WL_CONNECT_FAILED:  return "spatne heslo!";
        case WL_CONNECTION_LOST: return "spojeni ztraceno";
        case WL_DISCONNECTED:    return "odpojen / timeout";
        default:                 return "status=" + String((int)s);
    }
}


void ConnectInternet(Lgfx * gfx) {
    const char ssid[] = WIFI_SSID;
    const char pass[] = WIFI_PASSWORD;

    Serial.println("[WiFi] Odpojuji predchozi spojeni...");
    WiFi.disconnect(true);
    delay(200);
    WiFi.mode(WIFI_STA);
    delay(100);

    // --- scan: vypiseme vsechny viditelne site ---
    Serial.println("[WiFi] Skenuji site...");
    gfx->connecting("WiFi: skenuju site...", "");
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("[WiFi] Scan: zadne site nenalezeny!");
        gfx->connecting("WiFi: zadne site!", "zkontroluj antenu");
    } else {
        Serial.printf("[WiFi] Scan: nalezeno %d siti:\n", n);
        bool found = false;
        for (int j = 0; j < n; j++) {
            String name = WiFi.SSID(j);
            int rssi = WiFi.RSSI(j);
            Serial.printf("  [%d] \"%s\"  RSSI=%d dBm  ch=%d\n",
                          j + 1, name.c_str(), rssi, WiFi.channel(j));
            if (name == String(ssid)) found = true;
        }
        if (!found) {
            Serial.printf("[WiFi] POZOR: sit \"%s\" v scanu NENI viditelna!\n", ssid);
            gfx->connecting("WiFi: SSID nenalezeno", "sit neni v dosahu?");
        } else {
            Serial.printf("[WiFi] Sit \"%s\" nalezena, pripojuji...\n", ssid);
        }
    }
    WiFi.scanDelete();
    // --- konec scanu ---

    WiFi.begin(ssid, pass);

    Serial.print("[WiFi] Pripojuji k: ");
    Serial.println(ssid);

    gfx->connecting("WiFi: " WIFI_SSID, "cekam...");

    // az 20 pokusu x 500ms = 10 sekund
    uint8_t i = 0;
    wl_status_t st;
    while ((st = WiFi.status()) != WL_CONNECTED && i < 20) {
        delay(500);
        i++;

        String statusStr = wifiStatusText(st);
        Serial.printf("[WiFi] pokus %d/20  status: %s\n", i, statusStr.c_str());
        gfx->connecting("WiFi: " WIFI_SSID,
                        String(i) + "/20  " + statusStr);

        // pouze spatne heslo je permanentni chyba - vse ostatni zkousime dal
        if (st == WL_CONNECT_FAILED) {
            Serial.println("[WiFi] Spatne heslo, koncim.");
            break;
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
        String ip = WiFi.localIP().toString();
        Serial.print("[WiFi] Pripojeno! IP: ");
        Serial.println(ip);
        gfx->connecting("WiFi: pripojeno", ip);
        delay(300);
    } else {
        String errStr = wifiStatusText(WiFi.status());
        Serial.print("[WiFi] CHYBA: ");
        Serial.println(errStr);
        gfx->connecting("WiFi: CHYBA", errStr);
        delay(2000);
    }
}



String NetReader::ntp(Lgfx * gfx){

    ConnectInternet(gfx);
    NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0, 60000);
    if(WiFi.status()== WL_CONNECTED) {
        Serial.println("[NTP] Stahuji cas...");
        gfx->connecting("NTP: stahuji cas...", "europe.pool.ntp.org");

        timeClient.begin();
        bool ok = timeClient.update();

        if (ok) {
            String date = timeClient.getFormattedDate();
            String time = timeClient.getFormattedTime();
            Serial.print("[NTP] Datum: "); Serial.println(date);
            Serial.print("[NTP] Cas:   "); Serial.println(time);
            gfx->connecting("NTP: OK  " + time, date);
            delay(1500);
        } else {
            Serial.println("[NTP] update() selhal!");
            gfx->connecting("NTP: CHYBA", "update selhal");
            delay(2000);
        }
        timeClient.end();
    }else{
        Serial.println("[NTP] WiFi neni pripojeno, preskakuji NTP.");
        gfx->connecting("NTP: preskoceno", "neni WiFi");
        delay(1500);
        return "";
    }
    WiFi.disconnect();
    return timeClient.getFormattedDate();
}

void NetReader::ConnectToWiFi() {

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID_CML);
    Serial.println("Connecting  " WIFI_SSID_CML);

    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 31) {
        Serial.print('.');
        delay(500);

        if ((++i % 16) == 0) {
            Serial.println(F(" still trying to connect " WIFI_SSID_CML));
        }
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.print(F("Connected. My IP address is: "));
        Serial.println(WiFi.localIP());
    }else{
        Serial.print(F("WL not connected " WIFI_SSID_CML));
    }

}

int  NetReader::zapis(float teplota1, float teplota2, float teplota3, float tlak, Dparser *  dp ){
    String path = "/";

    String d = "{\"s\":\"ch\",";

   // d += "\"t1\":\"" + String(teplota1) + "\",";
    d += "\"t2\":\"" + String(teplota2,1) + "\",";
    d += "\"t3\":\"" + String(teplota3,1) + "\",";
    d += "\"tlak\":\"" + String(tlak,0) + "\",";

    d+= "\"u\":[";
    for(int i =0; i<24; i++){
        d+= String(dp->volty[i]);
        if (i<23) d+=",";
    }
    d+="]}";

    Serial.print( "poslat: ");
    Serial.println( d);

    if(WiFi.status()== WL_CONNECTED){

        HTTPClient http;
        String serverPath = serverName + path;
        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());
        int httpResponseCode = http.POST(d);
        http.addHeader("Content-Type", "application/json");
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        http.end();
    }else{
        return 1;
    }

    return 0;
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
