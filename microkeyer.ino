
/**
 * Microkeyer - Morse paddle keyer using Arduino Pro Micro
 * 
 * Matti Pöllä <mpo@iki.fi> 2022
 */


#include "Keyboard.h"


const int TONE_HZ = 600;
const int WPM_SPEED = 15;
const int CLEAR_MS = 2000; // Reset after 2 seconds

// Arduino Pro Micro internal led
const int LED = 17;
// Piezo buzzer
const int BUZZER = 14;

// Iambic paddle connected using a 3.5 mm stereo jack
const int paddleLeft = 6;
const int paddleRight = 7;


const int DIT_MS = int(1000.0 * 60 / (50 * WPM_SPEED));
const int DA_MS = 3 * DIT_MS;
const int LETTER_SPACE_MS = 3 * DIT_MS;
const int WORD_SPACE_MS = 7 * DIT_MS;
const int NEWLINE_DELAY_MS = 10000;
const int DEBOUNCE_DELAY = DIT_MS;


int paddleLeftState = 0;
int paddleRightState = 0;
unsigned long lastDebounceTime = 0;

String buffer = "";


char morseDecode(String s) {

    if (s == ".-") { return 'A'; }
    if (s == "-...") { return 'B'; }
    if (s == "-.-.") { return 'C'; }
    if (s == "-..") { return 'D'; }
    if (s == ".") { return 'E'; }
    if (s == "..-.") { return 'F'; }
    if (s == "--.") { return 'G'; }
    if (s == "....") { return 'H'; }
    if (s == "..") { return 'I'; }
    if (s == ".---") { return 'J'; }
    if (s == "-.-") { return 'K'; }
    if (s == ".-..") { return 'L'; }
    if (s == "--") { return 'M'; }
    if (s == "-.") { return 'N'; }
    if (s == "---") { return 'O'; }
    if (s == ".--.") { return 'P'; }
    if (s == "--.-") { return 'Q'; }
    if (s == ".-.") { return 'R'; }
    if (s == "...") { return 'S'; }
    if (s == "-") { return 'T'; }
    if (s == "..-") { return 'U'; }
    if (s == "...-") { return 'V'; }
    if (s == ".--") { return 'W'; }
    if (s == ".--.") { return 'X'; }
    if (s == "-.--") { return 'Y'; }
    if (s == "--..") { return 'Z'; }

    if (s == ".--.-") { return 'Å'; }
    if (s == "-.-.") { return 'Ä'; }
    if (s == "---.") { return 'Ö'; }

    if (s == ".----") { return '1'; }
    if (s == "..---") { return '2'; }
    if (s == "...--") { return '3'; }
    if (s == "....-") { return '4'; }
    if (s == ".....") { return '5'; }
    if (s == "-....") { return '6'; }
    if (s == "--...") { return '7'; }
    if (s == "---..") { return '8'; }
    if (s == "----.") { return '9'; }
    if (s == "-----") { return '0'; }

    if (s == "..--..") { return '?'; }
    if (s == ".-.-.-") { return '.'; }

    return 'x';
}

// the setup function runs once when you press reset or power the board
void setup() {
  
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  
  Keyboard.begin();
  pinMode(paddleLeft, INPUT);
  pinMode(paddleRight, INPUT);
  lastDebounceTime = millis();


}

int space = 0;
int newline = 1;

// the loop function runs over and over again forever
void loop() {

  if ((millis() - lastDebounceTime) > WORD_SPACE_MS && buffer == "" && space == 0) {
   buffer = "";
   //Serial.print(" ");
   Keyboard.print(" ");
   space = 1;
  }

  if ((millis() - lastDebounceTime) > NEWLINE_DELAY_MS && newline == 0) {
    Serial.println("");
    newline = 1;
  }


  if ((millis() - lastDebounceTime) > CLEAR_MS && buffer != "") {
    buffer = "";
    //Serial.println("");
    lastDebounceTime = millis();
  }

  paddleLeftState = digitalRead(paddleLeft);
  paddleRightState = digitalRead(paddleRight);
  
  if (((millis() - lastDebounceTime) > LETTER_SPACE_MS) && buffer.length() > 0) {
    //Serial.print(morseDecode(String(buffer)));
    Keyboard.print(morseDecode(String(buffer)));
    buffer = "";
    space = 0;
    newline = 0;
    lastDebounceTime = millis();
  }
  
  // Are we ready for a new input?
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // Left paddle
    if (paddleLeftState == LOW && paddleRightState == HIGH) {
      tone(BUZZER, TONE_HZ);
      digitalWrite(LED, HIGH);
      delay(DIT_MS);
      noTone(BUZZER);
      digitalWrite(LED, LOW); 
      buffer += ".";
      lastDebounceTime = millis();
    }
    // Right paddle
    if (paddleLeftState == HIGH && paddleRightState == LOW) {
      tone(BUZZER, TONE_HZ);
      digitalWrite(LED, HIGH);
      delay(DA_MS);
      noTone(BUZZER);
      digitalWrite(LED, LOW);
      buffer += "-";
      lastDebounceTime = millis();
    } 
  }
}
