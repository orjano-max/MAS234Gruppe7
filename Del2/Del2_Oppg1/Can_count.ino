

#include <FlexCAN.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

static CAN_message_t msg,rxmsg;
volatile uint32_t count = 0;
IntervalTimer TX_timer;
volatile uint32_t can_msg_count = 0;




void setup() {
  // put your setup code here, to run once:
  
  Can0.begin(500000); //Sett CanBus hastigheten 


  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // Clear the buffer.
  display.clearDisplay();
  
  delay(1000);
  Serial.println(F("CAN Bus Tx test"));

  for(int i = 0; i <= 100; i=i+3)
  {
      display.setTextSize(0);
      display.setTextColor(WHITE);
      display.setCursor(0,5);
      display.println(" Fitta Til Torstein   Er gronn. Ver. 3.6");
      display.println(" ");
      display.println(" fittaergronn.co.uk");
      display.println(" ");
      display.print(" loading: ");
      display.print(i);
      display.println("%");
      
      delay(80);
      display.display();
      display.clearDisplay();   
  }

  
  msg.buf[0] = 1;
  msg.buf[1] = 2;
  msg.buf[2] = 0;
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.len = 8;
  msg.id = 0x7DF;
  msg.flags.extended = 0; 
  msg.flags.remote = 0;
//  msg.timeout = 500;
  TX_timer.begin(tx_CAN, 1000000);    /* Start interrutp timer */
  
}


/* From Timer Interrupt */
void tx_CAN(void)
{
  msg.len = 8;
  msg.id = 0x7DF;

  for(int i = 0; i < 8; i++)
  {
    msg.buf[i] = rxmsg.buf[i];
  }

  
  Can0.write(msg);
  can_msg_count++;
}


void loop() {

  String CANStrTX;
  String CANStrRX;

  
  while(Can0.read(rxmsg)) 
  { 

     for (int i=0; i < 8; i++) //For-loop to store data package in string
     {     
         CANStrRX += String(rxmsg.buf[i],HEX);
         CANStrRX += ("") ;
     }

     for (int i=0; i < 8; i++) //For-loop to store data package in string
     {     
         CANStrTX += String(msg.buf[i],HEX);
         CANStrTX += ("") ;
     }
     

     //Serial Bus message
     Serial.print(rxmsg.id,HEX); 
     Serial.print(' '); 
     Serial.print(rxmsg.len,HEX); 
     Serial.print(' ');
     Serial.println(CANStrRX);  


     //Display RX info
     display.setCursor(0,0);
     display.print("RX CAN ID: ");
     display.println(rxmsg.id,HEX); 
     display.print("RX CAN Length: ");
     display.println(rxmsg.len,HEX); 
     display.print("RX Data:");
     display.println(CANStrRX);
     display.print("RX Count: ");
     display.println(count);

     //Display TX info
     display.print("TX CAN ID: ");
     display.println(msg.id, HEX);
     display.print("TX CAN Lenght: ");
     display.println(msg.len, HEX);
     display.print("TX Data: ");
     display.println(CANStrTX);
     display.print("TX Count: ");
     display.println(can_msg_count);
     display.display();
     display.clearDisplay();
     count++;
     
  }
}
