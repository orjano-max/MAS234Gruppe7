
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
// Initiate strings for storing data from CAN TX and RX messages
String CANStrRX;
// Initiate variables used to to make datapackages that will be sent over CAN-bus
int myData = 0;
int dataPackage[] = {0, 0, 0, 0, 0, 0};
// Initiate variables to convert recieved data package to one variable
int B_0 = 0; 
int B_1 = 0; 
int B_2 = 0; 
int B_3 = 0; 
int B_4 = 0; 
int B_5 = 0; 

// Initiate variable to store converted data recieved
float imuConverted = 0;


void setup() {

  // Initiate serial communication (For showing if MPU is online)
  Serial.begin(9600);

  //Set CAN-bus Speed
  Can1.begin(250000);
  //Start CAN transmitting timer
  TX_timer.begin(tx_CAN, 100000);

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

    // If acceleration is negative, first slot in datapackage is 1
    if(myData < 0)
    {
      txmsg.buf[0] = 1;
      myData = -myData;
    }
    else
    {
      txmsg.buf[0] = 0;
    }

    // Converting rawdata from IMU to an array for sending IMU data over CAN-bus
    for (int i = 1; i < 6; i++)
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
    for (int i = 1; i < 6; i++)
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
    Serial.println("Sending pew pew pew");
  }
  else
  {
    // Do nothing
  }
}



void loop() {

  // Get raw values from IMU
  imu.getMotion6Counts(&ax, &ay, &az, &gx, &gy, &gz);

  // Store z-acceleration value in a variable for making CAN-bus datapackag
  myData = az;

  // Checking if message is recieved and gathering data of CAN message
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

    // Checing if mesage recieved has ID: 0x21h (IMU-data message ID)
    if(rxmsg.id == 33)
    { 

    B_0 = rxmsg.buf[0];
    B_1 = rxmsg.buf[1] * pow(256, 4) ;
    B_2 = rxmsg.buf[2] * pow(256, 3) ;
    B_3 = rxmsg.buf[3] * pow(256, 2) ;
    B_4 = rxmsg.buf[4] * pow(256, 1) ;
    B_5 = rxmsg.buf[5] * pow(256, 0) ;

    imuConverted = ( B_1 + B_2 + B_3 + B_4 + B_5 );
   
    // imu data is transmitted as integer [(m/(s^2))*1000]
    // dividing by 1000 to get value as [m/(s^2)] for displaying on screen
    imuConverted = imuConverted/1000;

    // Value is negative if first slot in data package is 1
    if(B_0 == 1)
    {
      imuConverted = -imuConverted;
    }
    else
    {
      // Do nothing
    }
    }

    // Checking if message recieved has ID:0x23h in order to start transmitting
    if (rxmsg.id == 35)
    {
      startTX = true;
    }

    // Checking if message recieved has ID: 0x24h in order to stop transmitting
    if (rxmsg.id == 36)
    {
      startTX = false;
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
  display.print(" RX Data:");
  display.println(CANStrRX);
  display.print(" IMU, z: ");
  display.print(imuConverted);
  display.println(" m/s^2");
  display.display();
  display.clearDisplay();
}
