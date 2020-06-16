#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#include "pitches.h"    //refers to the second tab pitch definitions used in melody generator

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

  
int redlight = 12; //pin assignment for red LED
int blackbutton = 11;  //pin assignment for black (select) button
int yellowbutton = 10;  //pin assignment for yellow (enter) button
int foiltouch = 13;  //pin assignment for aluminum foil that, when touched by tweezers, registers a failed surgery
int foilstate = 0;  //variable that registers whether tweezers are currently touching foil.  1 is not touching, 0 is touching
int holster = 2;  //pin assignment for the metal holster for the tweezers, breaking this connection initiates timer for surgeries
int holsterdone = 3;    //pin assignment that turns analog pin 5 into a digital pin for reading a completed surgery

int holsterstate = 1;  //variable that registers whether tweezers are currently touching holster. 1 is not touching, 0 is touching
int blackstate = 1;  //variable that shows whether black button is pressed.  1 is not, 0 is pressed
int yellowstate = 1;   //variable that shows whether yellow button is pressed.  1 is not, 0 is pressed
int playernumber = 1;   //variable that registers whether a one or two player game has been selected by user
int winningscore = 1000;  //variable that determines how many $ must be earned to win game
int onescore = 0;   //variable that holds the amount of $ earned by player one
int twoscore = 0;   //variable that holds the amount of $ earned by player two
int threescore = 0;
int fourscore = 0;

long firstdoc;   //variable used in the random generator that determines the doctor name assigned to player one
long seconddoc;   //variable used in the random generator that determines the doctor name assigned to player two
long thirddoc;
long forthdoc;
long i[4];

int playerturn;  //variable used to determine which player's turn is next
int firstname;   //variable that holds the randomly generated doctor name for player one
int secondname;   //variable that holds the randomly generated doctor name for player two
int thirdname;
int forthname;


long randomsurgery;  //variable used in the random number generator used to determine which surgery is next
int procedurecost;  //variable that holds the amount of $ randomly generated for a procedure
int tweezerstate;  //variable that is used to determine whether tweezers are touching the holster
int timer;    //holds the amount of seconds left during a surgery
int t;     //used to govern a loop in the timer function
int x;   //used to govern a loop in the timer function
int payment;    //the payment for the surgery adjusted for how many seconds remain on the timer
int success = 0;    //registers whether the player was successful in surgery in order to update their score
int countertone;     //used in countdown sequence during surgery to advance musical tone up the scale during surgery
int randomlefty = 0;   //used in the random generator for bonus lefty surgery
int lefty = 0;         //registers whether the surgery is a bonus lefty surgery worth extra
int leftyfreq = 2;


