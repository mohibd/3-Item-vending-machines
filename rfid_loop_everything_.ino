const int ledPin = 13;// we will turn the Pin 13 light on
#define  animato 120
#define C5  523
#define E5  659

#define speaker                A4     // digital pin number for speaker/buzzer

float default_tempo = float(animato); // default tempo - beats per minute
float current_tempo = default_tempo;
float timings[7];                     // holds timings for each defined note/rest time value
#define trills_per_crotchet     8     // number of note changes in a trill per crotchet
float trill_duration;                 // set depending on the tempo selected

//
// Standard note/rest duration definitions, set in the set_tempo function
//
#define semib       timings[0] // 4 beats
#define dot_minim   timings[1] // 3 beats
#define minim       timings[2] // 2 beats
#define crot        timings[3] // 1 beat
#define quav        timings[4] // 1/2 beat
#define semiq       timings[5] // 1/4 beat
#define demi_semiq  timings[6] // 1/8 beat

//
// Function waits for the given time in seconds, or part thereof.
//

#include<Servo.h>
Servo servo1;
Servo servo2;
Servo servo3;

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define RED 2
#define Green 3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// include the library code:
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Arduino pins attached to joystick
#define joystick_switch_pin  8
#define joystick_x_pin       A0
#define joystick_y_pin       A1

//Joystick values
#define up    0
#define right 1
#define down  2
#define left  3
#define enter 4
#define none  5

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int acess; //rfid check
// Menus
String main_menu[] = {"1.Help", "2.Food", "3.Drink"};

int pirPin = 6;                 // PIR Out pin
int pirStat = 0;                   // PIR status

// Current menu and item to be displayed
int current_menu_item;
String *current_menu;
// Used to check joystick state
int last_joy_read;
String Auth1 = "B2 6D F9 1B";
String AuthFob = "76 80 6B AF";

int read_joystick() {
  int output = none;
  // read all joystick values
  int X_Axis = analogRead(joystick_x_pin);     // read the x axis value
  int Y_Axis = analogRead(joystick_y_pin);     // read the y axis value
  Y_Axis = map(Y_Axis, 0, 1023, 1023, 0);      // invert the input from the y axis so that pressing the stick forward gives larger values
  int SwitchValue = digitalRead(joystick_switch_pin);  // read the state of the switch
  SwitchValue = map(SwitchValue, 0, 1, 1, 0);  // invert the input from the switch to be high when pressed

  if (SwitchValue == 1) {
    output = enter;
  } else if (X_Axis >= 900) {
    output = right;
  } else if (X_Axis <= 100) {
    output = left;
  } else if (Y_Axis >= 900) {
    output = up;
  } else if (Y_Axis <= 100) {
    output = down;
  }
  return output;
}

void wait(float duration) {
  uint32_t start;
  start = millis();
  duration = duration * 1000; // convert to milliseconds
  do {
  } while (millis() - start <= duration);
}

//
// Set tempo by adjusting durations of note/rest timings
//
void set_tempo(float new_tempo) {
  float crotchet_duration;
  current_tempo = new_tempo;           // keep current tempo up to date in case it needs to be queried
  crotchet_duration = 60 / new_tempo;  // timing in seconds for 1 beat
  semib      = crotchet_duration * 4;  // semibrieve, 4 beats
  dot_minim  = crotchet_duration * 3;  // dotted mimin, 3 beats
  minim      = crotchet_duration * 2;  // minim, 2 beats
  crot       = crotchet_duration;      // crotchet, 1 beat
  quav       = crotchet_duration / 2;  // quaver, 1/2 beat
  semiq      = crotchet_duration / 4;  // semiquaver, 1/4 beat
  demi_semiq = crotchet_duration / 8;  // demi-semi-quaver, 1/8 beat

  // Set the note duration time for one note in a trill depending on the
  // defined number of note changes in a trill per crotchet.
  trill_duration = crot / trills_per_crotchet;
}

//
// Play given note for given duration in seconds, or part thereof.
// Observe that this function is 'blocking', although the tone
// function is 'non-blocking', ie control stays with the function
// until note has completed.
//
void play(int note, float note_duration) {
  tone(speaker, note, note_duration * 1000); // play the given note for the given duration
  wait(note_duration);                       // wait for note to complete
}

//
// Function performs a basic trill with given two notes for the given duration
//
void trill(int note_1, int note_2, float note_duration) {
  note_duration = note_duration * 1000; // convert to milliseconds
  uint32_t start = millis();
  do {
    play(note_1, trill_duration);
    play(note_2, trill_duration);
  } while (millis() - start < note_duration);
}

//
// Rest for given duration in seconds, or part thereof.
//
void rest(float rest_duration) {
  noTone(speaker);      // ensure no tone is currently playing
  wait(rest_duration);  // wait for given rest to complete
}



void print_line(int line, String text) {
  lcd.setCursor(0, line);
  lcd.print("               ");
  lcd.setCursor(0, line);
  lcd.print(text);
}

void move_up() {

}

void move_down() {

}

void move_right() {
  if (current_menu_item >= sizeof(current_menu)) {
    current_menu_item = 0;
  } else {
    current_menu_item++;
  }
}
void move_left() {
  if (current_menu_item <= 0) {
    current_menu_item = sizeof(current_menu);
  } else {
    current_menu_item--;
  }
}
void door_bell_2() {
  set_tempo(default_tempo * 5); // lively pace
  // 3/4 time
  // bar 1
  play(E5, dot_minim);
  // bar 2
  play(C5, dot_minim);
  // bar 3
  rest(dot_minim);
  // bar 4
  play(E5, dot_minim);
  // bar 5
  play(C5, dot_minim);
  // bar 6
  rest(dot_minim);
  play(E5, dot_minim);
  // bar 7
  play(C5, dot_minim);
  // bar 8
  rest(dot_minim);
  // bar 9
  play(E5, dot_minim);
  // bar 10
  play(C5, dot_minim);
}

