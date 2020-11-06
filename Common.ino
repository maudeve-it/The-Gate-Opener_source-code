void InitPins() {    //called by setup: pins initializing...
  _PRINTLN_TEST(__FUNCTION__);
  pinMode(GATE_PIN, OUTPUT);
  pinMode(GSM_RESET_PIN, OUTPUT);
  digitalWrite(GATE_PIN, !ACTIVE_GATE);
  digitalWrite(GSM_RESET_PIN, HIGH);
  pinMode(RESET_PIN,INPUT);
  pinMode(GATE_LED_PIN, OUTPUT);
  pinMode(GSM_R_LED_PIN, OUTPUT);
  pinMode(GSM_G_LED_PIN, OUTPUT);
  pinMode(SMS_R_LED_PIN, OUTPUT);
  pinMode(SMS_G_LED_PIN, OUTPUT);
  pinMode(SMS_B_LED_PIN, OUTPUT);
  pinMode(INT_PIN,INPUT);
  digitalWrite(GATE_LED_PIN, !ACTIVE_GATE);
  digitalWrite(GSM_R_LED_PIN, LOW);
  digitalWrite(GSM_G_LED_PIN, LOW);
  digitalWrite(SMS_R_LED_PIN, LOW);
  digitalWrite(SMS_G_LED_PIN, LOW);
  digitalWrite(SMS_B_LED_PIN, LOW);
}



void SetTiming ( long delta, _step loopStep) {    //stores avg and max time for each step in main loop
static unsigned long loopCounter =0;
float lc;
long a;
//  _PRINTLN_TEST(__FUNCTION__);
  if (dontGetTime) {
    if (loopStep == LOOP) 
      dontGetTime=false;
    return;
  }
  lc=loopCounter; //going to float...
  avgTime[loopStep] = ((avgTime[loopStep] * lc)+delta)/(lc+1);
  if (maxTime[loopStep]<(delta)) {
    maxTime[loopStep]=delta;
  };
  if (loopStep==LOOP)
    loopCounter++;
}


/*
void Restart() {      //called to perform a software reset
//  setWdt(true);
  wdt_enable(WDTO_250MS);
  while(1);
}
*/

void (* Restart)( void )=0;    //alternative: without using watchdog 



void ResetPhone() {
  _PRINTLN_TEST(__FUNCTION__);
  dontGetTime = true;
  // reset phone
  digitalWrite(GSM_RESET_PIN, LOW);
  delay(GSM_SETUP_DELAY);
  digitalWrite(GSM_RESET_PIN, HIGH);
  delay(GSM_WAKEUP_DELAY);
  //initializing phone
  Restart();
}



