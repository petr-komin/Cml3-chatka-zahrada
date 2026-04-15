# Chatka-zahrada — monitorovací stanice

ESP32 firmware pro meteorologicko-monitorovací stanici na chatě. Zobrazuje teploty, atmosferický tlak, stav solárních baterií a přijímá data ze vzdálených bezdrátových senzorů přes ESP-NOW.

## Hardware

| Komponenta | Popis |
|---|---|
| ESP32 (ESP-WROVER-KIT) | hlavní MCU |
| ILI9488 TFT 3.5" (480×320) | displej, SPI |
| BMP280 | teplota + atmosferický tlak, I²C (0x76) |
| DS18B20 × 3 | teploty: tady / voda / venku, OneWire (GPIO4) |
| RTC DS1307 | hodiny reálného času, I²C |
| Bateriový modul | napětí 16 solárních článků, Serial2 (4800 baud) |

**Zapojení:**

| Signál | GPIO |
|---|---|
| TFT CS | 5 |
| TFT DC | 21 |
| TFT RST | 22 |
| TFT MOSI | 13 |
| TFT CLK | 14 |
| TFT MISO | 12 |
| I²C SDA (BMP280 + RTC) | 33 |
| I²C SCL (BMP280 + RTC) | 32 |
| OneWire (DS18B20) | 4 |

## Co zobrazuje

- **Nahoře:** teploty venku / tady / voda (DS18B20)
- **Vlevo uprostřed:** vzdálené senzory (ESP-NOW, viz níže)
- **Dole vlevo:** hodiny + datum (RTC, synchronizováno přes NTP)
- **Dole vedle hodin:** atmosferický tlak (hPa)
- **Vpravo:** napětí 16 solárních článků (2 skupiny po 8)

## Konfigurace

Zkopíruj šablonu a vyplň své hodnoty:

```bash
cp include/secrets.h.example include/secrets.h
```

Obsah `secrets.h`:

```cpp
#define WIFI_SSID      "nazev_wifi"       // pro NTP sync
#define WIFI_PASSWORD  "heslo_wifi"
#define WIFI_SSID_CML  "lokalni_wifi"     // lokální síť chatky (zatím nevyužito)
#define SERVER_URL     "http://192.168.x.x"  // lokální server (zatím nevyužito)

// ROM adresy DS18B20 — zjistíš je ze Serial monitoru při prvním spuštění
// (výpis: "Dallas[n] adresa: { 0x28, ... }")
#define DALLAS_ADDR_TADY   { 0x28, ... }
#define DALLAS_ADDR_VODA   { 0x28, ... }
#define DALLAS_ADDR_VENKU  { 0x28, ... }
```

## Build a nahrání

```bash
pio run              # sestavení
pio run -t upload    # nahrání na desku
pio device monitor   # sériový monitor (57600 baud)
```

nebo zkratka pro monitor:

```bash
./monit.sh
```

---

## Vzdálené senzory (ESP-NOW)

Stanice přijímá data ze vzdálených senzorů přes **ESP-NOW** — proprietární protokol Espressif. Nevysílá žádný WiFi beacon, senzory se nepřipojují jako klienti. Senzor se probudí z deep sleep, odešle jeden paket a jde zpět spát — celé to trvá desítky milisekund.

### Datová struktura paketu

Senzor musí odeslat přesně tuto strukturu (viz `include/RemoteSensor.h`):

```cpp
struct EspNowPacket {
    char     label[16];     // identifikátor senzoru: "zahrada", "sklep", ...
    uint16_t baterie_mv;    // napětí baterie v milivoltech, např. 3201
    char     hodnota[32];   // volný string: "22.3" nebo "22.3 / 65%"
};
```

- `label` — krátký textový identifikátor (max 15 znaků + `\0`). Slouží zároveň jako popisek na displeji. Podle `label` se senzor rozpozná — při každém dalším paketu se přepíše hodnota ve stejném slotu.
- `baterie_mv` — napětí baterie senzoru v mV. Zobrazuje se malým písmem vedle hodnoty. Použij `0` pokud napětí neměříš.
- `hodnota` — libovolný formátovaný string (max 31 znaků + `\0`). Senzor si sám formátuje co chce zobrazit.

### Příklad firmware pro senzor (ESP8266 / ESP32)

```cpp
#include <Arduino.h>
#include <espnow.h>      // ESP8266: espnow.h | ESP32: esp_now.h
#include <ESP8266WiFi.h> // ESP8266: ESP8266WiFi.h | ESP32: WiFi.h

// MAC adresa hlavní stanice — zjistíš ji ze Serial monitoru při startu
// (výpis: "ESP-NOW prijimac inicializovan" + MAC z WiFi.macAddress())
uint8_t masterMAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

struct EspNowPacket {
    char     label[16];
    uint16_t baterie_mv;
    char     hodnota[32];
};

void setup() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    esp_now_init();

    // přidej přijímač
    esp_now_add_peer(masterMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

    // sestav paket
    EspNowPacket pkt;
    memset(&pkt, 0, sizeof(pkt));
    strncpy(pkt.label, "zahrada", 16);
    pkt.baterie_mv = 3350;                    // změř ADC
    snprintf(pkt.hodnota, 32, "%.1f", 22.3f); // změř senzor

    esp_now_send(masterMAC, (uint8_t*)&pkt, sizeof(pkt));

    // jdi spát — probudí se za N minut
    ESP.deepSleep(10 * 60 * 1000000ULL); // 10 minut v mikrosekundách
}

void loop() {}
```

> **ESP32 senzor:** místo `espnow.h` použij `esp_now.h` a `WiFi.h`, funkce `esp_now_add_peer` má jiný prototyp — viz ESP-IDF dokumentaci nebo příklady PlatformIO.

### Jak stanice zpracovává příchozí pakety

1. Přijde paket — zkontroluje se délka (`sizeof(EspNowPacket)`)
2. Hledá se slot podle `label`
   - nalezen → přepíše se `hodnota`, `baterie_mv`, `lastSeen`
   - nenalezen → obsadí se první volný slot (max 3 senzory)
   - žádný volný slot → paket se zahodí (vypíše varování na Serial)
3. Pokud senzor nepošle data déle než **30 minut**, zobrazí se šedě (timeout)

### Zjištění MAC adresy hlavní stanice

Po nahrání firmware se na Serial monitoru (57600 baud) zobrazí:

```
ESP-NOW prijimac inicializovan
```

MAC adresu zjistíš přidáním do `setup()` (nebo dočasně):

```cpp
Serial.println(WiFi.macAddress());
```
