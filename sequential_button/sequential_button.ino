#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
<https://www.dfrobot.com/index.php?route=product/product&product_id=1121>

 ***************************************************
 This example demonstrates the basic functions of the library for DFPlayer.

 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/*********** Notice and Troubleshooting ***************
 1. Connection and Diagram can be found here:
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2. This code is tested on Arduino Uno, Leonardo, and Mega boards.
 ****************************************************/

//// Rules for placing files and folders on SD Card when using DFPlayer Mini
//    In folders numbered 01 – 99. File names with 3 digit prefixes 001 – 255. Command 0x0F
//    In a folder named MP3 (or perhaps mp3). File names with 4 digit prefixes 0001 to 3000. Command 0x12
//    In the root directory. File names with 4 digit prefixes 0001 to 3000. Command 0x03
//    In folders named 01 – 15. File names with 4 digit prefixes 0001 – 3000. Command 0x14

// Global Variables
const int buttonPin = 2;  // Pin number for the pushbutton
const int ledPin = 13;    // Pin number for the LED
bool playstate = 0;       // Boolean to track play state
int songIndex = 0;        // Index for the current song, starts at 0 - gets incremented to 1 on first play, as 1 is the first song.
int totalSongs = 0;       // Total number of songs on the SD card
int buttonState = 0;      // Current state of the button

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX for SoftwareSerial communication with MP3
DFRobotDFPlayerMini myDFPlayer;         // Instance of the DFPlayer Mini library

void printDetail(uint8_t type, int value); // Function declaration for printing error details

void setup() {
  // Initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // Initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // Start serial communications for debugging
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  // Initialize DFPlayer and check for errors
  if (!myDFPlayer.begin(mySoftwareSerial)) {  

    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    flash(2);

  }else{
    delay(5000);
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(20);  //Set volume value. (0 to 30)
      
    // Retrieve and print the number of folders - N.B. doesn't work 
    // int folderCount = myDFPlayer.readFolderCounts();
    // Serial.print("Number of folders: ");
    // Serial.println(folderCount);

    // Specify the folder number and retrieve the number of files in that folder
    int folderNumber = 1; // Change this to the folder number you want to check
    int fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);
    Serial.print("Number of files in folder ");
    Serial.print(folderNumber);
    Serial.print(": ");
    Serial.println(fileCount);

    // Retrieve and print the total number of songs
    totalSongs = myDFPlayer.readFileCounts();
    Serial.print(F("Total songs: "));
    Serial.println(totalSongs);
    // LED flashes song count
    flash(totalSongs);
  }
}

// Function to flash the LED a specified number of times
void flash(int count)
{
    int cycle = 0;
    while( cycle < count ){
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    cycle = cycle + 1; 
    }
}

// Function to play the next song in the sequence
void play()
{
  songIndex = songIndex + 1;
  if (songIndex > totalSongs) {
    songIndex = 1;  // Reset to the first song after playing all songs
  }
  
  Serial.print(F("Playing song number: "));
  Serial.println(songIndex);
  myDFPlayer.playFolder(01, songIndex);
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the errors and states.
  }
}

void loop()
{
  /// Read the current state of the pushbutton
  int currentButtonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. 
  if (currentButtonState == HIGH && buttonState == LOW) {
      Serial.println("Button pressed");
      // Setting play state to 1 to enable play function
      playstate = 1;
      // turn LED on
      digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off
    digitalWrite(ledPin, LOW);
  }
  buttonState = currentButtonState;

  // If playstate is set, play the next song
  if (playstate == 1){
    playstate = 0;
    Serial.println("playstate: 1");
    play();
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}