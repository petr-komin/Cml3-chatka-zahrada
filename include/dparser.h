#include <Arduino.h>
#ifndef dparser_h
#define dparser_h


class Udaj{
    public:
        String str="";
        double number;
        long timestamp;
};

class Dparser{

    void parseLine(String line);
    void parseVoltageLine(String line, int);
    void parseDatetime(String val);
    void parseSklenikData(String s);


  public:
    Udaj sauna1;
    Udaj sauna2;
    Udaj sauna_bat;
    Udaj kadiba;


    String voda="*";

    String vlhko="*";
    String sud="???";
    String sklenik="?*";
    String zaSklenikem="?-";
    String baterySklenik="?.?";
    uint32_t sklenikRXTime=0;
    uint32_t sudRXTime=0;
  //  STM32F1_RTC *rtc;

    String datareqPath="";

    long volty[32];
    int timebuf[8];
    bool timeRecieved=false;
    void parseBuf(String s);
    void parseBateryData(String s);
    long sklenikTimeout();
    long sudTimeout();

    int parseKadibouda(String data);
    void parseKadiTime(String data);

};

#endif
