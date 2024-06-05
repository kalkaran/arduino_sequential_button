#include <DFRobotDFPlayerMini.h>


/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>

 ***************************************************
 This example shows the basic function of library for DFPlayer.

 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

// format sd card on mac:
// 1. sudo diskutil list
// 2. sudo diskutil eraseDisk FAT32 MUSIC MBRFormat /dev/disk ??
// need to erase .Spotlight-V100 and .fseventsd
// 3. sudo mdutil -d "/Volumes/MUSIC"
// 4. sudo mdutil -X "/Volumes/MUSIC"
// 5. rm -rf /Volumes/MUSIC/.fseventsd
// 6. dot_clean -m /Volumes/MUSIC

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Button settings.
const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 13;    // the number of the LED pin

bool playstate = 0; // playstate
int songIndex = 0;  // Starting at index below the first song, gets incremented in the play function
int totalSongs = 0; 
int buttonState = 0;

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup()
{  
    // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  int buttonState = 0;  // variable for reading the pushbutton status
  bool playstate = 0; // playstate

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    flash(10);
    //while(true);
  }else{
    Serial.println(F("DFPlayer Mini online."));
    delay(2000);
//    myDFPlayer.volume(30);  //Set volume value. From 0 to 30
    //myDFPlayer.play(1);  //Play the first mp3
    //myDFPlayer.randomAll()

    delay(3000);
      int folderCount = myDFPlayer.readFolderCounts();
      Serial.print("Number of folders: ");
      Serial.println(folderCount);
      int folderNumber = 1; // Specify the folder number you want to check
      int fileCount = myDFPlayer.readFileCountsInFolder(folderNumber);
      Serial.print("Number of files in folder ");
      Serial.print(folderNumber);
      Serial.print(": ");
      Serial.println(fileCount);
      totalSongs = myDFPlayer.readFileCounts();  // Get the total number of songs
      Serial.print(F("Total songs: "));
      Serial.println(totalSongs);  // Debug statement


      flash(totalSongs);

  }
}

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

void play()
{
  //static unsigned long timer = millis();
  songIndex = songIndex + 1;
  if (songIndex > totalSongs) {
    songIndex = 1;  // Reset to the first song after playing all songs
  }
  
  Serial.print(F("Playing song number: "));
  Serial.println(songIndex);
  myDFPlayer.playFolder(01, songIndex);
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  // if (millis() - timer > 3000) {
  //   timer = millis();
  // }
}

void loop()
{
  // read the state of the pushbutton value:
  int currentButtonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (currentButtonState == HIGH && buttonState == LOW) {
      Serial.println("Button pressed");
      playstate = 1;
      // turn LED on:
      digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
  buttonState = currentButtonState;

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