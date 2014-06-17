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

// set ASK_SILENT to 1 to let the user press the green button to turn off sound
#define ASK_SILENT 0

// set ASK_LEVEL to 1 to let the user choose the initial level by pressing a
// button during the initial "roulette" display, which is shown faster to
// tell that this feature is enabled:
// RED: 8
// GREEN: 14
// YELLOW: 20
// BLUE: 31
#define ASK_LEVEL 0

// set TEST_LEVELS to 1 to test changes to level logic
#define TEST_LEVELS 0


const int led_red    = 1;         // Output pins for the LEDs
const int led_blue   = 2;
const int led_yellow = 3;
const int led_green  = 4;
const int buzzer = 5;		 // Output pin for the buzzer
const int random_seed = 0;       // Unconnected pin used for random seed generation
const int red_button    = 7;     // Input pins for the buttons
const int blue_button   = 8;
const int yellow_button = 9;
const int green_button  = 10;
const int unused = 8;
// logic constants
const int RED = 1;             // Program state associated with the colors
const int BLUE = 2;            // These stay the same even if the led_* pins change
const int YELLOW = 3;
const int GREEN = 4;
const int LEV1 = 8;            // Levels chosen from original simon game
const int LEV2 = 14;           // 8, 14, 20, and 31 were the original game lengths
const int LEV3 = 20;           // http://en.wikipedia.org/wiki/Simon_%28game%29
const int LEV4 = 31;

const int test_num = 3;        // Number of times power-on test is repeated

long sequence[LEV4];           // Array to hold sequence
int count = 0;                 // Sequence counter
int wait = 500;                // Variable delay as sequence gets longer
int silent = 0;                // Turned on if user chooses silent mode
int level = LEV1;

/*
  playtone function taken from Oomlout sample
 takes a tone variable that is half the period of desired frequency
 and a duration in milliseconds
 */
void playtone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone *2) {
    if (!silent) {
      digitalWrite(buzzer, HIGH);
    }
    delayMicroseconds(tone);
    if (!silent) {
      digitalWrite(buzzer, LOW);
    }
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

int readButton() {
  if (digitalRead(red_button) == LOW) {    // Red button
    return RED;
  }
  if (digitalRead(green_button) == LOW) {  // Green button
    return GREEN;
  }
  if (digitalRead(yellow_button) == LOW) { // Yellow button
    return YELLOW;
  }
  if (digitalRead(blue_button) == LOW) {   // Blue button
    return BLUE;
  }
  return 0; // No button pressed
}

// function to reset after winning or losing
void resetCount() {
  count = 0;
  wait = 500;
}

void advanceLevel() {
  switch (level) {
    case LEV1:
      level = LEV2;
      break;
    case LEV2:
      level = LEV3;
      break;
    case LEV3:
      level = LEV4;
      break;
    case LEV4:
      // have mercy
      level = LEV1;
      break;
  }
}


#if ASK_SILENT
// choose silent mode if user asks by pressing green button while powering ontest
void silence() {
  silent = 1;
  wait = 5;
  long button = 0;
  for (int i=1; i < 200; i++) { // one second
    flash_green();
    button = readButton();
    if (button == GREEN) { // play silently
      break;
    }
    if (button) { // any other button to play loud
      silent = 0;
      break;
    }
  }
  if (!button) {
    silent = 0;
  }
  resetCount();
}
#endif


#if ASK_LEVEL
void chooseLevel() {
  wait = 50;
  long button = 0;

  delay(500); // debounce for silent mode choice
  for (int i=1; i < 30; i++) { // two seconds
    switch (i % 4) {
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
        delay(50);
        break;
    }

    button = readButton();
    switch (button) {
      case RED:
        level = LEV1;
        break;
      case GREEN:
        level = LEV2;
        break;
      case YELLOW:
        level = LEV3;
        break;
      case BLUE:
        level = LEV4;
        break;
    }
    if (button) {
      break;
    }
  }
  resetCount();
}

#else

// a simple power-on test function to flash all of the LEDs in turn
// without offering the user the chance to choose a level
void runtest() {
  int test_count = 0;

  wait = 100; // go fast at boot
  for(test_count = 0; test_count < test_num; test_count++) {
    flash_red();
    flash_green();
    flash_yellow();
    flash_blue();
  }
  resetCount();
}
#endif

/* a function to flash the LED corresponding to what is held
 in the sequence
 */
void squark(long led) {
  switch (led) {
  case RED:
    flash_red();
    break;
  case GREEN:
    flash_green();
    break;
  case YELLOW:
    flash_yellow();
    break;
  case BLUE:
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
  advanceLevel();
}

long getNext() {
#if TEST_LEVELS
  return RED; // make testing through all levels easier
#else
  return (random(4) + 1);
#endif
}

// function to build and play the sequence
void playSequence() {
  sequence[count] = getNext();       // add a new R/G/Y/B value to sequence
  for (int i = 0; i < count; i++) {  // loop for sequence length
    squark(sequence[i]);             // flash/beep
  }
  wait = 500 - (count * 15);         // vary delay
  count++;                           // increment sequence length
}

// function to read sequence from player 
void readSequence() {
  long input = 0;
  for (int i=1; i < count; i++) {               // loop for sequence length
    do {                                       // wait until button pressed
      input = readButton();
    } while (!input);
    if (sequence[i-1] == input) {              // was it the right button?
      squark(input);                           // flash/buzz
      if (i == level) {                        // check for correct sequence length
        congratulate();                        // congratulate the winner
      }
    }
    else {
      playtone(4545,1000);                   // low tone for fail
      squark(sequence[i-1]);                 // double flash for the right colour
      squark(sequence[i-1]);
      resetCount();                          // reset sequence
    } 
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
#if ASK_SILENT
  silence(); // before noisy runtest in case use wants silent
#endif
  // need only one of runtest or chooseLevel
#if ASK_LEVEL
  chooseLevel();
#else
  runtest();
#endif
}

// standard sketch loop function
void loop() {
  playSequence();  // play the sequence
  readSequence();  // read the sequence
  delay(1000);     // wait a sec
}


