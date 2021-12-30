//
// Created by manx on 30.12.21.
//

#ifndef CML5_NETREADER_H
#define CML5_NETREADER_H

#include <Arduino.h>
#include <NTPClient.h>



class NetReader {

    public:
        void ConnectToWiFi();
        void readData();
        void ntp();


};


#endif //CML5_NETREADER_H