const unsigned char wretched_bone [] PROGMEM= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x0f, 0x00, 0x00, 0x03, 0xfe, 0x3f,
0xc0, 0x00, 0x07, 0xfe, 0x7f, 0xe0, 0x00, 0x0f, 0xfe, 0x7f, 0xe0, 0x03, 0xff, 0xfe, 0xff, 0xff,
0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff,
0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xef,
0xfe, 0x7f, 0xff, 0x00, 0x07, 0xfe, 0x7f, 0xf0, 0x00, 0x03, 0xfc, 0x7f, 0xe0, 0x00, 0x01, 0xf8,
0x3f, 0xc0, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char broken_heart [] PROGMEM ={0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x3c, 0x00, 0x07, 0xfe, 0x00, 0xff, 0x80, 0x0f,
0xfe, 0x03, 0xff, 0xe0, 0x1f, 0xfe, 0x07, 0xff, 0xf0, 0x3f, 0xfe, 0x07, 0xff, 0xf8, 0x3f, 0xfe,
0x07, 0xff, 0xf8, 0x7f, 0xff, 0x07, 0xff, 0xfc, 0x7f, 0xff, 0x83, 0xff, 0xfc, 0x7f, 0xff, 0x83,
0xff, 0xfc, 0x7f, 0xff, 0x87, 0xff, 0xfc, 0x3f, 0xff, 0x8f, 0xff, 0xfc, 0x3f, 0xff, 0x0f, 0xff,
0xf8, 0x3f, 0xff, 0x0f, 0xff, 0xf8, 0x3f, 0xff, 0x07, 0xff, 0xf8, 0x1f, 0xff, 0x87, 0xff, 0xf0,
0x0f, 0xff, 0x8f, 0xff, 0xe0, 0x0f, 0xff, 0xcf, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xc0, 0x03,
0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xff,
0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xff,
0xe0, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0xfe, 0x00,
0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 

const unsigned char bread_basket [] PROGMEM ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00,
0x1f, 0xe0, 0x1f, 0xe0, 0x3f, 0xf0, 0x3f, 0xf0, 0x7f, 0xf8, 0x3f, 0xf8, 0x7f, 0xf8, 0x3f, 0xf8,
0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8,
0x3f, 0xfc, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xf0,
0x7f, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8,
0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc,
0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xf8,
0x3f, 0xff, 0xff, 0xf0, 0x0f, 0x80, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char water_on_the_knee [] PROGMEM ={0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x03, 0xff, 0x80, 0x07, 0xff, 0xc0, 0x0f, 0xff, 0xe0, 0x1f,
0x83, 0xe0, 0x3f, 0x00, 0xf0, 0x3e, 0x00, 0x78, 0x7c, 0x00, 0x78, 0x7c, 0x00, 0x38, 0x78, 0x00,
0x3c, 0x78, 0x00, 0x3c, 0x7f, 0x00, 0x7c, 0x7f, 0xff, 0xfc, 0x3f, 0xff, 0xfc, 0x3f, 0xff, 0xf8,
0x3f, 0xff, 0xf8, 0x3f, 0xff, 0xf0, 0x3f, 0xff, 0xf8, 0x3f, 0xff, 0xf8, 0x3f, 0xff, 0xf8, 0x3f,
0xff, 0xf0, 0x3f, 0xff, 0xf0, 0x3f, 0xff, 0xf0, 0x3f, 0xff, 0xf0, 0x1f, 0xff, 0xf0, 0x1f, 0xff,
0xf0, 0x1f, 0xff, 0xe0, 0x1f, 0xff, 0xe0, 0x0f, 0xff, 0xe0, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00};


const unsigned char writer_cramp [] PROGMEM ={0x00, 0x38, 0x38, 0x38, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70, 0x70, 0x70, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00};


const unsigned char funny_bone [] PROGMEM ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x7e,
0x7e, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x03, 0xff, 0x7f, 0xc7, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xfc,
0x3f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0xff, 0xc0, 0x07, 0xfe, 0xff, 0x00, 0x01, 0xfe,
0x7c, 0x00, 0x00, 0xfc, 0x78, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00};


void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  pinMode(redlight, OUTPUT);       //pin configurations
  pinMode(blackbutton, INPUT);
  pinMode(yellowbutton, INPUT);
  pinMode(foiltouch, INPUT);
  pinMode(holster, INPUT);
  pinMode(holsterdone, INPUT);

  pinMode(14, INPUT);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  delay(5000);
}

void loop() {            
lighton(); 
welcome();
welcome_melody();
lightoff();
delay(2000);
playerselect();
delay(2000);
scoreset();
delay(2000);
leftyrarity();
delay(2000);
nameassign();
delay(4000); 
mainloop();
gameover();
delay(200);          
}


void mainloop()  {            //MAINLOOP includes functions to: show which player's turn, select and display a surgery and $ value,
                     //detect the tweezers lifting, time the surgery, determine points earned by player, display running score total
  playerturn = 1;
  onescore = 0;
  twoscore = 0;
  threescore = 0;
  fourscore = 0;

  while ((onescore < winningscore) && (twoscore < winningscore) && (threescore < winningscore) && (fourscore < winningscore)) {
    showturn();
    delay(3000);

    randomoperation();
    delay(4000);
    
    leftybonus();
    tweezerready();
    delay(200);
    
    timedsurgery();
    delay(2000);

    scoredisplay();
    delay(4000);



    if (playerturn < playernumber)
      playerturn++;
      
    else
     playerturn = 1;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
}

void welcome() {     //brief splash opening screen with name of game
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("      Welcome to"));
  display.println(F("    Hurry, Doctor! "));
  display.display(); 
}

void welcome_melody()  {     //theme music that plays during splash screen
int melody[] = {
  NOTE_D4, NOTE_CS4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_FS4, NOTE_FS4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_CS4, NOTE_D4, NOTE_E4, NOTE_A4     //melody can be altered here calling different pitches defined in tab 2 (pitches.h)
};
int noteDurations[] = {
  8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 1                                 
}; 
  for (int thisNote = 0; thisNote < 17; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(3, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(3);
  }}

void success_melody()  {     //theme music that plays after successful surgery
  int melody[] = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, 0, NOTE_C4, NOTE_F4, 0,         //melody can be altered here calling different pitches defined in tab 2 (pitches.h)
  };
  
  int noteDurations[] = {
    8, 8, 8, 4, 16, 8, 2, 8                                 
  };
   
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(3, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(3);
  }}  

