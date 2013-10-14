/*little_simon
 ============
 
 "little_simon" was derived from "Arduino Simon" by Chris Swan and
 used under a Creative Commons Attribution-ShareAlike 3.0
 Unported License: http://creativecommons.org/licenses/by-sa/3.0/.
 "little_simon" is licensed under the same Creative Commons
 Attribution-ShareAlike 3.0 Unported License: http://creativecommons.org/licenses/by-sa/3.0/
 
 The original code for "Arduino Simon" is available at the Fritzing
 website: http://fritzing.org/projects/arduino-simon. You can also 
 find it at GitHub: https://github.com/cpswan/Arduino
 
 When compiling and uploading "little_simon" to the target ATtiny44,
 be sure to use the MIT-HLT board definitions. Under tools select
 'ATtiny44 (internal 8 MHz clock) then burn bootloader to the 
 target. See the MIT High-Low tech website at http://hlt.media.mit.edu/?p=1695
 for more information on how to use Arduino IDE for programming
 AVR microcontrollers.
 */

const int led_red = 1;         // Output pins for the LEDs
const int led_blue = 2;
const int led_yellow = 3;
const int led_green = 4;
const int buzzer = 5;		 // Output pin for the buzzer
const int random_seed = 0;        // Unconnected pin used for random seed generation
const int red_button = 9;      // Input pins for the buttons
const int blue_button = 10;
const int yellow_button = 6; 
const int green_button = 7;   // Pin 13 is special - didn't work as planned
const int unused = 8;
long sequence[20];             // Array to hold sequence
int count = 0;                 // Sequence counter
long input = 5;                // Button indicator
int wait = 500;                // Variable delay as sequence gets longer
int test_count = 0;            // Counter for power-on test function
const int test_num = 3;              // Number of times power-on test is repeated  
const int test_time = 100;           // Length of time for each step of test sequence in millis
/*
  playtone function taken from Oomlout sample
 takes a tone variable that is half the period of desired frequency
 and a duration in milliseconds
 */
void playtone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone *2) {
    digitalWrite(buzzer, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buzzer, LOW);
    delayMicroseconds(tone);
  }
}


/*
  functions to flash LEDs and play corresponding tones
 very simple - turn LED on, play tone for .5s, turn LED off
 */
void flash_red() {
  digitalWrite(led_red, HIGH);
  playtone(2273,wait);             // low A
  digitalWrite(led_red, LOW);
}

void flash_blue() {
  digitalWrite(led_blue, HIGH);
  playtone(1700,wait);             // D
  digitalWrite(led_blue, LOW);
}

void flash_yellow() {
  digitalWrite(led_yellow, HIGH);
  playtone(1275,wait);             // G
  digitalWrite(led_yellow, LOW);
} 

void flash_green() {
  digitalWrite(led_green, HIGH);
  playtone(1136,wait);             // high A
  digitalWrite(led_green, LOW);
}

// a simple power-on test function to flash all of the LEDs in turn
void runtest() {
  for(test_count = 0; test_count < test_num; test_count++) {
    digitalWrite(led_red, HIGH);      // Turn on LED
    playtone(2273,test_time);        // Play corresponding tone for specified test_time
    digitalWrite(led_red, LOW);      // Turn of LED
    digitalWrite(led_blue, HIGH);      // Turn on LED
    playtone(1700,test_time);        // Play corresponding tone for specified test_time
    digitalWrite(led_blue, LOW);      // Turn of LED
    digitalWrite(led_yellow, HIGH);      // Turn on LED
    playtone(1275,test_time);        // Play corresponding tone for specified test_time
    digitalWrite(led_yellow, LOW);      // Turn of LED
    digitalWrite(led_green, HIGH);      // Turn on LED
    playtone(1136,test_time);        // Play corresponding tone for specified test_time
    digitalWrite(led_green, LOW);      // Turn of LED
  }
  // Test Pattern from Chris Swan's original code
  //    flash_red();
  //    flash_green();
  //    flash_yellow();
  //    flash_blue();  
}

/* a function to flash the LED corresponding to what is held
 in the sequence
 */
void squark(long led) {
  switch (led) {
  case 0:
    flash_red();
    break;
  case 1:
    flash_green();
    break;
  case 2:
    flash_yellow();
    break;
  case 3:
    flash_blue();
    break;
  }
  delay(50);
}

// function to congratulate winning sequence
void congratulate() {
  digitalWrite(led_red, HIGH);       // turn all LEDs on
  digitalWrite(led_green, HIGH);
  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_blue, HIGH);
  playtone(1014,250);                // play a jingle
  delay(25);    
  playtone(1014,250);
  delay(25);    
  playtone(1014,250);
  delay(25);    
  playtone(956,500);
  delay(25);    
  playtone(1014,250);
  delay(25);
  playtone(956,500);
  delay(2000);   
  digitalWrite(led_red, LOW);        // turn all LEDs off
  digitalWrite(led_green, LOW);
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_blue, LOW);
  resetCount();                      // reset sequence
}

// function to reset after winning or losing
void resetCount() {
  count = 0;
  wait = 500;
}

// function to build and play the sequence
void playSequence() {
  sequence[count] = random(4);       // add a new value to sequence
  for (int i = 0; i < count; i++) {  // loop for sequence length
    squark(sequence[i]);             // flash/beep
  }
  wait = 500 - (count * 15);         // vary delay
  count++;                           // increment sequence length
}

// function to read sequence from player 
void readSequence() {
  for (int i=1; i < count; i++) {               // loop for sequence length
    while (input==5){                          // wait until button pressed
      if (digitalRead(red_button) == LOW) {    // Red button
        input = 0;
      }
      if (digitalRead(green_button) == LOW) {  // Green button
        input = 1;
      }
      if (digitalRead(yellow_button) == LOW) { // Yellow button
        input = 2;
      }
      if (digitalRead(blue_button) == LOW) {   // Blue button
        input = 3;
      }
    }
    if (sequence[i-1] == input) {              // was it the right button?
      squark(input);                           // flash/buzz
      if (i == 20) {                           // check for correct sequence of 20
        congratulate();                        // congratulate the winner
      }
    }
    else {
      playtone(4545,1000);                   // low tone for fail
      squark(sequence[i-1]);                 // double flash for the right colour
      squark(sequence[i-1]);
      resetCount();                          // reset sequence
    } 
    input = 5;                                   // reset input
  }
}  

// standard sketch setup function
void setup() {
  pinMode(unused, INPUT_PULLUP);      // pin ''unused' is not used, so set to high
  pinMode(led_red, OUTPUT);      // configure LEDs and buzzer on outputs
  pinMode(led_green, OUTPUT); 
  pinMode(led_yellow, OUTPUT); 
  pinMode(led_blue, OUTPUT); 
  pinMode(buzzer, OUTPUT);
  pinMode(red_button, INPUT_PULLUP);    // configure buttons on input
  pinMode(green_button, INPUT_PULLUP);
  pinMode(yellow_button, INPUT_PULLUP);
  pinMode(blue_button, INPUT_PULLUP);
  randomSeed(analogRead(random_seed));     // random seed for sequence generation
  runtest();
}

// standard sketch loop function
void loop() {
  playSequence();  // play the sequence
  readSequence();  // read the sequence
  delay(1000);     // wait a sec
}


