#include "gfx.h"
//MCUFRIEND_kbv tft;
ILI9488 tft = ILI9488(TFT_CS, TFT_DC, TFT_RST);


//#include "cmuntt28.h"
#include "cmuntt14.h"

#include "cmunobx8.h"

#include "cmuntbx16.h"

#include <FreeDefaultFonts.h>

//FreeMono12pt7b.h
//FreeSansBoldOblique12pt7b.h
#include <Fonts/FreeMonoBold18pt7b.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>

#include <Fonts/Org_01.h>


uint16_t barva(char r, char g, char b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

 uint16_t barva_freq,barva_suma, barva_amper, barva_volty_clanek, barva_volty  , tmava  , barva_pruh_amper , barva_watt  , bg_hodin, barva_hodin, ram_hodin , barva_datumu ;


 void Lgfx::init(){

   tft.begin();
   tft.setRotation(3);



  tft.setRotation(3);
  tft.fillScreen(BLACK);

  tft.fillScreen(0);
  tft.setTextColor(YELLOW,BLACK);




  //tft.setTextSize(1);
  //tft.setCursor(122,32);
  //tft.println("Radio");

 // tft.drawCircle(160, 120, 100, barva(0,90,0));
 // tft.drawCircle(160, 120, 110, barva(2,80,0));

  barva_freq = barva(055,255,95);

  barva_amper = barva(20,100,200);
 // barva_pruh_amper = barva(0,100,255);

  barva_volty= barva(00,205,60);
  barva_volty_clanek = barva(20,200,59);
  tmava=barva(35,35,35);
  barva_suma  = barva(250, 250,250);
  barva_watt = barva(255,100,00);

   bg_hodin = barva(0,0,50);
   barva_hodin = barva(0,200,200);
   ram_hodin = barva(177,152,00);
   barva_datumu = barva(177,150,0);

  }

void Lgfx::drawBatteryBox() {
   uint16_t ram = barva(60, 60, 60);  // nenaplna modroseda
   int bx = 295;
   int by = 14;
   int bw = 150;
   int bh = 240;
   tft.drawRect(bx, by, bw, bh, ram);
}

String lasti[60];

String tmave_minule[50];

  void reprint(int x, int y, uint16_t barva, String text, unsigned char darkslot){

    tft.setCursor(x,y);
    if (!text.equals( tmave_minule[darkslot] )){
      tft.setTextColor(BLACK);
      tft.print( tmave_minule[darkslot]);
      tft.setCursor(x,y);
    }
    tft.setTextColor(barva);
    tft.print(text);
     tmave_minule[darkslot]=text;


  }




void superPrint(uint16_t x,uint16_t y, uint16_t barva, uint16_t sirka, String s,  char slot){
      int xi;
      if (!lasti[slot] || lasti[slot].length()==0){
        lasti[slot]=String("                                                        ");
      }

      for (int i=0; i<s.length(); i++){
        xi=i*sirka+x;
        tft.setCursor(xi,y);

        if (s[i]!=lasti[slot][i]){
          tft.setTextColor(BLACK);
          tft.print(lasti[slot][i]);

        }
        tft.setCursor(xi,y);
          tft.setTextColor(barva);

          tft.print(s[i]);

      }
     lasti[slot]  = String(s);
  }







#include <Fonts/FreeSans12pt7b.h>




//  tft.setFont(&FreeSans12pt7b);



 //     tft.drawFastVLine(i,  240-y, y, b);
//      tft.drawFastVLine(i,  0, 240-y, tmava);




uint16_t barvaDleNapeti(unsigned int u){

    if (u<2800){
      return barva(255,40,0)    ;
    }

    if (u<3000){
      return barva(255,u-2800 +50  ,0)    ;
    }

  if (u<3100){
      return barva(255,255,0)  ;
    }

  if (u>3500){
        return barva(255,255,100)  ;
      }


  return barva_volty_clanek;
  }


void Lgfx::napeti(Dparser* dp ){
 //tft.setFont(&cmunobx8pt7b);
tft.setFont(&FreeSans12pt7b);
          int  y0= 45;
          int x=308;
float u;
//tft.fillRect(200 , 0 , 40, 200, barva(100,12,33));

        tft.setTextColor(barva_volty_clanek);
        int suma=0;
        int y=y0;
        for (int n=0; n<8; n++){

          long vv = dp->volty[n];
          u = (float)vv/1000;
          suma+= vv;
          String label = (vv == 0) ? "?.??" : String(u);
          reprint(x, y, barvaDleNapeti(vv) ,  label,  10+n);
          y+=25;

          if (n==3) y+=10;
        }

        reprint(x, y+25, barva_suma ,  String((double)suma/1000,1),  9);

suma=0;
        y=y0;
        x=378;

        for (int n=8; n<16; n++){
          long vv = dp->volty[n];
          u = (float)vv/1000;

          suma+=vv;
          String label = (vv == 0) ? "?.??" : String(u);
          reprint(x, y,barvaDleNapeti(vv) , label,  10+n);
          y+=25;
          if (n==11) y+=10;
        }


        reprint(x, y+25, barva_suma , String((double)suma/1000,1),  8);

  };

void vodarna(Dparser *dp){

	int y=140;
	int x=200;

	uint16_t b = barva(0,180,0);

	uint16_t b3 = barva(180,180,0);

	tft.drawRect(x , y , 60, 50, barva(250,250,50));

	if (dp->sudTimeout()> 11 * 60){
		 b = barva(90, 90, 90);
	}

	tft.setFont(&FreeSmallFont);
	tft.setCursor(x+15,y+15);
	tft.setTextColor(b3);
	tft.print("Sud");

	tft.setFont(&FreeSans12pt7b);
	reprint(x+8, y+38, b , dp->sud , 5);



}

void sklenikoveUdaje(Dparser *dp){

	int y=210;
	int x=8;



	uint16_t b = barva(200,230,230);
	uint16_t b2 = barva(180,200,180);
	uint16_t b3 = barva(180,180,0);

	tft.drawRect(x-4 , y-16 , 215, 55, barva(200,0,50));

	if (dp->sklenikTimeout()> 11 * 60){

		 b = barva(100,100,100);
		 b2 = barva(90,90,90);
	}

	tft.setFont(&FreeSmallFont);
	tft.setCursor(x,y);
	tft.setTextColor(b3); tft.print("dole");
	tft.setFont(&cmunbx16pt7b);
	reprint(x, y+30, b , String(dp->sauna1.number,1) , 2);


	x+=95;


	tft.setFont(&FreeSmallFont);
	tft.setCursor(x,y);
	tft.setTextColor(b3); tft.print("nahore");
	tft.setFont(&cmunbx16pt7b);
	reprint(x, y+30, b2 , String(dp->sauna2.number,1) , 3);



	tft.setFont(&cmunobx8pt7b);
	reprint(x+80, y+30, b3 , String(dp->sauna_bat.number,2) , 4);

}

void externiTeplomer(Dparser * dp){

	uint16_t b = barva(5,255,40);
   	int x=10;
   	int y=0;
   	tft.setTextColor(b);
	tft.setFont(&FreeSmallFont);
	tft.setCursor(x+4,y+20);
	tft.print("venku");
	tft.setFont(&cmunbx16pt7b);
	double v = dp->kadiba.number;
	reprint(x+4, y+20+30, b , String(v,1) , 6);



    b = barva(200,1,1);
    x=196;
    y=70;
    tft.setTextColor(b);
    tft.setFont(&FreeSmallFont);
    tft.setCursor(x+4,y+20);
    tft.print("sprcha");
    tft.setFont(&cmunbx16pt7b);
    v = dp->sprcha.number;
    reprint(x+4, y+20+30, b , String(v,1) , 36);



}

    void Lgfx::espUdaje(Dparser *dp){
    	sklenikoveUdaje(dp);
        externiTeplomer(dp);
    	vodarna(dp);
        int y=100;
        int x=12;
/*
      String venku= String(dp->kadiba.number,1);

      if (lasti[0]!= dp->voda || lasti[1]!=venku || lasti[2]!=dp->vlhko ){

    	  uint16_t b = barva(0,100,80);

          lasti[0] = dp->voda;
          lasti[1] = venku;
          lasti[2] = dp->vlhko;

      }
*/
        x=10;
        y=90;
        tft.setFont(&FreeSmallFont);
        tft.setCursor(x,y);
        tft.print("sklenik");
        tft.setFont(&cmunbx16pt7b);
        reprint(x, y+30, barva(150,150,175) , dp->sklenik , 1);


    }


String dvojcislo(int x){
    if (x<10){
      return "0"+String(x);
    }else{
      return String(x);
      }

  }

  String last_t_s="";

  bool ctverec_hodin=false;

  void Lgfx::printDateTime(DateTime * dt){




  int x=12;
  int y=300;

  if (!ctverec_hodin){
    tft.fillRect(x-7 , y-45 , 173, 54, bg_hodin);
    ctverec_hodin=true;
    tft.drawRect(x-7 , y-45 , 173, 54, ram_hodin);
  }

    //tft.setFont(&FreeSans12pt7b);
    //tft.setFont(&cmunbx16pt7b);
  tft.setFont(&FreeMonoBold18pt7b);

    tft.setCursor(x,y);

    String s = dvojcislo(dt->hour())+":"+dvojcislo(dt->minute() )+":"+dvojcislo(dt->second());
    superPrint(x, y, barva_hodin , 20, s , 7);


tft.setFont(&cmunobx8pt7b);

    String datum = String(dt->day())+"."+String(dt->month())+".";
    superPrint(x, y-30, barva_datumu , 15, datum , 6);

  }

void Lgfx::connecting(String s) {
    connecting(s, "");
}

void Lgfx::connecting(String s, String detail) {

    int x = 60;
    int y = 45;

    tft.fillRect(x, y, 360, 70, BLACK);

    tft.setFont(&FreeSans12pt7b);

    if (s.length() > 0) {
        tft.setCursor(x + 3, y + 26);
        tft.setTextColor(RED);
        tft.print(s);
    }

    if (detail.length() > 0) {
        tft.setCursor(x + 3, y + 56);
        tft.setTextColor(barva(200, 200, 0));
        tft.print(detail);
    }
}


// Formátuje teplotu pro displej: "--.-" pokud senzor není připojen (NAN)
String formatTeplota(float t) {
    if (isnan(t)) return "--.-";
    return String(t, 1);
}

  void Lgfx::ruzneUdaje(float teplota1, float teplota2, float teplota3, float venku, float tlak){
      int x=15;
      int y=160;

      tft.setFont(&cmuntt14pt7b);
      superPrint(x+80, y, barva(90,90,90) , 15, String(teplota1,1) , 4);
      superPrint(x, y, YELLOW , 15, String(tlak,0)+" " , 5);



      	x=100;
		tft.setFont(&FreeSmallFont);
		tft.setCursor(x,20);
        tft.setTextColor(YELLOW);
		tft.print("tady");
		tft.setFont(&cmunbx16pt7b);
		reprint(x, 50, YELLOW , formatTeplota(teplota2) , 0);



		uint16_t b = barva(0,180,255);

      	x=210;
		tft.setFont(&FreeSmallFont);
		tft.setCursor(x,20);
        tft.setTextColor(b);
		tft.print("voda");
		tft.setFont(&cmunbx16pt7b);
		reprint(x, 50, b , formatTeplota(teplota3) , 35);


		// venkovni teplota ze senzoru (Dallas VENKU)
		uint16_t bv = barva(5, 255, 40);
		x = 10;
		tft.setFont(&FreeSmallFont);
		tft.setCursor(x + 4, 20);
		tft.setTextColor(bv);
		tft.print("venku");
		tft.setFont(&cmunbx16pt7b);
		reprint(x + 4, 50, bv , formatTeplota(venku) , 6);






  }
