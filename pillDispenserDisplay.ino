// Tell the compiler to use only one core
#if CONFIG_RTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#include <SPI.h>
#include <TFT_eSPI.h>
#include <stdio.h>
#include <vector>

#define PORTRAIT 0
#define LANDSCAPE 3
#define SCREENWIDTH 320
#define SCREENHEIGHT 240

#define KEYCOLOR TFT_LIGHTGREY
#define KEYPRESSEDCOLOR TFT_MAGENTA
#define KEYTEXTCOLOR TFT_BLACK
#define KEYFONTSIZE 2
#define KEYSPACING 23;


static uint16_t x_touch = 0;
static uint16_t y_touch = 0;

volatile bool scheduleChanged = false;

// Task handles
static TaskHandle_t get_input_handle = NULL;

uint8_t interval1,interval2,interval3;
uint8_t intervals[3];

// Display object
TFT_eSPI display = TFT_eSPI();


class Button{

  public:
    Button(uint16_t xBeginningCoord=0, uint16_t yBeginningCoord=0, uint16_t w=0, uint16_t h=0,  String s = "", uint16_t f = 2){

      xBegin = xBeginningCoord;
      yBegin = yBeginningCoord;
      xEnd = xBegin + w;
      yEnd = yBegin + h;
      width = w;
      height = h;
      label  = s;
      fontSize = f;
      selected = false;
    }

    String label;
    uint16_t xBegin;
    uint16_t yBegin;
    uint16_t xEnd;
    uint16_t yEnd;
    uint16_t fontSize = 2;
    int height;
    int width;
    bool selected;

    void draw(void){

      int buttonColour;

      if (selected == false) buttonColour = TFT_LIGHTGREY;
      else buttonColour = TFT_MAGENTA;

      display.fillRoundRect(xBegin, yBegin, width, height, 5, buttonColour);
      display.setCursor(xBegin+ (width/2 - 2.5 *label.length()) -fontSize * 3,    yBegin+ height*0.4 - fontSize * 1.5);
      display.setTextColor(TFT_BLACK);
      display.println(label);
    }

};


Button compartment1[4];
Button compartment2[4];
Button compartment3[4];


void createPage(void){

  // Since a "page" can be thought of as an array of buttons, well, thats what pages will be in this code
//Button button_one(10,10, 50, 50, "1" );
  display.setTextColor(TFT_WHITE);

  int xgap, ygap, width, height, xBegin, yBegin;
  height = 30;
  width = 40;
  xgap = 20;
  ygap = 30;
  xBegin = 90;
  yBegin = 30;

  for (int h = 1; h < 4; h++){

    display.setCursor(10, 35 + (h-1)*(height + ygap));
    display.print("Tray ");
    display.print(h);

    
    xBegin = 90; 

    for (int i = 0; i < 4; i++){


      String buttonLabel = String((i+1) * 1) + "s";
      switch (h){

        case 1:
          compartment1[i] = (Button(xBegin, yBegin, width, height, buttonLabel));
          break;
        case 2:
          compartment2[i] = (Button(xBegin, yBegin, width, height, buttonLabel));
          break;
        case 3:
          compartment3[i] = (Button(xBegin, yBegin, width, height, buttonLabel));
          break;
      }
      xBegin +=   (width + xgap);     
    }
    yBegin +=  (height+ ygap); 
  
  }

  compartment1[0].selected = true;
  compartment2[0].selected = true;
  compartment3[0].selected = true;


}


uint8_t getInterval(int compartment[]);

void getTouch(void *p){

  Button *temp = NULL;
  while(1){

    if (display.getTouch(&x_touch, &y_touch)){

        y_touch = 240 - y_touch;
      
        temp = findPressedKey(x_touch, y_touch, compartment1);
        if(temp == NULL)  temp = findPressedKey(x_touch, y_touch, compartment2);
        if (temp == NULL) temp = findPressedKey(x_touch, y_touch, compartment3);



        if (temp != NULL){
          
          
          if(temp->selected == false){
            

            temp->selected = true;
            drawPage();
            scheduleChanged = true;
            
            display.setCursor(25, 200);
            display.setTextColor(TFT_WHITE);
            display.println("Schedule Changed!");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            display.setCursor(25, 200);
            display.setTextColor(TFT_BLACK);
            display.println("Schedule Changed!");      

          }

          
        }
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void detectScheduleChange(void *p){

  
  while(1){

    if (scheduleChanged){

      
      scheduleChanged = false;
      intervals[0] = getInterval(compartment1);
      intervals[1] = getInterval(compartment2);
      intervals[2] = getInterval(compartment3);

      Serial2.write(intervals, sizeof(intervals));

      

      
    }


    
  }

  vTaskDelay(10/portTICK_PERIOD_MS);
}




Button* findPressedKey(uint16_t x, uint16_t y, Button temp[]){


  for (int i = 0; i < 4; i++){

    if ((x >= temp[i].xBegin) && (x <= temp[i].xEnd) && (y >= temp[i].yBegin) && (y <= temp[i].yEnd)){

      for (int j = 0; j < 4; j++){

        if (j != i) temp[j].selected = false;
      }

      Button *b = &temp[i];
      return b;
    }
  }

  return NULL;

}



void drawPage(void){

   for (int i = 0; i < 4; i++){

    compartment1[i].draw();
  }

   for (int i = 0; i < 4; i++){

    compartment2[i].draw();
  }

   for (int i = 0; i < 4; i++){

    compartment3[i].draw();
  }
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial2.begin(9600);

  display.init();
  display.setRotation(LANDSCAPE);
  display.fillScreen(TFT_BLACK);
  display.setTextSize(2);

  createPage();
  drawPage();

  //scheduleChangeQueue = xQueueCreate(5, sizeof(bool));

  xTaskCreatePinnedToCore(getTouch,
                        "getTouch",
                        2048,
                        NULL,
                        1,
                        &get_input_handle,
                        app_cpu
  );


  xTaskCreatePinnedToCore(detectScheduleChange,
                        "detectScheduleChange",
                        2048,
                        NULL,
                        1,
                        NULL,
                        app_cpu
  );
  



  

}

void loop() {
  // put your main code here, to run repeatedly:

}



uint8_t getInterval(Button compartment[]){


  for (int i = 1; i < 5; i++){

    if (compartment[i-1].selected == true) return i;

  }

  return 0;
}
