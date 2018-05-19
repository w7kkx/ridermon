
unsigned long timeMilli = 0;
unsigned long lastEventMilli = 0;
unsigned long lastToneStart = 0;
unsigned long sensorCount = 0; 
unsigned long rfidCount = 0;
unsigned long totalRfidCount = 0; 
unsigned long totalSensorCount = 0;
unsigned long blinkMilli = 0;
boolean hasEvent = true;



#define SENSORPIN  2
#define GREEN 12
#define RED 11
#define YELLOW 10
#define TONE 9

#define ONBOARD 13

const unsigned long timeoutMillis = 3000;
const unsigned long toneDurationMillis = 250;

const unsigned long blinkDurationMillis = 250;

void setup() {
  // setup the Interrupt listener for the beam break 
  attachInterrupt(digitalPinToInterrupt(SENSORPIN), onPinInterrupt, FALLING);  // TODO: Might match mode to beam break sensor
  // set up listening on the serial port  ( we do the serial port monitoring in the serialEvent function )
  Serial.begin(9600);

  // setup the pins
  pinMode( GREEN, OUTPUT );
  pinMode( RED, OUTPUT ); 
  pinMode( YELLOW, OUTPUT );
  pinMode( TONE, OUTPUT );
  
}

void loop() {
  watchMillis();
  updateIndicatorPins();
  monitorTimeout();
}

void onPinInterrupt(){
  lastEventMilli = timeMilli;
  sensorCount++;
  totalSensorCount++;
  hasEvent=true;
}

void onRFIDEvent(){ 
  lastEventMilli = timeMilli; 
  rfidCount++;
  totalRfidCount++;
  lastToneStart=timeMilli;
  hasEvent = true;
}

void serialEvent(){
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      onRFIDEvent();
    }
  }
}

void watchMillis(){
  timeMilli = millis(); 
}

void updateIndicatorPins(){ 
    if (rfidCount < sensorCount ) { 
      digitalWrite( GREEN, HIGH );
      digitalWrite( RED, LOW );
      digitalWrite( YELLOW, HIGH );
    } else if (rfidCount > sensorCount ) { 
      digitalWrite( GREEN, HIGH );
      digitalWrite( RED, HIGH );
      digitalWrite( YELLOW, LOW );
    } else { 
      digitalWrite( GREEN, LOW );
      digitalWrite( RED, HIGH );
      digitalWrite( YELLOW, HIGH );
    }

    if( timeMilli- lastToneStart > toneDurationMillis ) { 
       digitalWrite( TONE, HIGH );
    }else{
      digitalWrite( TONE, LOW );
    }

    if( (timeMilli - blinkMilli) > blinkDurationMillis ){ 
      digitalWrite( ONBOARD, HIGH );
      blinkMilli=timeMilli;
    }
    else{
      digitalWrite( ONBOARD, LOW );
    }
}

void monitorTimeout(){
   if( (timeMilli - lastEventMilli) > timeoutMillis ){
      sensorCount=0;
      rfidCount=0;
      // output a bit of json with the totals 
      if( hasEvent ){
        Serial.print( "{\"RFIDEvents\":" );
        Serial.print (  totalRfidCount );
        Serial.print(",\"SensorEvent\":");
        Serial.print( totalSensorCount );
        Serial.print( "}\n" );
        Serial.flush();
        hasEvent = false;
      }
   }
}

