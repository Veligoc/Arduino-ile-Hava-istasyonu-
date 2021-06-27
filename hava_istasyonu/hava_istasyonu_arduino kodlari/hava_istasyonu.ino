#include <Wire.h>   
#include <SFE_BMP180.h> //bmp180kütüphanesi
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#define DHT11PIN 4// DHT11PIN olarak Dijital 4"yi belirliyoruz.
dht11 DHT11;
#include <SoftwareSerial.h>
SFE_BMP180 bmp180;
#include <LiquidCrystal.h>

//               (Rs, E, D4, D5, D6 ,D7)
LiquidCrystal lcd(13 , 10, 9 , 8 , 6 , 5);//lcd pinlerini bu pinlere atadık

int rxPin = 0;                                               //ESP8266 RX pini
int txPin = 1;                                               //ESP8266 TX pini
int toprak = 0;
int ruzgar = 0;
int isik = 0;
float disnem;
String agAdi = "agadi" ;                 // Adımızın adını yazıyoruz.    
String agSifresi = "agsifresi" ;           // Ağımızın şifresini buraya yazıyoruz.
String ip = "184.106.153.149";                                //Thingspeak ip adresi
SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.
void setup() {

lcd.begin(16,2); 

 esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
   esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
     lcd.setCursor (0, 0);
        lcd.print("at yollandi");
  
  while(!esp.find("OK")){                                     //Modül hazır olana kadar bekliyoruz.
      esp.println("AT");
         lcd.clear();
            lcd.print("esp bulunamadi");

  }
    lcd.setCursor (0, 1);
      lcd.clear();
        lcd.print("ok alindi");
                 
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
    while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
       esp.println("AT+CWMODE=1");
          lcd.clear();
            lcd.print("Ayar Yapiliyor...");
  }

  lcd.clear();
    lcd.print("Client ayarlandi");
      lcd.clear();
         lcd.print("Aga Baglaniliyor..");
            esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
                 while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
   lcd.clear();
      lcd.print("Aga Baglandi.");
        delay(1000);
 lcd.clear();

  bool baglanti = bmp180.begin();

  if (baglanti) {
  //  Serial.println("BMP180 baglanti saglandi"); 
  }
}

void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
      if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
          lcd.clear();
             lcd.print("AT+CIPSTART Error");
  }

  char status;
  double T=0, P=0;
  bool baglanti = false;
   
//***************bmp180 kısmı************basınç ve sıcaklık 
  status = bmp180.startTemperature();

  if (status != 0) {
    delay(1000);
    status = bmp180.getTemperature(T);

    if (status != 0) {
      status = bmp180.startPressure(3);

      if (status != 0) {
        delay(status);
        status = bmp180.getPressure(P, T);

        if (status != 0) {
      lcd.clear();
               lcd.setCursor (0, 0);
                 lcd.print("P");
                    lcd.print(P);
                    
               lcd.setCursor (10, 0);
                  lcd.print("T");
                    lcd.print(T);


     //*************dht11 kısmı************** nem 

int chk = DHT11.read(DHT11PIN);

     lcd.setCursor(0,1);
        lcd.print("%");
           disnem = DHT11.humidity;
           int nemm = (int)DHT11.humidity;
              lcd.print(nemm);
          
     //*************toprak sensörü*************7
         int topraksensoru = analogRead(A1);
         toprak = map(abs(topraksensoru), 0, 1024, 99, 0);
      
    lcd.setCursor(4,1);
       lcd.print("%");
        lcd.print(toprak);
         
  //*************rüzgar sensörü*************7
         int ruzgarsensoru = analogRead(A0);
         ruzgar = map(abs(ruzgarsensoru), 0, 100, 0, 99);
      
    lcd.setCursor(7,1);
       lcd.print("H");
         lcd.print(ruzgar);
              
//*************ısık sensörü*************7
       
    int isiksensoru = analogRead(A2);
    isik = map(abs(isiksensoru), 0, 1024, 0, 99);
      
    lcd.setCursor(11,1);
       lcd.print("i");
         lcd.print(isik);
                 
String veri = "GET https://api.thingspeak.com/update?api_key=521GO5I6FG0IJD36";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.

  veri += "&field1=";
  veri += String(T);
  veri += "&field2=";
  veri += String(disnem);                                        //Göndereceğimiz  değişkenler
  veri += "&field3=";
  veri += String(toprak);
  veri += "&field4=";
  veri += String(P);  
  veri += "&field5=";
  veri += String(ruzgar);
  veri += "&field6=";
  veri += String(isik);  
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+6);
  delay(2000);
  
  
    esp.print(veri);                                          //Veriyi gönderiyoruz.
        lcd.setCursor(15,1);
       lcd.print("*");
  //  lcd.clear();
     //  lcd.print("Veri gonderildi");
     //    delay(2000);

   // lcd.clear();
     // lcd.print("Baglanti Kapatildi.");
       // esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
           delay(1000);             

         
        }
      }
    }    
  }
}
