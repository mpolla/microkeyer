
/**
 * Microkeyer - Use your two paddle Morse key as a USB input device
 * 
 * https://github.com/mpolla/microkeyer/
 * 
 * Matti Pöllä <mpo@iki.fi> 2022
 */

#include "Keyboard.h"

// Beep frequency
const int TONE_HZ = 600;

// Words per minute speed
const int WPM_SPEED = 20;

// Reset input buffer after 2 seconds of inactivity
const int CLEAR_MS = 2000;

// Pin for piezo buzzer
const int BUZZER = 14;

// Iambic paddle connected using a 3.5 mm stereo jack
const int paddleLeft = 6;
const int paddleRight = 7;

const int DIT_MS = int(1000.0 * 60 / (50 * WPM_SPEED));
const int DA_MS = 3 * DIT_MS;
const int LETTER_SPACE_MS = 3 * DIT_MS;
const int WORD_SPACE_MS = 7 * DIT_MS;
const int DEBOUNCE_DELAY = DIT_MS;


int paddleLeftState = 0;
int paddleRightState = 0;
unsigned long lastInputMillis = 0;

String buffer = "";


String morseDecode(String s) {

  if (s == ".-") { return "A"; }
  if (s == "-...") { return "B"; }
  if (s == "-.-.") { return "C"; }
  if (s == "-..") { return "D"; }
  if (s == ".") { return "E"; }
  if (s == "..-.") { return "F"; }
  if (s == "--.") { return "G"; }
  if (s == "....") { return "H"; }
  if (s == "..") { return "I"; }
  if (s == ".---") { return "J"; }
  if (s == "-.-") { return "K"; }
  if (s == ".-..") { return "L"; }
  if (s == "--") { return "M"; }
  if (s == "-.") { return "N"; }
  if (s == "---") { return "O"; }
  if (s == ".--.") { return "P"; }
  if (s == "--.-") { return "Q"; }
  if (s == ".-.") { return "R"; }
  if (s == "...") { return "S"; }
  if (s == "-") { return "T"; }
  if (s == "..-") { return "U"; }
  if (s == "...-") { return "V"; }
  if (s == ".--") { return "W"; }
  if (s == ".--.") { return "X"; }
  if (s == "-.--") { return "Y"; }
  if (s == "--..") { return "Z"; }

  if (s == ".--.-") { return "Å"; }
  if (s == "-.-.") { return "Ä"; }
  if (s == "---.") { return "Ö"; }

  if (s == ".----") { return "1"; }
  if (s == "..---") { return "2"; }
  if (s == "...--") { return "3"; }
  if (s == "....-") { return "4"; }
  if (s == ".....") { return "5"; }
  if (s == "-....") { return "6"; }
  if (s == "--...") { return "7"; }
  if (s == "---..") { return "8"; }
  if (s == "----.") { return "9"; }
  if (s == "-----") { return "0"; }

  if (s == "........") { return "Error"; }
  if (s == ".-...") { return "&"; }
  if (s == ".----.") { return "\""; }
  if (s == ".--.-.") { return "@"; }
  if (s == "-.--.-") { return ")"; }
  if (s == "-.--.") { return "("; }
  if (s == "---...") { return ":"; }
  if (s == "--..--") { return ","; }
  if (s == "-...-") { return "="; }
  if (s == "-.-.--") { return "!"; }
    
  if (s == ".-.-.-") { return "."; }
  if (s == "-....-") { return "-"; }
  if (s == " -..-") { return "×"; }
  if (s == " ------..-.-----") { return "%"; }
  if (s == ".-.-.") { return "+"; }
  if (s == " .-..-.") { return "\""; }
  if (s == " ..--..") { return "?"; }
  if (s == " -..-.") { return "/"; }

  // Prosigns
  if (s == ".-.-") { return "AA"; }
  if (s == ".-.-.") { return "AR"; }
  if (s == ".-...") { return "AS"; }
  if (s == "-...-.-") { return "BK"; }
  if (s == "-...-") { return "BT"; }
  if (s == "-.-..-..") { return "CL"; }
  if (s == "-.-.-") { return "CT"; }
  if (s == "-..---") { return "DO"; }
  if (s == "-.-.-") { return "KA"; }
  if (s == "-.--.") { return "KN"; }
  if (s == "...-.-") { return "SK"; }
  if (s == "...-.") { return "SN"; }
  if (s == "...---...") { return "SOS"; }

  return "";
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Keyboard.begin();
  pinMode(paddleLeft, INPUT);
  pinMode(paddleRight, INPUT);
  lastInputMillis = millis();
}

// Should we append whitespace after completing the
// current Morse code character
bool appendSpace = true;

// Was the previous input a dit/0 or a dah/1
int previous = 0;

// the loop function runs over and over again forever
void loop() {

  if ((millis() - lastInputMillis) > WORD_SPACE_MS && buffer == "" && appendSpace == true) {
   buffer = "";
   //Serial.print(" ");
   Keyboard.print(" ");
   appendSpace = false;
  }

  if ((millis() - lastInputMillis) > CLEAR_MS && buffer != "") {
    buffer = "";
    //Serial.println("");
    lastInputMillis = millis();
  }

  paddleLeftState = digitalRead(paddleLeft);
  paddleRightState = digitalRead(paddleRight);  
  
  if (((millis() - lastInputMillis) > LETTER_SPACE_MS) && buffer.length() > 0) {
    //Serial.print(morseDecode(String(buffer)));
    String decoded = morseDecode(String(buffer));
    // In case of error "dit-dit-dit-dit-dit-dit-dit-dit" use backspace
    if (decoded == "Error") {
      Keyboard.write(8);
      appendSpace = false;  
    }
    // Return/newline
    else if (decoded == "AA") {
      Keyboard.println("");
      appendSpace = false;
    } else {
      Keyboard.print(morseDecode(String(buffer)));
      appendSpace = true;
    }
    buffer = "";
    lastInputMillis = millis();
  }
  
  // Are we ready for a new input?
  if ((millis() - lastInputMillis) > DEBOUNCE_DELAY) {
    // Left paddle
    if (paddleLeftState == LOW && paddleRightState == HIGH) {
      tone(BUZZER, TONE_HZ);
      delay(DIT_MS);
      noTone(BUZZER);
      buffer += ".";
      lastInputMillis = millis();
      previous = 0;
    }
    // Right paddle
    else if (paddleLeftState == HIGH && paddleRightState == LOW) {
      tone(BUZZER, TONE_HZ);
      delay(DA_MS);
      noTone(BUZZER);
      buffer += "-";
      lastInputMillis = millis();
      previous = 1;
    }
    // Both paddles
    else if (paddleLeftState == LOW && paddleRightState == LOW) {
      tone(BUZZER, TONE_HZ);
      if (previous == 0) {
        delay(DA_MS);
        previous = 1;
        buffer += "-";
      } else if (previous == 1) {
        delay(DIT_MS);
        previous = 0;
        buffer += ".";
      }
      noTone(BUZZER);
      lastInputMillis = millis();
    }
  }
}
