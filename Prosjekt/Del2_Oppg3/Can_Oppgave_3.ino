
#include <Wire.h>
#include <FlexCAN.h>          //Library for CAN-bus
#include <Adafruit_GFX.h>     //Library for Adafruit display functions
#include <Adafruit_SSD1306.h> //Library for OLED Display, set height to:64


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



void setup() {

  Serial.begin(115200);

  Wire.begin();

  if (!imu.begin(AFS_2G, GFS_250DPS)) {
      Serial.println("MPU6050 is online...");
  }
  else {
      Serial.println("Failed to init MPU6050");
      while (true) 
          ;
  }
  
  Can0.begin(500000); //Set CAN-bus Speed 
  TX_timer.begin(tx_CAN, 1000000);    //Start CAN transmitting timer
  
  display.begin(SSD1306_SWITCHCAPVCC); //Initiate display
  display.clearDisplay(); //Clear display for old graphics
  
  for(int i = 0; i <= 100; i=i+3)  //For-loop to display Loading Screen
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
      //Clear display after loading screen
      display.display();
      display.clearDisplay();   
  }  
}

//From TX_timer, start transmitting back data recieved:
void tx_CAN(void)
{
  txmsg.len = 8;
  txmsg.id = 0x7DF;

  for(int i = 0; i < 8; i++)
  {
    txmsg.buf[i] = rxmsg.buf[i];
  }
  
  Can0.write(txmsg);
  count_TX++;
}


//Main loop:
void loop() {

  String CANStrTX;
  String CANStrRX;
  
  while(Can0.read(rxmsg)) 
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
  
  //Clear display before writing new values:
  display.display();
  display.clearDisplay();
  
  //Drawing rounded rectangle with lines:
  display.drawRoundRect(0, 0, 128, 64, 5, WHITE);
  display.drawLine(0, 15, 128, 15, WHITE);
  display.drawLine(5, 30, 90, 30, WHITE);
  
  //Write out info on the display after first message is recieved:
  display.setTextSize(0);
  display.setCursor(15,5);
  display.println("MAS234 - Gruppe 7");
  display.println(" ");
  display.println(" CAN-statistikk");
  display.println(" ");
  display.print(" Antall mottatt: ");
  display.println(count_RX);
  display.println(" ");
  display.print(" Mottok sist ID: ");
  display.println(rxmsg.id);
  
}
