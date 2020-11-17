
// Library for CAN-bus
#include <FlexCAN.h>

// Library for Adafruit display functions
#include <Adafruit_GFX.h>
// Library for OLED Display(set height to 64 in header file)
#include <Adafruit_SSD1306.h>

// Library for communicating with MPU6050
#include <Wire.h>

//Library for IMU sensor MPU6050
#include <MPU6050.h>
#include "MPU6050.h"

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

// Initiate IMU sensor and variables to store IMU data
MPU6050 imu;
int16_t ax, ay, az, gx, gy, gz;
// Initiate variable to store converted z-axis IMU data [m/s^2]
float az_gf = 0;

// Initiate CAN TX and RX messages
static CAN_message_t txmsg, rxmsg;
// Initiate CAN RX counter
volatile uint32_t count_RX = 0;
// Initiate CAN TX counter
volatile uint32_t count_TX = 0;
// Initiate string for storing data recieved from CAN message
String CANStrRX;
// Initiate variable used to start transmitting data
bool startTX = false;
// Define interval timer for CAN-bus
IntervalTimer TX_timer;

// Initiate variables used to to make datapackages that will be sent over CAN-bus
int myData;
int dataPackage[] = {0, 0, 0, 0, 0};



void setup() {

  // Initiate serial communication (For showing if MPU is online)
  Serial.begin(9600);

  //Set CAN-bus Speed
  Can1.begin(250000);
  //Start CAN transmitting timer
  TX_timer.begin(tx_CAN, 1000000);

  // Initiate wire in order to communicate with MPU6050 via I2C
  Wire.begin();
  // Check if MPU6050 is online
  if (!imu.begin(AFS_2G, GFS_250DPS)) {
    Serial.println("MPU6050 is online...");
  }
  else {
    Serial.println("Failed to init MPU6050");
    while (true)
      ;
  }

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
  //Only transmit data if message ID:0x22 is recieved
  if (startTX == true)
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
  else
  {
    // Do nothing
  }
}



void loop() {

  // Get raw values from IMU
  imu.getMotion6Counts(&ax, &ay, &az, &gx, &gy, &gz);

  // Store z-acceleration value in a variable for maikng CAN-bus datapackag
  myData = -az;

  //Convert raw value from IMU to m/s^2
  az_gf = az * (9.81 / 16384);

  // Checking if message is recieved and gathering data of CAN message
  while (Can1.read(rxmsg))
  {
    // Checking if message recieved has ID:0x21 in order to start transmitting
    if (rxmsg.id == 33)
    {
      startTX = true;
    }

    // Update RX counter
    count_RX++;
  }

  // Drawing rounded rectangle with lines:
  display.drawRoundRect(0, 0, 128, 64, 5, WHITE);
  display.drawLine(0, 15, 128, 15, WHITE);
  display.drawLine(5, 30, 90, 30, WHITE);

  // Write out info on the display:
  display.setTextSize(0);
  display.setCursor(15, 5);
  display.println("MAS234 - Gruppe 7");
  display.println(" ");
  display.println(" CAN-statistikk");
  display.println(" ");
  display.print(" Antall mottatt: ");
  display.println(count_RX);
  display.print(" Mottok sist ID: ");
  display.println(rxmsg.id);
  display.print(" IMU, z: ");
  display.print(-az_gf);
  display.println(" m/s^2");
  display.display();
  display.clearDisplay();
}
