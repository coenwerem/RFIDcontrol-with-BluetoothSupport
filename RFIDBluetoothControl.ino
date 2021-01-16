/*
Written by Clinton Enwerem. RFID and Bluetooth control of AC/DC devices using a 5V SPDT relay rated:
Maximum AC load current: 10A @ 250/125V AC
Maximum DC load current: 10A @ 30/28V DC
*/

#include <MFRC522.h>  // MFRC522 RFID module library.
#include <SPI.h>      // SPI device communication library.

#define rstPin 9      // Defines pins for RST and SDA connections respectively.
#define sdaPin 10

byte cardUID[] = {50,127,37,27}; //This is the allowed card UID
int relayPin = 2;
int buzzerPin = 4;
int state = 0; // State of the relayPin
long int password1 = 92;// light on signal from Bluetooth Android app
long int password2 = 79; // light off signal from Bluetooth Android app
long int blueComm;

MFRC522 rfid(sdaPin, rstPin);   // Creates an instance of the MFRC522 library.

void setup()
{
  Serial.begin(9600); // Starts the serial connection at 9600 baud rate.
  SPI.begin();        // Initiates SPI connection between RFID module and Arduino.
  rfid.PCD_Init(); // Initiates MFRC522 RFID module.
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); //To make sure the relay is OFF at first.
}

void loop()
{
   if(rfid.PICC_IsNewCardPresent())// If a new card is present,
    {
     readCard(); // Function to read card, match the card to the pre-defined card, and turn ON the relay or buzzer depending on whether the card matches or not.
    }
    
    else if(Serial.available()> 0)
    {
    blueComm = Serial.parseInt();// Reads the data from the serial port, changes it to char, and appends it to a string.
    callBlue(); // Function that activates the Bluetooth communication and checks for existing commands to turn the relay ON or OFF.
    }
}

void readCard()
{
  rfid.PICC_ReadCardSerial();//  Read the card.
  
  // Checking if cards match 
  int i = 0; // Initialize a counter to loop through the array of the card UID
  boolean match = false; // Boolean to check whether each index of the array matches the cardUID
  while(i < rfid.uid.size) // While loop of cardUID sizem(4)
  {
    if(rfid.uid.uidByte[i] == cardUID[i])
    {
      match = true;
    }
    i++; //Increment the while loop
  }
  
  if(match == true) //If the indices match,...
  {
    digitalWrite(relayPin, state); //Switch on the lamp (load) by turning on the relay.
    state = !state;
    delay(1000);
  }
        
  else //If the array indices don't match,...
  {
    digitalWrite(buzzerPin, HIGH);//  Blare the buzzer 
    delay(2000);                   //for 2 seconds
    digitalWrite(buzzerPin, LOW); // and turn OFF the buzzer.
    //Serial.write("Someone tried to turn on your lamp with a wrong card!");// Displays on the Bluetooth app on the user's phone.
  }
  rfid.PICC_HaltA();     // Stops the reading process.   
}


//Bluetooth Function
void callBlue()
{
  if (blueComm == password1) 
     {
      digitalWrite(relayPin, LOW); // Turn Relay ON
      state = 1;
      Serial.write("LED is on");
     }
  else if (blueComm == password2) 
     {
      digitalWrite(relayPin, HIGH);
      state = 0;
      Serial.write("LED is off");
     }  
}