void setup() {
  pinMode(A4, OUTPUT);
  digitalWrite(A4, HIGH);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(7, INPUT);
  digitalWrite(7, HIGH);
  pinMode(speaker, OUTPUT);

  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader. ..");
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(Green, LOW);
  digitalWrite(RED, LOW);

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if ((content.substring(1) == AuthFob) || (content.substring(1) == Auth1)) //change here the UID of the card/cards that you want to give access
  {
    lcd.clear();

    lcd.backlight();
    lcd.clear();
    Serial.print("Authorized access ");
    Serial.print("you may use the venting machine");
    lcd.print("Authorized access");
    lcd.println("you may use the venting machine");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(Green, HIGH);
    delay(1000);
    digitalWrite(Green, LOW);
    acess = 1;

  }

  else   {
    lcd.clear();

    lcd.backlight();
    lcd.clear();
    lcd.println(" Access denied");
    lcd.print("go away");

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(RED, HIGH);
    delay(1000);
    digitalWrite(RED, LOW);
  }

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  // arduino default once time function
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // set up joy pins
  pinMode(joystick_switch_pin, INPUT_PULLUP);

  // Print template on lcd.
  lcd.setCursor(0, 0);
  lcd.print("Main Menu:");
  lcd.setCursor(0, 1);
  lcd.print(main_menu[current_menu_item]);

  // Init vars
  current_menu_item = 0;
  last_joy_read = none;
  delay(10);

  //init servos
  servo1.attach(4);
  servo2.attach(5);
  servo3.attach(1);


  current_menu = main_menu;
  pinMode(pirPin, INPUT);

}

void loop() {
  /*
   * while (rfid scan == allowed to use machine) {
   * test for card 
   *if card present{ 
   *and allowed to use 
   *exit
   *}
   *if no card present loop back to start
   *
   *if wrong card present {
   *show access denied display error msg for 5 seconds 
   *then request new card 
   *loop until correct card is shown 
   */

  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  pirStat = digitalRead(pirPin);

  {
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
        distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)


  }

  // "main" arduino program
  int current_joy_read = read_joystick();
  if (distance <= 100) {
    lcd.backlight();
  }
  else if (distance > 69) {
    lcd.noBacklight();
  }

  if (LED_BUILTIN, HIGH &&  acess == 1) {
    if (current_joy_read != last_joy_read) {
      last_joy_read = current_joy_read;

      switch (current_joy_read) {
        case up:
          move_up();
          break;
        case down:
          move_down();
          break;
        case right:
          move_right();
          break;
        case left:
          move_left();
        case enter:
          //menu option 1
          if (current_menu_item == 0) {
            lcd.setCursor(0, 1); lcd.print("U NEED HELP? SuS");

            delay(5000);
          }
          //menu option 3
          if (current_menu_item == 2) {
            servo1.write(180);
            pirStat = digitalRead(pirPin);

            delay (500);
            pirStat = digitalRead(pirPin);

            if (pirStat == HIGH) {            // if motion detected
              pirStat = digitalRead(pirPin);

              servo1.write(0);

              lcd.clear();
              lcd.println("enjoy your Drink");
              delay (1000);

              lcd.setCursor(0, 0);
              lcd.print("Main Menu:      ");
              print_line(1, current_menu[current_menu_item]);
              delay(100);
            }
            while (pirStat == LOW) {
              pirStat = digitalRead(pirPin);

              servo1.write(0);
              delay (500);
              servo1.write(180);
              delay (500);

              if (pirStat == HIGH) {            // if motion detected
                servo1.write(0);

                lcd.clear();
                lcd.println("enjoy your Drink");
                delay (1000);

                lcd.setCursor(0, 0);
                lcd.print("Main Menu:      ");
                print_line(1, current_menu[current_menu_item]);
                delay(100);
              }

            }
          }
          //menu optnion 2
          else if (current_menu_item == 1) {
            servo2.write(180);
            delay (500);

            if (pirStat == HIGH) {            // if motion detected
              pirStat = digitalRead(pirPin);

              servo2.write(0);

              lcd.clear();
              lcd.println("enjoy your Food");
              delay (1000);

              lcd.setCursor(0, 0);
              lcd.print("Main Menu:      ");
              print_line(1, current_menu[current_menu_item]);
              delay(100);

            }
            while (pirStat == LOW) {
              pirStat = digitalRead(pirPin);

              servo2.write(0);
              delay (500);
              servo2.write(180);
              delay (500);

              if (pirStat == HIGH) {            // if motion detected
                servo1.write(0);

                lcd.clear();
                lcd.println("enjoy your Food");
                delay (1000);

                lcd.setCursor(0, 0);
                lcd.print("Main Menu:      ");
                print_line(1, current_menu[current_menu_item]);
                delay(100);
              }
            }
          }
        default: break;

      }
    }
  }
  int digitalVal = digitalRead(7);
  if (HIGH == digitalVal)
  {
    digitalWrite(ledPin, LOW); //turn the led off
    Serial.println("No Tilt Detected");

  }
  else
  {
    digitalWrite(ledPin, HIGH); //turn the led on
    Serial.println("Tilt Detected");
   

    // clear line and print values to lcd

  }
  print_line(1, current_menu[current_menu_item]);
                delay(100);
}
