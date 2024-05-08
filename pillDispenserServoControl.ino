#include <Servo.h>
#include <EEPROM.h>


/* Eeprom storage

address 0 interval1
address 1 interval2
address 2 interval3
address 3 position1
address 4 position2
address 5 position3



*/
uint8_t count1 =0;
uint8_t count2 =0;
uint8_t count3 =0;

uint8_t position1 = 0;
uint8_t position2 = 0;
uint8_t position3 = 0;

uint8_t interval1 = 0;
uint8_t interval2 = 0;
uint8_t interval3 = 0;

uint8_t intervals[3];

Servo servo1;
Servo servo2;
Servo servo3;


int positionToAngle(int position){

  return position * (180/22);
}


void setup() 
{
  TimerInit();
  Serial.begin(9600);
  Serial1.begin(9600);
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  
/*
    DO NOT DELETE THIS SECTION
  EEPROM.write(0, interval1);
  EEPROM.write(1, interval2);
  EEPROM.write(2, interval3);
  EEPROM.write(3, position1);
  EEPROM.write(4, position2);
  EEPROM.write(5, position3);

*/
  intervals[0] = EEPROM.read(0);
  intervals[1] = EEPROM.read(1);
  intervals[2] = EEPROM.read(2);

  position1 = EEPROM.read(3);
  position2 = EEPROM.read(4);
  position3 = EEPROM.read(5);

  servo1.write(positionToAngle(position1));
  servo2.write(positionToAngle(position2));
  servo3.write(positionToAngle(position3));
  

  
  

}

void loop()  
{
  if (Serial1.available()){
    
    Serial1.readBytes(intervals, 3);

    EEPROM.write(0, intervals[0]);
    EEPROM.write(1, intervals[1]);
    EEPROM.write(2, intervals[2]);


    Serial.print(intervals[0]);
    Serial.print(":");
    Serial.print(intervals[1]);
    Serial.print(":");
    Serial.println(intervals[2]);

  }

  if (count1 == intervals[0]){
    //servo1.write(positionToAngle(position1));
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    Serial.print("Servo 1 position: ");
    Serial.println(position1);
    position1++;
    count1 = 0;
  }
  if (count2 == intervals[1]){
    //servo2.write(positionToAngle(position2));
    digitalWrite(3, HIGH);
    delay(50);
    digitalWrite(3, LOW);
    Serial.print("Servo 2 position: ");
    Serial.println(position2);
    position2++;
    count2 = 0;
  }
  if (count3 == intervals[2]){
    //servo3.write(positionToAngle(position3));
    digitalWrite(4, HIGH);
    delay(50);
    digitalWrite(4, LOW);
    Serial.print("Servo 3 position: ");
    Serial.println(position3);
    position3++;
    count3 = 0;
  }




  if (position1 == 21){
      position1 = 0;
      servo1.write(0);
    }
  if (position2 == 21){
      position2 = 0;
      servo2.write(0);
    }
  if (position3 == 21){
      position3 = 0;
      servo3.write(0);
    }

  
  EEPROM.write(3, position1);
  EEPROM.write(4, position2);
  EEPROM.write(5, position3);

  

  


  delay(10);
}

void TimerInit()
{
  cli(); // Disable interrupts
  TCCR4A = 0; // Clear Timer4 control register A
  TCCR4B = (1 << CS42) | (1 << CS40); // Set prescaler to 1024
  TCCR4C = 0; // Clear Timer4 control register C
  TIMSK4 = (1 << TOIE4); // Enable Timer4 overflow interrupt
  TCNT4 = 0xC297; // Initialize Timer4 counter value for 1-second overflow


/*
  TCCR1A = 0; // Clear Timer1 control register A
  TCCR1B = (1 << CS12) | (1 << CS10); // Set prescaler to 1024
  TCCR1C = 0;
  TIMSK1 = (1 << TOIE1); // Enable Timer1 overflow interrupt
  TCNT1 = 63973; // Initialize Timer0 counter value

  */
  sei(); // Enable interrupts
}

ISR(TIMER4_OVF_vect)
{
  count1++; // Increment count
  count2++; // Increment count
  count3++; // Increment count
  TCNT4 = 0xC297; // Reset Timer4 counter value for next overflow
  //Serial.println(count); // Print "pigeon" to serial monitor
}

/*
ISR(TIMER1_OVF_vect)
{
  if (servoPulse) servoPulse = false;
  else servoPulse = true; 
  TCNT1 = 63973; // Reset Timer4 counter value for next overflow
  //Serial.println(count); // Print "pigeon" to serial monitor
}


*/




