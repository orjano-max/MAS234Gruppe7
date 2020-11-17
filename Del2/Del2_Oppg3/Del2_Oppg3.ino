
// Library for CAN-bus
#include <FlexCAN.h>

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
// Initiate string for storing data from CAN TX and RX messages
String CANStrRX;
// Define interval timer for CAN-bus
IntervalTimer TX_timer;

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
}

//Transmit data at rate determined by TX_timer
void tx_CAN(void)
{
  //Set length of datapackage
  txmsg.len = 8;
  //Set id of CAN message
  txmsg.id = 0x22;

  // For loop to store recieved data in sending data package
  for (int i = 0; i < 8; i++)
  {
    txmsg.buf[i] = rxmsg.buf[i];
  }

  // Sending CAN message
  Can1.write(txmsg);
  // Counting amount of messages sent
  count_TX++;
}


void loop() {


  while (Can1.read(rxmsg))
  {
    // Restetting string as nothing in order to store new data in string
    CANStrRX = "";
    
    // For-loop to store RX data package in string, for displaying data
    for (int i = 0; i < 8; i++)
    {
      CANStrRX += String(rxmsg.buf[i], HEX);
      CANStrRX += ("") ;
    }
    
    // Update RX counter
    count_RX++;
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
  display.setCursor(15, 5);
  display.println("MAS234 - Gruppe 7");
  display.println(" ");
  display.println(" CAN-statistikk");
  display.println(" ");
  display.print(" Antall mottatt: ");
  display.println(count_RX);
  display.print(" RX data:");
  display.println(CANStrRX);
  display.print(" Mottok sist ID: ");
  display.println(rxmsg.id);

}
