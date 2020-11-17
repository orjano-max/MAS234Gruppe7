

#include <Wire.h>
#include <WireIMXRT.h>
#include <WireKinetis.h>
#include <FlexCAN.h>          //Library for CAN-bus
#include <Adafruit_GFX.h>     //Library for Adafruit display functions
#include <Adafruit_SSD1306.h> //Library for OLED Display, set height to:64

#include <MPU6050.h>
#include "MPU6050.h"

MPU6050 imu;


#define OLED_DC     6   //Defining DC pin for OLED display
#define OLED_CS     10  //Defining CS pin for OLED display
#define OLED_RESET  5   //Defining RESET pin for oled display

Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);  //Set pins OLED display


#if (SSD1306_LCDHEIGHT != 64)         //Error if displayheight is wrong
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


static CAN_message_t txmsg,rxmsg;  //Initiate CAN TX and RX messages
volatile uint32_t count_RX = 0;   //Initiate CAN RX counter
volatile uint32_t count_TX = 0; //Initiate CAN TX counter

IntervalTimer TX_timer;  //Define interval timer


int16_t ax, ay, az, gx, gy, gz;

float az_gf = az *(9.81/16384);





void setup() {


  // put your setup code here, to run once:
  Serial.begin(115200);

  Can0.begin(250000); //Set CAN-bus Speed 
  Can1.begin(250000); 
  TX_timer.begin(tx_CAN, 1000000);    //Start CAN transmitting timer


 Wire.begin();

  if (!imu.begin(AFS_2G, GFS_250DPS)) {
      Serial.println("MPU6050 is online...");
  }
  else {
      Serial.println("Failed to init MPU6050");
      while (true) 
          ;
  }
  
  display.begin(SSD1306_SWITCHCAPVCC); //Initiate display
  display.clearDisplay(); //Clear display for old graphics
  
  for(int i = 0; i <= 100; i=i+11)  //For-loop to display Loading Screen
  {
      display.setTextSize(0);
      display.setTextColor(WHITE);
      display.setCursor(0,5);
      display.println(" MAS-234 Gruppe 7");
      display.println(" Oppgave 2");
      display.println(" Ver. 3.6");
      display.println(" ");
      display.println(" torsteingronn.co.uk");
      display.println(" ");
      display.print(" Loading: ");
      display.print(i);
      display.println("%");
      
      delay(80);
      display.display();
      display.clearDisplay();   
  }  
}


//From TX_timer, start transmitting back data recieved:
void tx_CAN(void)


{
  int ax1 = 0;
  int ax2 = 0;
  int ay1 = 0;
  int ay2 = 0;
  int az1 = 0;
  int az2 = 0;
  
  if(ax > 255){
    ax2 = ax/255;
    ax1 = ax-ax2; 
  }
  if(ay > 255){
    ay2 = ay/255;
    ay1 = ay-ay2; 
  }
  if(az > 255){
    az2 = az/255;
    az1 = az-az2; 
  }
  
  txmsg.len = 8;
  txmsg.id = 0x7DF;

  txmsg.buf[0] = 0;
  txmsg.buf[1] = 0;
  txmsg.buf[2] = ax1;
  txmsg.buf[3] = ax2;
  txmsg.buf[4] = ay1;
  txmsg.buf[5] = ay2;
  txmsg.buf[6] = az1;
  txmsg.buf[7] = az2 ;
  
  Can1.write(txmsg);
  count_TX++;
}


//Main loop:
void loop() {

  String CANStrTX;
  String CANStrRX;

  
 
  if (imu.getMotion6Counts(&ax, &ay, &az, &gx, &gy, &gz)) {
      Serial.print(ax);
      Serial.print(" ");
      Serial.print(ay);
      Serial.print(" ");
      Serial.print(az);
      Serial.print(" ");
      Serial.print(gx);
      Serial.print(" ");
      Serial.print(gy);
      Serial.print(" ");
      Serial.print(gz);
      Serial.println();
  }

    
    

  /*while(Can1.read(rxmsg) == false)
  {
       display.setTextSize(0);
    display.setTextColor(WHITE);
    display.setCursor(0,5);
    display.println(" MAS-234 Gruppe 7");
    display.println(" Oppgave 2");
    display.println(" Ver. 3.6");
    display.println(" ");
    display.println(" torsteingronn.co.uk");
    display.println(" ");
    display.print("IMU: ");
    display.println(az);

    
    delay(80);
    display.display();
    display.clearDisplay();
  }*/

     
      
    
 while(Can1.read(rxmsg)) 
  { 

     for (int i=0; i < 8; i++) //For-loop to store RX data package in string
     {     
         CANStrRX += String(rxmsg.buf[i],HEX);
         CANStrRX += ("") ;
     }

     for (int i=0; i < 8; i++) //For-loop to store TX data package in string
     {     
         CANStrTX += String(txmsg.buf[i],HEX);
         CANStrTX += ("") ;
     }

    count_RX++; //For counting every time a message is recieved   
  }

  if (imu.getMotion6Counts(&ax, &ay, &az, &gx, &gy, &gz)) {
        Serial.print(ax);
        Serial.print(" ");
        Serial.print(ay);
        Serial.print(" ");
        Serial.print(az);
        Serial.print(" ");
        Serial.print(gx);
        Serial.print(" ");
        Serial.print(gy);
        Serial.print(" ");
        Serial.print(gz);
        Serial.println();
    }

   //Drawing rounded rectangle with lines:
   display.drawRoundRect(0, 0, 128, 64, 5, WHITE);
   //display.drawLine(0, 15, 128, 15, WHITE);

   //Write out info on the display:
   display.setTextSize(0);
   display.setCursor(0,5);
   //display.println("MAS234 - Gruppe 7");
  // display.println(" ");
   display.print(" Raw TX:       ");
   display.println(count_TX);
   display.println(CANStrTX);
   display.print(" Processed RX: ");
   display.println(count_RX);
   display.print(" Last recieved ID: ");
   display.println(rxmsg.id);
   
 //  display.print(" IMU, z: ");
 //  display.print(-az_gf);
 //  display.print(" m/s^2");
   display.display();
   display.clearDisplay();
   delay(100);
  
}
