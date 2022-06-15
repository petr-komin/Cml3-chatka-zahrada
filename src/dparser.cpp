  #include "dparser.h"

#include <ArduinoJson.h>

  //#define Serial Serial3




void Dparser::parseDatetime(String val){

  int i=0;
  int k=0;
  String cislo="";
    for(i=0; i< val.length(); i++){

      char c = val[i];

        if (c>='0' && c<='9'){
          cislo+=c;
        }
        if (c==',' || c==')'){

          if (k<8){
            timebuf[k]=cislo.toInt();
            k++;
            cislo="";
          }
        }
    }

/*
Serial.print("Cas:[ ");
for(i=0; i<8; i++){
    Serial.print(String(i)+" => "+timebuf[i]+"      ");
  }
   Serial.println("]");
*/
   if (timebuf[0]>2020 && timebuf[7]<367 && timebuf[7]>=0){
    timeRecieved = true;
   }
}



void Dparser::parseLine(String line){

  String key="";
  String val="";

  int i=0;
  while(i<line.length()){
     if (line[i]==';') break;
     key+=line[i];
     i++;
  }
  i++;
  while(i<line.length()){
     if (line[i]!=' ') {
        val+=line[i];
     }
     i++;
  }


  //Serial.println("Key="+key+"   val="+val);

  int k = key.toInt();

  if (k==1){
    voda = val;
  }


  if (k==3) vlhko=val;

  //if (k==0) parseDatetime(val);
}

void Dparser::parseVoltageLine(String line, int lineNum){


     int numpos = line.indexOf(':');
     if (numpos==1){

      int ln = line.substring(0,1).toInt();

      int d1 = line.indexOf("[", 2);
      int d2 = line.indexOf("]", d1+2+2);
      //Serial.print("\t->("+String(d1)+".."+String(d2)+")" + line.substring(d1+1,d2));
      String vnitrek = line.substring(d1+1,d2);
      int i=0;
      String numerek;
      int n=0;
      while(i<vnitrek.length()){
        char z=vnitrek[i];
        if (z>='0' && z<='9'){
          numerek += z;
        }
        if (z==',' || z==']'){
          long v = numerek.toInt();
          if (ln<8 && n<4 && v>1000 && v<6000){
            volty[ ln *4 + n ] = v;
          }
          numerek="";
          n++;
        }

        i++;
      }

     }

}


void Dparser::parseBateryData(String s){
  if (s.length()<24) return;


 int i=0;
     int maxi=s.length();
     String line="";
     int l=0;
     while(i<maxi){

        if (s[i]=='\n'){
          //Serial.println("   *line "+String(l)+": "+line);
          parseVoltageLine(line, l);
          line="";
          l++;
        }else{
          line+=  s[i];
        }
        i++;
     }


 /* Serial.print("volty:");
  for (int i=0; i<20; i++){
    Serial.print(i);
    Serial.println(": "+String(volty[i]));
  }
 */
}

long Dparser::sklenikTimeout(){

	uint32_t t= 0 ;//rtc->getTime();
	return t - sklenikRXTime;

}

long Dparser::sudTimeout(){
	uint32_t t= 0 ;//rtc->getTime();
	return t - sudRXTime;
}





int Dparser::parseKadibouda(String data){

    String line="";
    for(int i=0; i<data.length(); i++){
        if ((char)data[i]==10) {
            int x = line.indexOf(' ');
            String json = line.substring(x+1);
            String cas = line.substring(0, x);
            // Serial.println(">>"+ cas  + "'"+ json +"'");

            if (json.length()>1){

                StaticJsonDocument<600> doc;
                DeserializationError error = deserializeJson(doc, json);

                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());
                    return -1;
                }
//{"s":"ch","t2":"21.2","t3":"22.2","tlak":"1029","u":[3289,3297,3290,3297,3289,3290,3290,3282,3297,3282,3297,3290,3297,3289,3297,3283,0,0,0,0,0,0,0,0]}
                const char* sens = doc["s"];
                String sensor = String(sens);

                if (sensor == "tep1") {
                    sauna1.number = doc["t1"];
                    sauna2.number = doc["t2"];
                    sauna_bat.number = doc["bat"];
                }

                if (sensor == "Kadib") {
                    kadiba.number = doc["t"];

                    sprcha.number = doc["tv"];
                }

                if (sensor == "sud"){
                    double x =doc["v"];
                    sud = String(x,1);
                }

                if (sensor == "skl"){
                    double x =doc["t"];
                    sklenik = String(x,1);
                }


            }else{
                Serial.println("Nic tam nebylo...");
            }

            line="";
        }else{
            line += data[i];
        }
    }
}

void Dparser::parseKadiTime(String data){

    int x = data.indexOf('T');

    String datumStr = data.substring(0,x);
    String casStr = data.substring(x+1, data.length());

    Serial.println(" .... dt ... ["+datumStr+"] ["+casStr+"]");
    String l ="";
    int hour;
    int minute;
    int sec;
    String ln[3];
    int a=0;
    ln[a]="";
    for( int i=0; i< casStr.length(); i++){
        if (casStr[i]!=':'){
            ln[a] += casStr[i];
        }else{
            a++;
            ln[a]="";
        }
    }


    Serial.println(" cas ka kusy {" + ln[0] + " " +  ln[1]  + " " + ln[2]+"}");

    datareqPath = "/data/"+ datumStr+"/"+ln[0]+".txt";

}