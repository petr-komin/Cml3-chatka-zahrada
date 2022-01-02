//
// Created by manx on 30.12.21.
//

#ifndef CML5_NETREADER_H
#define CML5_NETREADER_H

#include <Arduino.h>
#include <NTPClient.h>

#include "gfx.h"

class NetReader {

    public:
        int httpResponseCode=0;

        void ConnectToWiFi();
        String readData(String path);
        String ntp(Lgfx *);


};


#endif //CML5_NETREADER_H
