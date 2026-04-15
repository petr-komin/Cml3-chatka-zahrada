#pragma once
#include <Arduino.h>

#define REMOTE_SENSOR_COUNT  3
#define REMOTE_TIMEOUT_MS    (30UL * 60UL * 1000UL)  // 30 minut v ms

// Paket posílany senzorem přes ESP-NOW
struct EspNowPacket {
    char     label[16];     // identifikátor i popisek senzoru: "zahrada", "sklep"
    uint16_t baterie_mv;    // napětí baterie v milivoltech: 3201
    char     hodnota[32];   // volný formátovaný string: "22.3" nebo "22.3 / 65%"
};

// Slot v paměti přijímače — aktualizuje se při každém přijatém paketu
struct RemoteSensor {
    char     label[16];
    uint16_t baterie_mv;
    char     hodnota[32];
    uint32_t lastSeen;      // millis() při posledním příjmu
    bool     active;        // slot je obsazen
};
