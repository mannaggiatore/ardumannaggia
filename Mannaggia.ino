// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)  // for Zero, output on USB Serial console
#define Serial SerialUSB
#endif

RTC_DS1307 rtc;

// include the SD library:
#include <SPI.h>
#include <SD.h>

// change this to match your SD shield or module;
const int chipSelect = 10;
//File root;

// include the PCM library:
#include <TMRpcm.h>
TMRpcm tmrpcm;   // create an object for use in this sketch


void setup () {
  while (!Serial);  // for Leonardo/Micro/Zero

  Serial.begin(57600);
  Serial.println("**************");
  Serial.println("ARDUMANNAGGIA!");
  Serial.println("**************");


  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  //Set current time. Uncomment and run once
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  else
  {
    Serial.println("RTC is running!");
  }

  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  tmrpcm.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc

}

void loop () {

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(")");
  Serial.println();

  String pa = "gtts";
  pa = pa + "/" + format(now.month(), 2) + "/" + format(now.day(), 2) + "/" ;

  //pa = "gtts/01/01/";
  //Serial.println(pa);

  playDirectory(pa);

  Serial.println();
  delay(3000);
}

String format(long nu, int le) {
  String s = "0000000000" + String(nu);
  return s.substring(s.length() - le);
}

void playDirectory(String pat) {
  SD.begin(chipSelect);
  File dir = SD.open(pat);
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      dir.rewindDirectory();
      entry.close();
      break;
    }
    //Serial.println(pat);
    //Serial.println(entry.name());

    String mystring = pat + String(entry.name());
    Serial.println(mystring);
    char otherstring[24]; //char array to copy the String into
    mystring.toCharArray(otherstring, mystring.length() + 1); //Copy the string (+1 is to hold the terminating null char)

    tmrpcm.play(otherstring);
    while (tmrpcm.isPlaying()) {
    }
    entry.close();
  }

  dir.close();
}