void failure_melody()  {     //theme music that plays after failed surgery
  int melody[] = {
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4,          //melody can be altered here calling different pitches defined in tab 2 (pitches.h)
  };
  
  int noteDurations[] = {
    4, 4, 4, 4, 1                                 
  }; 
  for (int thisNote = 0; thisNote < 5; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(3, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(3);
  }}  
  
void lighton()   {                   //simple function to turn red LED on
    digitalWrite(redlight, HIGH);
  }
  
void lightoff()   {               //simple function to turn red LED off
    digitalWrite(redlight, LOW);
  }
  
void playerselect() {      //procedure for choosing a one player or two player game using black button to change selection and yellow button to enter selection
 yellowstate = 1;
 blackstate = 1;
 display.clearDisplay();
 display.setCursor(0, 0);
 display.println(F("Players: 1"));
 display.println(F("Select or Enter"));
 display.display();
 int toggled = 1;                                   //toggle variable keeps track of how many times the black button has been pushed, determining number of players selected

while (yellowstate == 1){
  lightoff();
  yellowstate = digitalRead(yellowbutton);
  blackstate = digitalRead(blackbutton);
  if ((blackstate == 0)) {
      toggled++;
      if(toggled >= 5)
        toggled = 1;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(F("Players: "));
      display.setCursor(54, 0);
      display.println((String(toggled)));
      display.println(F("Select or Enter"));
      display.display();
      delay(300);
      blackstate = 1;
      
  }
}

lighton();
delay(400);
display.clearDisplay();
display.setCursor(0, 0);

switch(toggled){
  case 1:
    display.println(F("    1 Player Game"));
    display.println(F("       Selected"));
    display.display();
    playernumber = 1;
    break;
    
  case 2:
    display.println(F("    2 Player Game"));
    display.println(F("       Selected"));
    display.display();
    playernumber = 2;
    break;

  case 3:
    display.println(F("    3 Player Game"));
    display.println(F("       Selected"));
    display.display();
    playernumber = 3;
    break;

  case 4:
    display.println(F("    4 Player Game"));
    display.println(F("       Selected"));
    display.display();
    playernumber = 4;
    break;
  }

yellowstate = 1;
lightoff();
}

void scoreset() {      //procedure for choosing the number of dollars needed to win the game
  yellowstate = 1;
  blackstate = 1;
  winningscore = 1000;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("    To Win: $1000"));
  display.println(F("   Select or Enter"));
  display.display();
  int toggled = 0;                                   //toggle variable keeps track of how many times the black button has been pushed, determining cash value selected

while (yellowstate == 1) {
  lightoff();
  yellowstate = digitalRead(yellowbutton);
  blackstate = digitalRead(blackbutton);
  if ((blackstate == 0) && (toggled == 0)) {
    toggled = 1;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("    To Win: $1500"));
    display.println(F("   Select or Enter"));
    display.display();
    delay(300);
    blackstate = 1;
  }
  if ((blackstate == 0) && (toggled == 1)) {
    toggled = 2;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("    To Win: $2000"));
    display.println(F("   Select or Enter"));
    display.display();
    delay(300);
    blackstate = 1;
  }
   if ((blackstate == 0) && (toggled == 2)) {
    toggled = 0;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("    To Win: $1000"));
    display.println(F("   Select or Enter"));
    display.display();
    delay(300);
    blackstate = 1;
  }}

lighton();
  delay(400);
  display.clearDisplay();
  display.setCursor(0, 0);
  
  if (toggled == 0) {
    display.println(F("     $1000 Needed"));
    display.println(F("        To Win"));
    display.display();
    winningscore = 1000;
  }
  
  if (toggled == 1) {
    display.println(F("     $1500 Needed"));
    display.println(F("        To Win"));
    display.display();
    winningscore = 1500;
  }
  
  if (toggled == 2) {
    display.println(F("     $2000 Needed"));
    display.println(F("        To Win"));
    display.display();
    winningscore = 2000;
  }

  yellowstate = 1;
  lightoff();
}

