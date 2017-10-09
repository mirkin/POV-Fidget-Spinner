
/*
 * 
 */
#include "font.h"
#include <avr/sleep.h>
//
// clear and set interup routines
#ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
//
boolean test=false;
byte hallPin = 8; 
byte ledPin=7;
byte button_1=6;
int LED_PINS[] = {0, 1, 2, 3, 4, 5, 9, 10};
bool LED_STATES[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
byte noLEDS=8;
int off=0;
int mode=1;
int maxMode=8;
volatile boolean ledON=true;
volatile boolean sensorTripped=false;
unsigned int RPM=120;
unsigned long lastSensorTime;
unsigned long deltaTime;
unsigned long pixelDelay=250UL;
boolean doStuff=false;
boolean hallVersionB=false;
/*
 * 1,000,000 microseconds in a second
 * 1,000 microseconds in a millisecond
 * delay(milliseconds)
 * delayMicroseconds(micros)
 */

void setup() {
  /* size of array divided by size one item takes up should give the length
   * if it's byte int etc. wouldn't work for array of strings.
  */
  noLEDS=sizeof(LED_PINS)/sizeof(LED_PINS[0]);
  if (test)
  {
    pinMode(ledPin, OUTPUT);
  }
  if (hallVersionB)
  {
    pinMode(hallPin, INPUT_PULLUP);
  }
  else
  {
    pinMode(hallPin, INPUT);
  }
  pinMode(button_1, INPUT_PULLUP);
  //
  for(int c=0; c<noLEDS; c++){
    pinMode(LED_PINS[c], OUTPUT);
    digitalWrite(LED_PINS[c], LED_STATES[c]);
  }
  //
  lastSensorTime = millis();
  deltaTime=250UL;
  //digitalWrite(hallPin,HIGH);
  //
  //GIMSK=B00100000; // General Interrupt Mask Register 5th bit enables PCMSK1 4th PCMSK0
  //PCMSK1=B00000100; // Pin Change Mask Register1 pin 8-11 9 enabled here to match hall pin
  GIMSK  = bit (PCIE1);   
  PCMSK1 = bit (PCINT10); //PCINT10 pin 7
  PCMSK0=B00000000; // Pin Change Mask Register0 pin 0-7
  sei();      // enable all interrupts
 
}

void loop() {
  // put your main code here, to run repeatedly:
  doStuff=false;
  /*
  if (ledON){
    flash();
  }
  */
  if (checkButton(button_1))
  {
    mode++;
    if (mode>maxMode)
    {
      mode=1;
    }
    showMode();
  }
  if (test){
    if (ledON)
    {
      digitalWrite(ledPin, HIGH); 
    }
    else
    {
      digitalWrite(ledPin, LOW); 
    }
  }
/*
  if (pixelDelay>3000) // slower than 3 seconds per rev perhaps hall sensor borked
  {
    doStuff=true;
    deltaTime=250UL; //4 turns per second
    pixelDelay=deltaTime * 3UL / 360UL; //1 rev time divided by 360 degrees * pixel degree
  } 
*/

  if (sensorTripped)
  {
    //new revolution
    sensorTripped=false;
    //
    deltaTime=millis()-lastSensorTime;
    lastSensorTime = millis();
    pixelDelay=deltaTime * 5UL / 360UL; //1 rev time divided by 360 degrees * pixel degree
    doStuff=true;
  }

  
  //turn it back on

  if (mode==1)
  {
    for (int i = 0; i < 8; i++){
      digitalWrite(LED_PINS[i] , ledON);
    }
  }
  else if (mode==2 && doStuff)
  {
    showWord(" Alex ");
  }
  else if (mode==3 && doStuff)
  {
    showWord(" - ZIVA - ");
  }
  else if (mode==4 && doStuff)
  {
    showWord("------------");
  }
  else if (mode==5 && doStuff)
  {
    showWord("ALEX & ZIVA");
  }
  else if (mode==6 && doStuff)
  {
    showWord(":) :) :) :) :)");
  }
  else if (mode==7){
    LED_STATES[off]=HIGH;
    off++;
    if (off >= noLEDS) {
      off=0;
    } 
    //turn new one off
    LED_STATES[off]=LOW;
    for(int c=0; c<noLEDS; c++) {
      digitalWrite(LED_PINS[c], LED_STATES[c]);
    }
    //delayMicroseconds(200000UL);
    delay(200);
  }
  else if (mode==8)
  {
    for (int i = 0; i < 8; i++){
      digitalWrite(LED_PINS[i] , LOW);
    }
    delay(100);
    for (int i = 0; i < 8; i++){
      digitalWrite(LED_PINS[i] , HIGH);
    }
    delay(100);
  }
}

bool checkButton(byte b){
  if (!digitalRead(b)){
    while(!digitalRead(b)){
      delay(5);
    }
    return true;
  }
  else
  {
    return false;
  }
}

void showWord(String letters){
  char charBuff[12];
  letters.toCharArray(charBuff, 12);
  int l = letters.length();
  for (int c = 0; c < l; c++) {
    if (sensorTripped)
    {
      return;
    }
    showLetter(charBuff[c]);
  }
}

void showLetter(char letter){
  //char letter=65;
  //letter-=32;
  for (int i=0; i<5; i++) { // each column
    char b = pgm_read_byte_near(&(font[letter-32][i]));
    for (int j=0; j<8; j++) {
      digitalWrite(LED_PINS[j], bitRead(b, 7-j));
    }
    pixelPause();
  }
  
  //clear the LEDs
  for (int i = 0; i < 8; i++){
    digitalWrite(LED_PINS[i] , LOW);
  }
  pixelPause();
}

void pixelPause(){
  //unsigned long pixelDelay=250 * 3UL / 360UL;
  delay(pixelDelay);
}

/*
 * Flash number of times to show current mode
 */
void showMode(){
  // all of for a short while
  for(int l=0; l<noLEDS; l++){
      digitalWrite(LED_PINS[l], LOW);
  }
  delay(200);
  for(int c=0; c<mode; c++){
    //all on
    delay(150);
    for(int l=0; l<noLEDS; l++){
      digitalWrite(LED_PINS[l], HIGH);
    }
    delay(150);
    //all off
    for(int l=0; l<noLEDS; l++){
      digitalWrite(LED_PINS[l], LOW);
    }
  }
  delay(200);
}

void showStates(){
  for(int c=0; c<noLEDS; c++){
    digitalWrite(LED_PINS[c], LED_STATES[c]);
  }
}

void flash(){
  /*
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10);                       // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second
  */
}

ISR(PCINT1_vect){  // Pin Interrupt 1 (pins 8-11)
  if (hallVersionB)
  {
    if (!digitalRead(hallPin)){
      sensorTripped=true;
    }
  }
  else
  {
    if (digitalRead(hallPin)){
      ledON=true;
      sensorTripped=true;
    }
    else
    {
      ledON=false;
    }
  }
}
