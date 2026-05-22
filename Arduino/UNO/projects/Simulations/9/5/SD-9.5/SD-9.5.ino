#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

TMRpcm audio;

void setup() {  
  if (!SD.begin(10)) {
    while(true);
  }
  
  audio.speakerPin = 9;
  audio.setVolume(3);
  audio.quality(1);
    
  if (SD.exists("1.wav")) {
    audio.play("1.wav");
  } 
}

void loop() {
}