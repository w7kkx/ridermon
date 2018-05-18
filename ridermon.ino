
unsigned long timeMilli = 0;
unsigned long lastEventMilli = 0;
unsigned long lastToneStart = 0;
unsigned long sensorCount = 0; 
unsigned long rfidCount = 0;
unsigned long totalRfidCount = 0; 
unsigned long totalSensorCount = 0;
boolean hasEvent = true;

#define SENSORPIN  2
#define GREEN 3
#define RED 4 
#define YELLOW 5
#define TONE 6

const unsigned long timeoutMillis = 3000;

const unsigned long toneDurationMillis = 250;

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
      digitalWrite( GREEN, LOW );
      digitalWrite( RED, HIGH );
      digitalWrite( YELLOW, LOW );
    } else if (rfidCount > sensorCount ) { 
      digitalWrite( GREEN, LOW );
      digitalWrite( RED, LOW );
      digitalWrite( YELLOW, HIGH );
    } else { 
      digitalWrite( GREEN, HIGH );
      digitalWrite( RED, LOW );
      digitalWrite( YELLOW, LOW );
    }

    if( timeMilli- lastToneStart > toneDurationMillis ) { 
       digitalWrite( TONE, LOW );
    }else{
      digitalWrite( TONE, HIGH );
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

