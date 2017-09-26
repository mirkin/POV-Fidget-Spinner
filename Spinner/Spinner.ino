
/*
 * 
 */

byte hallPin = 9; 
byte ledPin=10;
int LED_PINS[] = {0, 1, 2, 3, 4, 5, 6, 7};
bool LED_STATES[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int off=0;
volatile boolean ledON=true;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(hallPin, INPUT);
  //
  for(int c=0; c<(sizeof(LED_PINS)); c++){
    pinMode(LED_PINS[c], OUTPUT);
    digitalWrite(LED_PINS[c], LED_STATES[c]);
  }
  //digitalWrite(hallPin,HIGH);
  //
  GIMSK=B00100000; // General Interrupt Mask Register 5th bit enables PCMSK1 4th PCMSK0
  PCMSK1=B00000010; // Pin Change Mask Register1 pin 8-11 9 enabled here to match hall pin
  PCMSK0=B00000000; // Pin Change Mask Register0 pin 0-7
  sei();      // enable all interrupts
 
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  if (ledON){
    flash();
  }
  */
  
  if (ledON)
  {
    digitalWrite(ledPin, HIGH); 
  }
  else
  {
    digitalWrite(ledPin, LOW); 
  }
  //turn it back on
  LED_STATES[off]=HIGH;
  off++;
  if (off >= sizeof(LED_PINS)/sizeof(LED_PINS[0])) {
    off=0;
  }
  //turn new one off
  LED_STATES[off]=LOW;
  for(int c=0; c<(sizeof(LED_PINS)/sizeof(LED_PINS[0])); c++){
    digitalWrite(LED_PINS[c], LED_STATES[c]);
  }
  delay(1000);
  
  
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
  if (digitalRead(hallPin)){
    ledON=true;
  }
  else
  {
    ledON=false;
  }
}