void leftyrarity() {      //procedure for choosing how frequently the 'lefty bonus' surgeries occur
  yellowstate = 1;
  blackstate = 1;
  leftyfreq = 2;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("    Lefty: Normal"));
  display.println(F("   Select or Enter"));
  display.display();
  int toggled = 0;                                   //toggle variable keeps track of how many times the black button has been pushed, determining lefty frequency

while (yellowstate == 1) {
  lightoff();
  yellowstate = digitalRead(yellowbutton);
  blackstate = digitalRead(blackbutton);
  if ((blackstate == 0) && (toggled == 0)) {
    toggled = 1;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("     Lefty: Rare"));
    display.println(F("   Select or Enter"));
    display.display();
    delay(300);
    blackstate = 1;
  }
  if ((blackstate == 0) && (toggled == 1)) {
    toggled = 2;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("      Lefty: Off"));
    display.println(F("   Select or Enter"));
    display.display();
    delay(300);
   blackstate = 1;
  }
   if ((blackstate == 0) && (toggled == 2)) {
    toggled = 0;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("   Lefty: Normal"));
    display.println(F("   Select or Enter"));
    display.display();
    delay(300);
    blackstate = 1;
  }
}

  lighton();
  delay(400);
  display.clearDisplay();
  display.setCursor(0, 0);
  
  if (toggled == 0) {
    display.println(F("   Lefty Bonuses:"));
    display.println(F("     Normal (50%)"));
    display.display();
    leftyfreq = 2;
  }
  
  if (toggled == 1) {
    display.println(F("   Lefty Bonuses:"));
    display.println(F("      Rare (33%)"));
    display.display();
    leftyfreq = 3;
  }
  
  if (toggled == 2) {
    display.println(F("   Lefty Bonuses:"));
    display.println(F("     Off (0%)"));
    display.display();
    leftyfreq = 0;
  }
  yellowstate = 1;
  lightoff();
}

void nameassign ()  {       //randomly generates doctor names for each player from a list of seven possibilities. 
  firstdoc = 0;
  seconddoc = 0;
  thirddoc = 0;
  forthdoc = 0;
  int match = 1;
  while (match == 1) {
    match = 0;
    randomSeed(analogRead(0));                //seeds the random number with the unpredictable noise on unconnected pin A0; seems to help with true randomness
    firstname = i[0] = firstdoc = random(7);                  //random number generated on startup always seems to be the same
    secondname = i[1] = seconddoc = (random(4))+(random(4));   //another attempt to achieve greater randomness- arduino initiates the exact same 'random' list of numbers, so the first
    thirdname = i[2] = thirddoc = (random(2)+random(3)+ random(4));
    forthname = i[3] = forthdoc = 3;//(random(2)+random(3)+ random(2)+ random(3));
    for(int j=0; j<playernumber; j++)
      if(playernumber != 1)
        for(int k = j+1; k<playernumber; k++)
          if(i[j] == i[k])
            match = 1;
  }
  int assign = 1;
  display.clearDisplay();
  display.setCursor(0, 0);

while(assign < playernumber+1){
  if(assign == 1)
    display.setCursor(0, 0);
  if(assign == 2)
    display.setCursor(0, 8);
  if(assign == 3)
    display.setCursor(0, 16);
  if(assign == 4)
    display.setCursor(0, 23);
  switch(i[assign-1]){
    case 0: 
      display.print(String(assign));
      display.println(F(": Dr. Grave"));
      display.display();
      break;
   case 1: 
      display.print(String(assign));
      display.println(F(": Dr. Balloon"));
      display.display();
      break;
   case 2: 
      display.print(String(assign));
      display.println(F(": Dr. Frozen"));
      display.display();
      break;
   case 3: 
      display.print(String(assign));
      display.println(F(": Dr. Hogwart"));
      display.display();
      break;
   case 4: 
      display.print(String(assign));
      display.println(F(": Dr. Squishy"));
      display.display();
      break;
   case 5: 
      display.print(String(assign));
      display.println(F(": Dr. Tombstone"));
      display.display();
      break;
   case 6: 
      display.print(String(assign));
      display.println(F(": Dr. Shakes"));
      display.display();
      break;
  }
  assign++;
}
}

