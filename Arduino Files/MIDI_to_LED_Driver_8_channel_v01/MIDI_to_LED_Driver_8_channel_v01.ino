// 
// Create a Fade effect, which fades out an LED connected to an TLC5940 (16 Channel LED PWM Driver), connected via SPI to an arduino. 
// The Arduino takes MIDI Notes and triggers an LED which fades out when a NoteOn comes in.
// Usefull for creating customized MIDI Triggered Light effects 
// 
// /~~\ /~~\ |\  |~|~/~~  |~~\ /~~\ |~~\ /~~\~~|~~/~~\
// `--.|    || \ | ||     |__/|    ||--<|    | |  `--.
// \__/ \__/ |  \|_|_\__  |  \ \__/ |__/ \__/  |  \__/
//
// Contributors: Moritz Simon Geist, LennartS
// 05 Mai 2017
// V.012
//
// === Recources: ===
// 1. Create and edit logarithmic Value lists: https://docs.google.com/spreadsheets/d/1KdJeNoF1NuLTvQSDkJnCvIzLNUSvrxJJv9CtxnRS0rM/edit#gid=0
// 2. MIDI Library https://github.com/FortySevenEffects/arduino_midi_library (used: c4.2.)
// 3. TLC5940 Library https://code.google.com/p/tlc5940arduino/


#include <Tlc5940.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE()

//// PARAMETERS ////

// Fading curve --> the perception of the human eye doenst take a linear bightness drop as linear --> logarithmic brightness drop is more natural
static const uint8_t array[] PROGMEM = {
0,255	,185	,160	,140	,98	,90	,68	,60	,53	,44	,40	,35	,29	,24	,22	,20	,18	,16	,14	,12	,10	,9	,7	,6	,5	,4	,3	,2	,2	,1	,0	,0	,	};

// How fast curve is walked through (= transistion time between steps)
const uint8_t decayTime = 12;

// Numbers of channels you want to control
const uint8_t numbChannels = 16;

//// FUNCTIONS TO CALL IN SKETCH ////

// Set channel to first value in array and start fade out
void startFade(uint8_t channel);

// Test if fading works on all channels
void startAllFades();

// Set all channels to last value of array and therefore stop fade out
void resetStates();

// Sets all active (=not already faded out) channels to the next value in array
void step();


////////// Can't touch this ! 

// const uint8_t numbSteps = sizeof(array)/sizeof(array[0]); OLD 
// Here we initilazie how far in the array the fading goes --> we can set a maximum to set a minimum brightness (ambient light)

uint8_t maxSteps= sizeof(array)/sizeof(array[0]);
uint8_t numbSteps =maxSteps; // intitialize with maximum stepsize
uint8_t fadeStates[numbChannels];

//////////




////////////////// CUSTOM FUNCTIONS /////////////////////////////
void startFade(uint8_t channel) {
  fadeStates[channel] = 0;
}

void startAllFades() {
	for (uint8_t index=0; index < numbChannels; index++)  {
		fadeStates[index] = 0;
	}
}

void resetStates() {
	for (uint8_t index=0; index < numbChannels; index++)  {
		fadeStates[index] = numbSteps-1;
	}
}

void step() {
   
   for (uint8_t channel=0; channel<numbChannels; channel++) {
    if (fadeStates[channel] < numbSteps-1) fadeStates[channel]++;
    Tlc.set(channel,pgm_read_byte(&array[fadeStates[channel]])*16);
   }
  
  Tlc.update();
}


// EDIT this section to map the incoming MIDI-Notes to the appropriate TLC5940 Channel

void HandleNoteOn(byte channel, byte note, byte velocity)
{          
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  
  switch (note) {
    case 36:       // Midi Note 2 goes to ..
      startFade(1);     // 
      
      break;
    
    case 37: 			// Midi Note goes to ..
      startFade(2); 		// 
      
      break;

    case 38:       // Midi Note goes to ..
      startFade(3);     // 
      
    break;
    
    case 40: 			// Midi Note XX goes to ..
      startFade(4); 		// .. TLC Channel  BD Real
      
      break;
      
    case 41: 			// Midi Note XX goes to ..
      startFade(5);     // .. TLC Channel  BD Real
      
      break;
      
    case 42: 			// Midi Note XX goes to ..
      startFade(6); 		// .. TLC Channel 3 FP 2
      
      break;

    case 43: 			// Midi Note XX goes to ..
      startFade(7);     // .. TLC Channel 3 FP 2
      
      break;

    case 44: 			// Midi Note XX goes to ..
      startFade(8); 		// .. TLC Channel .. 
      break;
  
  }
}


void HandleNoteOff(byte channel, byte note, byte velocity)
{
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
}


void setup() {
  pinMode(13, OUTPUT);
        MIDI.setHandleNoteOn(HandleNoteOn); // 
        MIDI.setHandleNoteOff(HandleNoteOff);
  Tlc.init();
  resetStates();
  MIDI.begin();
  startAllFades();

}

void loop() {
  step();
  unsigned long now=millis();
  while (millis()<(now+decayTime)) MIDI.read();
}
