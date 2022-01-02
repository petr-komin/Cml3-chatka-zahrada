#ifndef cmlgfx_h
#define cmlgfx_h

#include <Arduino.h>
//#define HARLEYKONFIG  1
//#include <MCUFRIEND_kbv.h>  // LCD Sheild library slightly modified utility\mcufriend_shield.h


#include <SPI.h>
#include <Adafruit_GFX.h>
#include <ILI9488.h>


#include "dparser.h"
#include "RTClib.h"



#define BLACK   0x0000
#define BLUE    0x0010
#define RED     0xF800
#define GREEN   0x0400
#define CYAN    0x0410
#define MAGENTA 0x8010
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGBTO565(_r, _g, _b) ((((_r) & B11111000)<<8) | (((_g) & B11111100)<<3) | ((_b) >>3))


//DateVar date;
//TimeVar time;


#define TFT_CS   5
#define TFT_DC   21

#define TFT_RST  22
#define TFT_MOSI 13
#define TFT_CLK  14
#define TFT_MISO 12


class Lgfx{



  public:
    void init();
    void refreshFreq(unsigned long fr);

    void drawVoltage(float v);
    void drawAmper(float amper);

    void drobneUdaje(unsigned long int sec, long int x, int raw_i );
    void drawWatt(float w);

    void pruhVolty(float V);

    void printADC(int,int,int,int);

    void napeti( Dparser *);

    void espUdaje(Dparser *);

    void printDateTime(DateTime *);

    void ruzneUdaje(float teplota1, float t2, float t3, float tlak);

    void connecting(String s);

  };

#endif
