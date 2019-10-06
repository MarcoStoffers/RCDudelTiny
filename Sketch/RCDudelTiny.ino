/*************************************************************************************
* RCDudelTiny
* ============
* 
* Steuerung eines MP3-Players mit dem China IC GPD2806A:
* - Stick kurz nach oben = Power on / Play / Pause
* - Stick 2x kurz nach oben (innerhalb 1sec) = Power off
* - Stick lang nach oben = Lauter
* - Stick kurz nach unten = nächster Titel
* - Stick 2x kurz nach unten (innerhalb 1sec) = vorheriger Titel
* - Stick lang nach unten = Leiser
* 
* Bei RC-Signal Verlust schaltet der MP3-Player ab
* 
* Inputs:
* --------
* D8 = Receiver Input
* 
* Outputs:
* ---------
* D0  = Play/Pause Relais
* D1  = Next Relais
* D2  = Volume + Relais
* D3  = Volume - Relais
* D9  = Previous Relais
* D10 = Power Relais 
* 
* Hardware: ATtiny84A/8MHz internal | ATtiny Core von Spence Konde
* Author: Marco Stoffers
* Year: 2018
* License: CC | BY, NC, SA
*************************************************************************************/
/*************************************************************************************
 * User Defines
 *************************************************************************************/
#define Lauter_Leiser_Schritt_Abstand 1000
/*************************************************************************************
 * Defines
 *************************************************************************************/
#define Power 10
#define PlayPause 0
#define Next 1
#define Previous 9
#define VolumePlus 2
#define VolumeMinus 3
#define receiver_pin 8
#define between(x, a, b)  (((a) <= (x)) && ((x) <= (b)))
/*************************************************************************************
 * Variables
 *************************************************************************************/
volatile boolean rc_ok=false;
boolean eingeschaltet=false,hold_up=false,hold_down=false,meassuring=false;
boolean marker_playpause=false,marker_nextprevious=false;
uint8_t outputs[6]={Power, PlayPause, Next, Previous, VolumePlus, VolumeMinus};
uint8_t i,j,rc_avg_count=0;
volatile uint16_t rc_temp=0,rc_value=0;
uint16_t np_avg=0;
volatile uint32_t start_micros,stop_micros,micros_helper;
uint32_t rc_millis, volume_millis=0,playpause_millis=0,nextprevious_millis=0;
/*************************************************************************************
 * Setup
 *************************************************************************************/
void setup() {
  for(i=0;i<6;i++) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], LOW);
  }
  // Set Inputs
  pinMode(receiver_pin, INPUT);
  // enable Receiver input 
  delay(500); 
  attachInterrupt(0, receiver, CHANGE);
  // wait until RC input is ok
  while(!rc_ok) { }
  while(np_avg < 900){
  // learn RC middle
  // only when rc_value between 1400 and 1600
    if(between(rc_value, 1400, 1600)){
      for(i=0; i<10; i++) {
         np_avg += rc_value;
         delay(100);
      }
      np_avg = np_avg / 10;
    }
  }
}
/*************************************************************************************
 * Main Loop
 *************************************************************************************/
void loop() {
  // Stick nach oben
  if(rc_ok) {
    if(rc_value > np_avg + 250) {
      if(meassuring) {
        if(rc_millis < millis()) {
          Lauter();
          hold_up=false;
        }
        else hold_up=true;
      }
      else {
        meassuring=true;
        rc_millis=millis() + 750;
      }
    }
    // Stick nach unten
    else if(rc_value < np_avg - 250) {
      if(meassuring) {
        if(rc_millis < millis()) {
          Leiser();
          hold_down=false;
        }
        else hold_down=true;
      }
      else {
        meassuring=true;
        rc_millis=millis() + 750;
      }
    }
    // Stick in der Mitte
    else {
      if(hold_up || marker_playpause) {
        Ein_Play_Pause();
      }
      if(hold_down || marker_nextprevious) {
        Naechster();
      }
     meassuring=false;
    }
  }
  else PowerOff();
}
/*************************************************************************************
 * Subroutine Lautstärke lauter
 *************************************************************************************/
void Lauter() {
  if(volume_millis < millis()) {
    digitalWrite(VolumePlus, HIGH);
    delay(300);
    digitalWrite(VolumePlus, LOW);
    volume_millis = millis() + Lauter_Leiser_Schritt_Abstand;
  }  
}
/*************************************************************************************
 * Subroutine Lautstärke leiser
 *************************************************************************************/
void Leiser() {
  if(volume_millis < (millis())) {
    digitalWrite(VolumeMinus, HIGH);
    delay(300);
    digitalWrite(VolumeMinus, LOW);
    volume_millis = millis() + Lauter_Leiser_Schritt_Abstand;
  }  
}
/*************************************************************************************
 * Subroutine Einschalten, Play, Pause
 *************************************************************************************/
void Ein_Play_Pause() {
  if(!marker_playpause) {
    marker_playpause = true;
    playpause_millis = millis() + 1000;
    hold_up = false;
  }
  if((playpause_millis > millis()) && hold_up) {
    digitalWrite(Power, LOW);
    eingeschaltet=false;
    hold_up = false;
    marker_playpause = false;
  }
  if((playpause_millis < millis()) && !hold_up) {
    if(!eingeschaltet) {
      eingeschaltet = true;
      digitalWrite(Power, HIGH);
      marker_playpause = false;
    }
    else {
      digitalWrite(PlayPause, HIGH);
      delay(300);
      digitalWrite(PlayPause, LOW);
      marker_playpause = false;
    }
  }
}
/*************************************************************************************
 * Subroutine nächster Titel
 *************************************************************************************/
void Naechster() {
  if(!marker_nextprevious) {
    marker_nextprevious = true;
    nextprevious_millis = millis() + 1000;
    hold_down = false;
  }
  if((nextprevious_millis > millis()) && hold_down) {
    digitalWrite(Previous, HIGH);
    delay(300);
    digitalWrite(Previous, LOW);
    marker_nextprevious = false;
    hold_down = false;
  }
  if((nextprevious_millis < millis()) && !hold_down) {
    digitalWrite(Next, HIGH);
    delay(300);
    digitalWrite(Next, LOW);
    marker_nextprevious = false;
  }  
}
/*************************************************************************************
 * Subroutine Ausschalten MP3 Player
 *************************************************************************************/
void PowerOff() {
  digitalWrite(Power, LOW);
  eingeschaltet=false;  
}
/*************************************************************************************
 * Interrupt-subroutine to get Receiver Input
 *************************************************************************************/
void receiver() {
  micros_helper = micros();
  if(digitalRead(receiver_pin)) {
    start_micros = micros_helper;
  }
  else {
    stop_micros = micros_helper;
    if(start_micros < stop_micros) {
      rc_temp = rc_temp + (stop_micros - start_micros);
      rc_avg_count++;
    }
    // calculate average value of last 3 measurements and round to 10'er
    if(rc_avg_count > 2) {
      rc_value = rc_temp / 3;
      rc_value = ((rc_value/10)*10);
      rc_avg_count = 0;
      rc_temp = 0;
    }
    // check if value is correct
    if(between(rc_value, 900, 2100)) {
      rc_ok = true;
    }
    else rc_ok = false;
  }
}