void CheckHandReset() {
static unsigned long startTime;  //timer for flashing reset led
byte k;
String s;
short zero=0;  
bool stat[6];
#ifdef _TEST_HAND_RES
  static bool printed=false;
#endif
  _PRINTLN_TEST(__FUNCTION__);
  if (rstStatus == 'S'){    //Reset button previously pushed we are in 5-mins-Master-defining time
    #ifdef _TEST_HAND_RES
      if (!printed){
        Serial.println("5 mins grace time");
        printed=true;        
      }
    #endif
    if ((millis()-startTime)>(digitalRead(RESET_PIN) ? RST_LED_S_BLINK : RST_LED_F_BLINK)){  //managing Reset led flashing (couter is the flashing timer)
//      rstStatus='N';
      digitalWrite(RESET_PIN,!digitalRead(RESET_PIN));
      startTime=millis();
    }
    return;
  }
  // now (rstStatus != 'S')  
  if (digitalRead(RESET_PIN))    //pushing button reset_pin is low
    return;
  _PRINTLN_TEST_HAND_RES("Reset button pushed");
    
  #ifdef _TEST_WDT
    wdt_disable();               //disable watchdog 
  #endif
  startTime=millis();
  while ((millis() - startTime) < RST_BUT_DELAY) {
    if (digitalRead(RESET_PIN)){
      _PRINTLN_TEST_HAND_RES("Reset button released before RST_BUT_DELAY");
      
      #ifdef _TEST_WDT
        wdt_enable(WDTO_8S);    //enable watchdog for 8 secs timeout
        wdt_reset();            //reset watchdog
      #endif
      return;
    }
  };
  _PRINTLN_TEST_HAND_RES("RST_BUT_DELAY timer override for Reset button: going on");
  stat[0]=digitalRead(SMS_R_LED_PIN);
  stat[1]=digitalRead(SMS_B_LED_PIN);
  stat[2]=digitalRead(SMS_G_LED_PIN);
  stat[3]=digitalRead(GSM_R_LED_PIN);
  stat[4]=digitalRead(GSM_G_LED_PIN);
  stat[5]=digitalRead(GATE_LED_PIN);
  startTime=millis();
  while (!digitalRead(RESET_PIN))    // wait until reset button released
    if (millis()-startTime > RST_LED_F_BLINK){
      digitalWrite(SMS_R_LED_PIN, !digitalRead(SMS_R_LED_PIN));
      digitalWrite(SMS_B_LED_PIN, digitalRead(SMS_R_LED_PIN));
      digitalWrite(SMS_G_LED_PIN, digitalRead(SMS_R_LED_PIN));
      digitalWrite(GSM_R_LED_PIN, digitalRead(SMS_R_LED_PIN));
      digitalWrite(GSM_G_LED_PIN, digitalRead(SMS_R_LED_PIN));
      digitalWrite(GATE_LED_PIN, (digitalRead(SMS_R_LED_PIN) ? ACTIVE_GATE : !ACTIVE_GATE));
      startTime=millis();
    };
  _PRINTLN_TEST_HAND_RES("Reset button released (after timeout): going on");
    
  digitalWrite(SMS_R_LED_PIN,stat[0]);
  digitalWrite(SMS_B_LED_PIN,stat[1]);
  digitalWrite(SMS_G_LED_PIN,stat[2]);
  digitalWrite(GSM_R_LED_PIN,stat[3]);
  digitalWrite(GSM_G_LED_PIN,stat[4]);
  digitalWrite(GATE_LED_PIN,stat[5]);
  pinMode(RESET_PIN,OUTPUT);
  digitalWrite(RESET_PIN,LOW);
  for (k=0;k<20;k++){
    delay(RST_LED_F_BLINK);
    digitalWrite(RESET_PIN,!digitalRead(RESET_PIN));
  }
  digitalWrite(RESET_PIN,LOW);
  startTime=millis();

  _PRINTLN_TEST_HAND_RES("Reset button released (after timeout): going on");
  while (CleanSimCard(true)) {               //delete all phonebook registrations
    _PRINTLN_TEST_HAND_RES("ONE SIMCARD RECORD DELETED");
    if ((millis()-startTime)>RST_LED_F_BLINK){
      digitalWrite(RESET_PIN,!digitalRead(RESET_PIN));
      startTime=millis();
    }
  };
  expiringUsers=false;

  CheckSIM800Requests(true);    //this starts 5 min timer to register new masters via phone call

  #ifdef _TEST_WDT
    wdt_enable(WDTO_8S);    //enable watchdog for 8 secs timeout
    wdt_reset();   //reset watchdog
  #endif
  
}


/*
void ISR_powerfail() {
  doShutdown=true;
}


void SaveToEeprom() {
int k;  
  doShutdown=false;
  preShutdowns=countShutdowns;
  EEPROM.put( POW_RES_COUNTER, (countShutdowns+1));
  for (k=0; k=14; k++) 
    EEPROM.update(POW_RES_TIME+k,currClock.charAt(k));
  postShutdowns=countShutdowns+1;
}


void setWdt(bool resetmode) {
// "resetmode=true" set timeout= 250ms. "resetmode=true" set timeout= 8s.   
  cli();         //disables all interrupts on the microcontroller
  wdt_reset();   // resets the watchdog timer. This isnʼt always necessary but... 
  
  if (resetmode) {
//   SREG &= ~(1<<SREG_I); //disable interrupt
   WDTCSR |= ((1<<WDCE) | (1<<WDE));    //set watchdog register
   WDTCSR = ((1<<WDE) | (1<<WDP2));     //set "reset" mode and 250ms timeout
//   SREG |= (1<<SREG_I); //enable interrupts
  } else {
//   SREG &= ~(1<<SREG_I); //disable interrupt
   WDTCSR |= ((1<<WDCE) | (1<<WDE));               //set watchdog register
   WDTCSR = ((1<<WDIE)| (1<<WDP3) | (1<<WDP0));    //set "interrupt" mode and 8s timeout
//   SREG |= (1<<SREG_I); //enable interrupts
  }
  sei();
}

*/
