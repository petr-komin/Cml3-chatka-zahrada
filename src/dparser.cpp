  #include "dparser.h"



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

  if (k==2) venku=val;
  if (k==3) vlhko=val;

  //if (k==0) parseDatetime(val);
}

void Dparser::parseBuf(String s){
  timeRecieved = false;
//void parseData(String s){

  int d1 = s.indexOf("<data>", 0);
  int d2 = s.indexOf("</data>", d1+6);


 // Serial.println("hledani "+String(d1)+ "  "+String(d2));
  if (d1>=0 && d2>d1+26){
    String sub =  s.substring(d1+6, d2);
   //  Serial.println("vnitrek "+ sub);

     int i=0;
     int maxi=sub.length();
     String line="";
     int l=0;
     while(i<maxi){

        if (sub[i]=='\n'){
          Serial.println(" line "+String(l)+":"+line);
          parseLine(line);
          line="";
          l++;
        }else{
          line+=  sub[i];
        }
        i++;
     }
  }

  d1 = s.indexOf("<time>", 0);
  d2 = s.indexOf("</time>", d1+6);

  if (d1>=0 && d2>d1+20){
    String sub =  s.substring(d1+6, d2);
    parseDatetime(sub);

  }


  d1 = s.indexOf("<skl>", 0);
  d2 = s.indexOf("</skl>", d1+5);
  if (d1>=0 && d2>d1+10){
      String sub =  s.substring(d1+5, d2);
      parseSklenikData(sub);

  }

  d1 = s.indexOf("<sud>", 0);
  d2 = s.indexOf("</sud>", d1+5);
  if (d1>=0 && d2>d1+5){
	  sud = s.substring(d1+5, d2);
	//  sudRXTime = rtc->getTime();
  }


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

void Dparser::parseSklenikData(String s){
// 	<skl>
//	<cas>(2000, 1, 1, 2, 7, 9, 5, 1)</cas>
//	<t1>23.1</t1>
//	<t2>16.1</t2>
//	<u>3.28</u>
//	</skl>
	int a, b;

	a= s.indexOf("<t1>", 0);
	b= s.indexOf("</t1>", a+4);

	if (a>=0 && b>=0) sklenik= s.substring(a+4, b);

	a= s.indexOf("<t2>", 0);
	b = s.indexOf("</t2>", a+4);

	if (a>=0 && b>=0) zaSklenikem=s.substring(a+4, b);

    a= s.indexOf("<u>", 0);
    b = s.indexOf("</u>", a+3);

    if (a>=0 && b>=0) baterySklenik=s.substring(a+3, b);

    Serial.println("Sklenik: "+sklenik+ " / "+zaSklenikem+ " / "+baterySklenik);
  //  sklenikRXTime = rtc->getTime();
};

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
