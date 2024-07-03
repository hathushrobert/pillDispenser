#include <Servo.h>
#include <EEPROM.h>

#define SERVO1 5
#define SERVO2 2
#define SERVO3 3
#define MOTORSIGNAL1 6
#define MOTORSIGNAL2 7
#define LARGESERVO 8

#define DISPENSED 5
#define REJECTED 6
#define ACCEPTED 7
#define EJECTREJECTION 8
#define REJECTIONRETURNED 9
#define EJECTPILLTRAY 10
#define PILLTRAYRETURNED 11

uint8_t intercom = 0;

int comp1 = 1000;
int comp2 = 1000;
int comp3 = 1000;

int iterationAngle = 0;
bool moving = false;

int IRvalue1 = 0;
int IRvalue2 = 0;
int IRvalue3 = 0;


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

uint8_t interval1 = 10;
uint8_t interval2 = 10;
uint8_t interval3 = 10;

uint8_t intervals[3];

Servo largeServo;

bool dispensed = false;

int positionToAngle(int position){

  return position * (180/22);
}


void setup() 
{

  cli();
  TimerInit();
  Serial.begin(9600);
  Serial1.begin(9600);


  largeServo.attach(LARGESERVO);

  pinMode(SERVO1, OUTPUT);
  pinMode(SERVO2, OUTPUT);
  pinMode(SERVO3, OUTPUT);
  pinMode(MOTORSIGNAL1, OUTPUT);
  pinMode(MOTORSIGNAL2, OUTPUT);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  DDRE = (1<<PE5) | (1<<PE4) | (1<<PE3);
  TCCR3A=(1<<COM3C1)|(1<<WGM31);
  TCCR3B=(1<<WGM33)|(1<<WGM32)|(0<<CS32) | (1<<CS31) | (0<<CS30);
  ICR3=40000;
  OCR3A=comp1;
  OCR3B=comp2;
  OCR3C=comp3;
  TCNT3=0;

  sei();
  
  

  
/*
    DO NOT DELETE THIS SECTION
  EEPROM.write(0, interval1);
  EEPROM.write(1, interval2);
  EEPROM.write(2, interval3);
  EEPROM.write(3, position1);
  EEPROM.write(4, position2);
  EEPROM.write(5, position3);

*/
  intervals[0] = 10; //EEPROM.read(0);
  intervals[1] = 10;// EEPROM.read(1);
  intervals[2] = 10; //EEPROM.read(2);

 /* position1 = EEPROM.read(3);
  position2 = EEPROM.read(4);
  position3 = EEPROM.read(5);

  servo1.write(positionToAngle(position1));
  servo2.write(positionToAngle(position2));
  servo3.write(positionToAngle(position3));
  */

  
  

}

void loop()  
{
  if (Serial1.available() == 3){
    
    Serial1.readBytes(intervals, 3);

    EEPROM.write(0, intervals[0]);
    EEPROM.write(1, intervals[1]);
    EEPROM.write(2, intervals[2]);


    Serial.print(intervals[0]);
    Serial.print(":");
    Serial.print(intervals[1]);
    Serial.print(":");
    Serial.println(intervals[2]);

    count1 = 0;
    count2 = 0;
    count3= 0;

  }

  if (Serial1.available() == 1){

    intercom = Serial1.read();

    if (intercom == EJECTREJECTION){

      TIMSK4 = (0 << TOIE4);
      while (1){

      delay(10);
     
      if (Serial1.available()){

        intercom = Serial1.read();
        if (intercom == REJECTIONRETURNED) break;
      }
    }
    TIMSK4 = (1 << TOIE4);
    }
  }



  if (count1 == intervals[0]){

    TIMSK4 = (0 << TOIE4);
    
    if (moving == false){
      moving = true;
      Serial.println("nice");
    }
    else moving = false;

    while (moving){

      IRvalue1 = analogRead(A0);
      Serial.println(IRvalue1);

      if (moving){

        if (IRvalue1 < 355 && iterationAngle > 80){

          moving = false;
          iterationAngle = 0;
        }
        else{
          comp1++;
          iterationAngle++;
          OCR3A = comp1;
        }

        if (position1 >= 22 || comp1 >= 4800){
        moving = false;

        int i;
        for (i = comp1; i > 0; i--){

          OCR3A = i;
          delay(1);
        }
        comp1 = 1000;

        }
      }
    }


    position1++;
    count1 = 0;
    dispensed = true;
  }


  if (count2 == intervals[1]){

    TIMSK4 = (0 << TOIE4);
    
    if (moving == false){
      moving = true;
      Serial.println("nice");
    }
    else moving = false;

    while (moving){

      IRvalue2 = analogRead(A1);
      Serial.println(IRvalue2);

      if (moving){

        if (IRvalue2 < 355 && iterationAngle > 80){

          moving = false;
          iterationAngle = 0;
        }
        else{
          comp2++;
          iterationAngle++;
          OCR3B = comp2;
        }

        if (position1 >= 22 || comp2 >= 4800){
        moving = false;

        int i;
        for (i = comp2; i > 0; i--){

          OCR3B = i;
          delay(1);
        }
        comp2 = 1000;

        }
      }
    }


    position2++;
    count2 = 0;
    dispensed = true;
  }



  if (count3 == intervals[2]){

    TIMSK4 = (0 << TOIE4);
    
    if (moving == false){
      moving = true;
      Serial.println("nice");
    }
    else moving = false;

    while (moving){

      IRvalue3 = analogRead(A2);
      Serial.println(IRvalue3);

      if (moving){

        if (IRvalue3 < 355 && iterationAngle > 80){

          moving = false;
          iterationAngle = 0;
        }
        else{
          comp3++;
          iterationAngle++;
          OCR3C = comp3;
        }

        if (position1 >= 22 || comp3 >= 4800){
        moving = false;

        int i;
        for (i = comp3; i > 0; i--){

          OCR3C = i;
          delay(1);
        }
        comp3 = 1000;

        }
      }
    }


    position3++;
    count3 = 0;
    dispensed = true;
  }








  











  if (dispensed){

    dispensed = false;
    intercom = DISPENSED;
    Serial1.write(intercom);
    delay(1000);

    while (1){

      TCNT4 = 0;
      if (Serial1.available()){

        int intercom = Serial1.read();
        if (intercom == ACCEPTED){

          analogWrite(MOTORSIGNAL1, 0);
          analogWrite(MOTORSIGNAL2, 100);
          delay(3000);
          analogWrite(MOTORSIGNAL1, 0);
          analogWrite(MOTORSIGNAL2, 0);

          largeServo.write(5);
          intercom = EJECTPILLTRAY;
          Serial1.write(intercom);

          while(1){

            if (Serial1.available()){
              intercom = Serial1.read();
              if (intercom == PILLTRAYRETURNED) break;
            }
          }
          largeServo.write(100);
          break;
        }
        else if (intercom == REJECTED){

          analogWrite(MOTORSIGNAL1, 100);
          analogWrite(MOTORSIGNAL2, 0);
          delay(3000);
          analogWrite(MOTORSIGNAL1, 0);
          analogWrite(MOTORSIGNAL2, 0);
          break;
        }
      }
      delay(5);
    }

    TIMSK4 = (1 << TOIE4);
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


  Serial.print(count1);
    Serial.print(":");
    Serial.print(count2);
    Serial.print(":");
    Serial.println(count3);
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




