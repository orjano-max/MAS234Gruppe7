
// Library for CAN-bus
#include <FlexCAN.h>

//tissetassen

// Library for Adafruit display functions
#include <Adafruit_GFX.h>
// Library for OLED Display(set height to 64 in header file)
#include <Adafruit_SSD1306.h>

// Defining DC pin for OLED display
#define OLED_DC     6
// Defining CS pin for OLED display
#define OLED_CS     10
// Defining RESET pin for oled display
#define OLED_RESET  5
// Set pins OLED display
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

// Error if displayheight is wrong
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Initiate CAN TX and RX messages
static CAN_message_t txmsg, rxmsg;
// Initiate CAN RX counter
volatile uint32_t count_RX = 0;
// Initiate CAN TX counter
volatile uint32_t count_TX = 0;
// Initiate variable used to start transmitting data
bool startTX = false;
// Define interval timer for CAN-bus
IntervalTimer TX_timer;

// Initiate variables used to to make datapackages that will be sent over CAN-bus
int myData;
int dataPackage[] = {0, 0, 0, 0, 0};

void setup() {

  //Set CAN-bus Speed
  Can1.begin(250000);
  //Start CAN transmitting timer
  TX_timer.begin(tx_CAN, 1000000);

  // Initiate display
  display.begin(SSD1306_SWITCHCAPVCC);
  // Clear display of old graphics
  display.clearDisplay();

  // For-loop to display fake loading Screen
  for (int i = 0; i <= 100; i = i + 9)
  {
    display.setTextSize(0);
    display.setTextColor(WHITE);
    display.setCursor(0, 5);
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

  // Write out info on the display:
  display.setTextSize(0);
  display.setCursor(20, 30);
  display.println("Processing...");
  display.display();
  display.clearDisplay();
}

//Transmit data at rate determined by TX_timer
void tx_CAN(void)
{
  
    //Set length of datapackage
  txmsg.len = 8;
  //Set id of CAN message
  txmsg.id = 0x22;

  // Converting rawdata from IMU to an array for sending IMU data over CAN-bus
  for (int i = 0; i < 6; i++)
  {
    if (myData / pow(256, 5 - i) >= 1) {
      int x = floor(myData / pow(256, 5 - i));
      dataPackage[i] = x;
      myData = myData - x * pow(256, 5 - i);
    }
    else
    {
      //Do nothing
    }
  }

  // Making CAN-bus datapackage containing raw data from IMU z-axis
  for (int i = 0; i < 6; i++)
  {
    txmsg.buf[i] = dataPackage[i];
  }

  // Last two places in data package unused
  txmsg.buf[6] = 0;
  txmsg.buf[7] = 0;

  // Sending CAN message
  Can1.write(txmsg);
  // Counting amount of messages sent
  count_TX++;

}


//Main loop:
void loop() {

// Checking if message is recieved and gathering data of CAN message
  while (Can1.read(rxmsg))
  { 
    // Update RX counter
    count_RX++;
  }

  /*for(i = 0; i < 6; i++)
  {
    
  }*/
  
  

}