void showturn(){                     //part of the main game loop- it shows the doctor name of whose turn is next.
  display.clearDisplay();
  display.setCursor(0, 0);
  
  switch(i[playerturn-1]){
    case 0: 
      display.println(F("Dr. Grave,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
    case 1: 
      display.println(F("Dr. Balloon,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
    case 2:
      display.println(F("Dr. Frozen,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
    case 3: 
      display.println(F("Dr. Hogwart,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
    case 4: 
      display.println(F("Dr. Squishy,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
    case 5: 
      display.println(F("Dr. Tombstone,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
    case 6: 
      display.println(F("Dr. Shakes,"));
      display.println(F("Get Ready!"));
      display.display();
      break;
  }
}


void randomoperation () {                    //part of the main game loop. randomly chooses a procedure and a maximum $ value to be earned, which decreases as time elapses
  randomSeed(analogRead(0));                   //the possible dollar ranges of each procedure can be set independently, if game board construction results in some surgeries being more
  randomsurgery = random(6);                 //difficult than others
  display.clearDisplay();
  display.setCursor(0, 0);
  
  switch(randomsurgery){
    case 0:
      display.println(F("Bread Basket"));
      display.drawBitmap(96, 0, bread_basket, 32, 32, 1);
      randomSeed(analogRead(0));
      procedurecost = (((random(3)+1))*100)+(((random(9)+1))*10);
      delay(1000);
      display.setCursor(0, 8);
      display.print(F("Up To $"));
      display.setCursor(43, 8);
      display.println(String(procedurecost));
      display.display();
      break;
      
    case 1:
      display.println(F("Funny Bone"));
      display.drawBitmap(96, 0, funny_bone, 32, 19, 1);
      randomSeed(analogRead(0));
      procedurecost = (((random(3)+1))*100)+(((random(9)+1))*10);
      delay(1000);
      display.setCursor(0, 8);
      display.print(F("Up To $"));
      display.setCursor(43, 8);
      display.println(String(procedurecost));
      display.display();
      break;
      
    case 2:
      display.println(F("Broken Heart"));
      display.drawBitmap(88, 0, broken_heart, 40, 32, 1);
      randomSeed(analogRead(0));
      procedurecost = (((random(3)+1))*100)+(((random(9)+1))*10);
      delay(1000);
      display.setCursor(0, 8);
      display.print(F("Up To $"));
      display.setCursor(43, 8);
      display.println(String(procedurecost));
      display.display();
      break;

    case 3:
      display.println(F("Writer's Cramp"));
      display.drawBitmap(122, 0, writer_cramp, 6, 32, 1);
      randomSeed(analogRead(0));
      procedurecost = (((random(3)+1))*100)+(((random(9)+1))*10);
      delay(1000);
      display.setCursor(0, 8);
      display.print(F("Up To $"));
      display.setCursor(43, 8);
      display.println(String(procedurecost));
      display.display();
      break;
      
    case 4:
      display.println(F("Wrenched Knee"));
      display.drawBitmap(88, 0, wretched_bone, 40, 19, 1);
      randomSeed(analogRead(0));
      procedurecost = (((random(3)+1))*100)+(((random(9)+1))*10);
      delay(1000);
      display.setCursor(0, 8);
      display.print(F("Up To $"));
      display.setCursor(43, 8);
      display.println(String(procedurecost));
      display.display();
      break;
      
    case 5:
      display.println(F("Water On The Knee"));
      display.drawBitmap(105, 0, water_on_the_knee, 23, 32, 1);
      randomSeed(analogRead(0));
      procedurecost = (((random(3)+1))*100)+(((random(9)+1))*10);
      delay(1000);
      display.setCursor(0, 8);
      display.print(F("Up To $"));
      display.setCursor(43, 8);
      display.println(String(procedurecost));
      display.display();
      break;
  }
  
}

void tweezerready()  {                //checks whether tweezers are in the holster, and if not, displays a message to return the tweezers.
  tweezerstate = digitalRead(holster);   //if tweezers are already in the holster or are returned, displays a message to pull tweezers from holster to begin a timed surgery.
  while (tweezerstate != 0)  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("   Return Tweezers"));
    display.println(F("    To Continue..."));
    tweezerstate = digitalRead(holster);
    display.display();
    delay(300);
  }
  
  tweezerstate = 0;
  while (tweezerstate == 0)  {
   display.clearDisplay();
   display.setCursor(0, 0);
   display.println(F("   Lift Tweezers"));
   display.println(F("      To Begin!"));
   tweezerstate = digitalRead(holster);
   display.display();
   delay(300);
  }
  tweezerstate = 1;
}

void leftybonus()  {       //this function randomly triggers a 'lefty bonus' surgery, worth an additional $150
  lefty = 0;
  randomlefty = 0;
  randomlefty = random(leftyfreq);
  Serial.print("Roll: ");
  Serial.println(randomlefty);
  Serial.print("Out of: ");
  Serial.println(leftyfreq);
  if (randomlefty == 1)  {
    lefty = 1;   
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("   Lefty Bonus..."));
    display.println(F("  Use Other Hand!"));
    display.display();
    delay(3000);
  }
}

void timedsurgery ()  {              //this function displays the current surgery on the top half of the LCD during surgery
  countertone = 80;
  String opname = "";
  display.clearDisplay();
  display.setCursor(0, 0);
  
  switch(randomsurgery){
    case 0:
      opname = "Bread Basket";
      display.println(F("Bread Basket"));
      display.drawBitmap(96, 0, bread_basket, 32, 32, 1);
      display.display();
      break;
      
    case 1:
      opname = "Funny Bone";
      display.println(F("Funny Bone"));
      display.drawBitmap(96, 0, funny_bone, 32, 19, 1);
      display.display();
      break;
      
    case 2:
      opname = "Broken Heart";
      display.println(F("Broken Heart"));
      display.drawBitmap(88, 0, broken_heart, 40, 32, 1);
      display.display();
      break;
    
    case 3:
      opname = "Writer's Cramp";
      display.println(F("Writer's Cramp"));
      display.drawBitmap(122, 0, writer_cramp, 6, 32, 1);
      display.display();
      break;
      
    case 4:
      opname = "Wrenched Knee";
      display.println(F("Wrenched Knee"));
      display.drawBitmap(88, 0, wretched_bone, 40, 19, 1);
      display.display();
      break;

    case 5:
      opname = "Water On The Knee";
      display.println(F("Water On The Knee"));
      display.drawBitmap(105, 0, water_on_the_knee, 23, 32, 1);
      display.display();
      break;
  }
  timer = 19;                    //this countdown timer gives 20 seconds for surgery and also polls both the foil bed and holster every microsecond to see if the tweezers touch

  tweezerstate = 1;
  foilstate = 1;
  delay(1000);
  tweezerstate = 1;
  foilstate = 1;
  for (t = 0; t < 19; t++)  {
    while ((tweezerstate == 1) && (foilstate == 1) && (timer > -1)) {
      for (x = 0; x < 1000; x++)  {
        tweezerstate = digitalRead(holsterdone);
        foilstate = digitalRead(foiltouch);
        delay(1);
        if ((tweezerstate == 0) || (foilstate == 0)) {
          lighton();
          break;
        }
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(opname);
    if(opname == "Bread Basket")
       display.drawBitmap(96, 0, bread_basket, 32, 32, 1);
    if(opname == "Funny Bone")
      display.drawBitmap(96, 0, funny_bone, 32, 19, 1);
    if(opname == "Broken Heart")
      display.drawBitmap(88, 0, broken_heart, 40, 32, 1);
    if(opname == "Writer's Cramp")
      display.drawBitmap(122, 0, writer_cramp, 6, 32, 1);
    if(opname == "Wrenched Knee")
      display.drawBitmap(88, 0, wretched_bone, 40, 19, 1);
    if(opname == "Water On The Knee")
      display.drawBitmap(105, 0, water_on_the_knee, 23, 32, 1);


    
    display.setCursor(64, 8);
    display.println(String(timer));
    display.display();
    tone(3, (countertone), 1000);
    timer = timer - 1;
    countertone = countertone + 45;
    if (timer < 1)  {
      break;
    }
   }
  }


  if (tweezerstate == 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
  
    display.println(F("       Success,"));
    display.println(F("       Doctor!"));
    display.display();
    success = 1;
    success_melody();
    delay(3000);
    display.clearDisplay();
    display.setCursor(0, 0);
  
    display.println(F("    Return Object"));
    display.println(F("        To Bin"));
    display.display();
    delay(4000);
  }
  else{
    display.clearDisplay();
    display.setCursor(0, 0);
  
    display.println(F("       Surgery"));
    display.println(F("       Failed!"));
    display.display(); 
    success = 0;
    failure_melody();
  }
}

void scoredisplay() {
  lightoff();
  payment = procedurecost - ((procedurecost / 20)) * ((18 - timer)); 
  if (payment < 100) {
    payment = 100;
  }

  if(success == 1)
    switch(playerturn){
      case 1:
        onescore = onescore + payment;
        break;
        
      case 2:
        twoscore = twoscore + payment;
        break;

      case 3:
        threescore = threescore + payment;
        break;
        
      case 4:
        fourscore = fourscore + payment;
        break;
    }

    if((success == 1) && (lefty == 1)){
      display.clearDisplay();
      display.setCursor(0, 0);
    
      display.println(F("    Offhand Bonus"));
      display.println(F("      Achieved!"));
      display.display();
      switch(playerturn){
        case 1:
          onescore = onescore + 150;
          
          success_melody();
          delay(3000);
          break;
        
        case 2:
          twoscore = twoscore + 150;
          success_melody();
          delay(3000);
          break;

        case 3:
          threescore = threescore + 150;
          success_melody();
          delay(3000);
          break;
        
        case 4:
          fourscore = fourscore + 150;
          success_melody();
          delay(3000);
          break;
      }  
    }
    
  switch(playernumber){
    case 1:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.display();
      break;

     case 2:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.setCursor(0, 8);
      display.print(F("Player 2: $"));
      display.println(String(twoscore));
      display.display();
      break;

    case 3:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.setCursor(0, 8);
      display.print(F("Player 2: $"));
      display.println(String(twoscore));
      display.setCursor(0, 16);
      display.print(F("Player 3: $"));
      display.println(String(threescore));
      display.display();
      break;

    case 4:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.setCursor(0, 8);
      display.print(F("Player 2: $"));
      display.println(String(twoscore));
      display.setCursor(0, 16);
      display.print(F("Player 3: $"));
      display.println(String(threescore));
      display.setCursor(0, 24);
      display.print(F("Player 4: $"));
      display.println(String(fourscore));
      display.display();
      break;
    
  }
}


void gameover(){                    //when the winning threshold is achieved, show a congratulatory message if a one player game, who the winner was in a two player game,
  display.clearDisplay();
  display.setCursor(0, 0);
  int score[playernumber];
  
  switch(playernumber){
    case 1:
      score[0] = onescore-400;
      break;

    case 2:
      score[0] = onescore;
      score[1] = twoscore;
      break;

    case 3:
      score[0] = onescore;
      score[1] = twoscore;
      score[2] = threescore;
      break;

    case 4:
      score[0] = onescore;
      score[1] = twoscore;
      score[2] = threescore;
      score[3] = fourscore;
      break;
  }

  int max_v = onescore;
  int max_i = 0;
  if(playernumber > 1){
    for(int l=0; l<playernumber; l++){
      if(score[l]>max_v){
        max_v = score[l];
        max_i = l;
      }
    }
  }
  
  display.print(F("  Congratulations,"));
  display.display();
  display.setCursor(0, 8);
  

  switch(max_i){
    case 0:
      display.print(F("    Player 1 Wins!"));
      display.display();
      break;
      
    case 1:
      display.print(F("    Player 2 Wins!"));
      display.display();
      break;

    case 3:
      display.print(F("    Player 3 Wins!"));
      display.display();
      break;

    case 4:
      display.print(F("    Player 4 Wins!"));
      display.display();
      break;
  }
  success_melody();
  delay(4000);

  switch(playernumber){
    case 1:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.display();
      break;

     case 2:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.setCursor(0, 8);
      display.print(F("Player 2: $"));
      display.println(String(twoscore));
      display.display();
      break;

    case 3:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.setCursor(0, 8);
      display.print(F("Player 2: $"));
      display.println(String(twoscore));
      display.setCursor(0, 16);
      display.print(F("Player 3: $"));
      display.println(String(threescore));
      display.display();
      break;

    case 4:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("Player 1: $"));
      display.println(String(onescore));
      display.setCursor(0, 8);
      display.print(F("Player 2: $"));
      display.println(String(twoscore));
      display.setCursor(0, 16);
      display.print(F("Player 3: $"));
      display.println(String(threescore));
      display.setCursor(0, 24);
      display.print(F("Player 4: $"));
      display.println(String(fourscore));
      display.display();
      break;
    
  }


  delay(7000);
  yellowstate = 1;
  while (yellowstate == 1)  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("   Press Enter Key"));
    display.println(F("    To Play Again"));
    display.display();
    yellowstate = digitalRead(yellowbutton);
    delay(50);
  }
  yellowstate = 1;
}
