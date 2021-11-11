/* Отладка -------------------------------------------------------------------*/
#define DEBUG // Если не нужна служебная информация по Serial. Закоментить
/* ---------------------------------------------------------------------------*/
/* Основная структура настроек и состояний ------------------------------------*/
typedef struct
{
    String      Version = "FW v.0.100b";    // Версия прошивки
    uint8_t     Key_right : 1;    // 6 кнопка
    uint8_t     Key_left  : 1;     // 1 кнопка
    uint8_t     Key_up    : 1;       // 4 кнопка
    uint8_t     Key_down  : 1;     // 3 кнопка
    uint8_t     Key_S1    : 1;       // 0 кнопка 
    uint8_t     Key_S2    : 1;       // 4 кнопка
    long        Led_blink_interval = 600;
    unsigned long currentMillis=0;
    long        previousMillis_L=0;
    long        previousMillis_R=0;
    long        previousMillis_E=0;
    uint8_t     Led_left_state=0;
    uint8_t     Led_right_state=0;
    uint8_t     Led_Emergency_state=0;
    bool        Power_ON = false;

} Headlights_set;

Headlights_set set;

#include "AnalogMultiButton.h"

const int BUTTONS_PIN = A0;
const int BUTTONS_TOTAL = 6;
const int BUTTONS_VALUES[BUTTONS_TOTAL] = {0,100,200,300,400,500};

AnalogMultiButton buttons(BUTTONS_PIN, BUTTONS_TOTAL, BUTTONS_VALUES);

/* Настройки LED Strip -------------------------------------------------------*/
#define COLOR_DEBTH 1 // Настройка цветовой глубины LED Strip 1, 2, 3 (байт на цвет)

#include "microLED.h" // Подключение библиотеки для LED Strip

#define STRIP_FR_PIN 4  // LED Strip Передний правый 
#define STRIP_FL_PIN 5  // LED Strip Передний левый 
#define STRIP_BR_PIN 6  // LED Strip Задний правый 
#define STRIP_BL_PIN 7  // LED Strip Задний Левый 
#define STRIP_BC_PIN 8  // LED Strip Задний центральный 
// Указываем количество светодиодов в каждой ленте
#define NUMLEDS_FR 16
#define NUMLEDS_FL 16
#define NUMLEDS_BR 16
#define NUMLEDS_BL 16
#define NUMLEDS_CN 16      

#define BRIGHTNESS_DEF            150
#define BRIGHTNESS_DEF_FRONT      150
#define BRIGHTNESS_DEF_BACK       150

#define BRIGHTNESS_MAX            255
/* ---------------------------------------------------------------------------*/
/* Инициализация классов Led Strip -------------------------------------------*/
microLED<NUMLEDS_FR, STRIP_FR_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> strip_FR;
microLED<NUMLEDS_FL, STRIP_FL_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> strip_FL;
microLED<NUMLEDS_BR, STRIP_BR_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> strip_BR;
microLED<NUMLEDS_BL, STRIP_BL_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> strip_BL;
microLED<NUMLEDS_CN, STRIP_BC_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> strip_BC;
/*----------------------------------------------------------------------------*/
/* Функция первичной инициализация светодиодных лент WS2812 ------------------*/
void Init_LED_Strip()
{
  // Настройка яркости при инициализации
  strip_FR.setBrightness(BRIGHTNESS_DEF_FRONT);
  strip_FL.setBrightness(BRIGHTNESS_DEF_FRONT);
  strip_BR.setBrightness(BRIGHTNESS_DEF_BACK);
  strip_BL.setBrightness(BRIGHTNESS_DEF_BACK);
  strip_BC.setBrightness(BRIGHTNESS_DEF_BACK);

  // очистка буфера (выключить диоды, чёрный цвет)
  strip_FR.clear();
  strip_FL.clear();
  strip_BR.clear();
  strip_BL.clear();
  strip_BC.clear();

  // применяется при выводе .show() !
  strip_FR.show(); 
  strip_FL.show();
  strip_BR.show(); 
  strip_BL.show();
  strip_BC.show();
}
/*----------------------------------------------------------------------------*/
/* Функция габаритных фонарей по умолчанию -----------------------------------*/
void Led_state_def()
{
  if (set.Power_ON == true)
  {
    if (set.Key_down == false && 
        set.Key_left == false &&
        set.Key_right == false &&
        set.Key_up == false &&
        set.Key_S1 == false &&
        set.Key_S2 == false)
    {
      strip_FR.setBrightness(BRIGHTNESS_DEF_FRONT);
      strip_FL.setBrightness(BRIGHTNESS_DEF_FRONT);
      strip_BR.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BL.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BC.setBrightness(BRIGHTNESS_DEF_BACK);

      strip_FR.fill(mWhite);
      strip_FL.fill(mWhite);
      strip_BR.fill(mRed);
      strip_BL.fill(mRed);
      strip_BC.fill(mRed);

      strip_FR.show(); 
      strip_FL.show();
      strip_BR.show(); 
      strip_BL.show();
      strip_BC.show();
    }
    if (set.Key_down == false &&  set.Key_left == false &&
                                  set.Key_right == false &&
                                  set.Key_S1 == false)
    {
      strip_BR.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BL.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BC.setBrightness(BRIGHTNESS_DEF_BACK);

      strip_BR.fill(mRed);
      strip_BL.fill(mRed);
      strip_BC.fill(mRed);

      strip_BR.show(); 
      strip_BL.show();
      strip_BC.show();
    }

    if (set.Key_left == true)
    {
      strip_FR.setBrightness(BRIGHTNESS_DEF_FRONT);
      strip_BR.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BC.setBrightness(BRIGHTNESS_DEF_BACK);

      strip_FR.fill(mWhite);
      strip_BR.fill(mRed);
      strip_BC.fill(mRed);

      strip_FR.show(); 
      strip_BR.show(); 
      strip_BC.show();
    }
    else if (set.Key_right == true)
    {
      strip_FL.setBrightness(BRIGHTNESS_DEF_FRONT);
      strip_BL.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BC.setBrightness(BRIGHTNESS_DEF_BACK);

      strip_FL.fill(mWhite);
      strip_BL.fill(mRed);
      strip_BC.fill(mRed);

      strip_FL.show(); 
      strip_BL.show(); 
      strip_BC.show();
    }
    if (set.Key_S1 == true)
    {
      strip_BC.setBrightness(BRIGHTNESS_DEF_BACK);
      strip_BC.fill(mRed);
      strip_BC.show();
    }
  }
}
/*----------------------------------------------------------------------------*/
/* Функции основной инициализации МК -----------------------------------------*/
void setup() 
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Debug Mode ON");
  Serial.println(set.Version);
  delay(1000); 
#endif

  Init_LED_Strip(); // Инициализация LED Strip
  set.Power_ON = true; // Заготовка для программного включения
  
  Led_state_def();  // Установка индикации по умолчанию

  set.Key_down=0;
  set.Key_up=0;
  set.Key_left = 0;
  set.Key_right = 0;
  set.Key_S1 = 0;
  set.Key_S2 = 0;
}
/*----------------------------------------------------------------------------*/
/* Основной цикл программы ---------------------------------------------------*/
void loop() 
{
  set.currentMillis = millis();
  buttons.update();   // Обновление считывания нажатых кнопок
/* Обработка нажатых кнопок --------------------------------------------------*/
  if(buttons.onPress(1))          // Обработка кнопки Left
  {
    if (set.Key_right == true)
    {  
        set.Key_right = ~set.Key_right; // Обработка кнопки Right
        set.Key_left = ~set.Key_left;
    }
    else set.Key_left = ~set.Key_left;
  } 
    
  if(buttons.onPress(5)) // Обработка кнопки Right
  {
    if (set.Key_left == true)
    {
      set.Key_left = ~set.Key_left;
      set.Key_right = ~set.Key_right;
    }
    else set.Key_right = ~set.Key_right;
  }
   
  if(buttons.onPress(0)) set.Key_S1 = ~set.Key_S1; // Обработка кнопки S1
  //if(buttons.onPress(4)) set.Key_S2 = ~set.Key_S2; // Обработка кнопки S2

  if(buttons.isPressed(2)) set.Key_down = true; // Обработка кнопки Down
    else set.Key_down = false;

  if(buttons.isPressed(3)) set.Key_up = true; // Обработка кнопки Up
    else set.Key_up = false;
/*----------------------------------------------------------------------------*/  
/* Обработка состояний устройства --------------------------------------------*/
  if(set.Key_down == true)   // Задний стоп сигнал
  {
    if (set.Key_left == true)
    { 
      strip_BR.setBrightness(BRIGHTNESS_MAX);
      strip_BC.setBrightness(BRIGHTNESS_MAX);
      strip_BR.fill(mRed);
      strip_BC.fill(mRed);
      strip_BR.show(); 
      strip_BC.show();
    }
    else if (set.Key_right == true)
    {
      strip_BL.setBrightness(BRIGHTNESS_MAX);
      strip_BC.setBrightness(BRIGHTNESS_MAX);
      strip_BL.fill(mRed);
      strip_BC.fill(mRed);
      strip_BL.show(); 
      strip_BC.show();
    }
    else if (set.Key_S1 == true)
    {
      strip_BC.setBrightness(BRIGHTNESS_MAX);
      strip_BC.fill(mRed);
      strip_BC.show();
    }
    else
    {
      strip_BR.setBrightness(BRIGHTNESS_MAX);
      strip_BL.setBrightness(BRIGHTNESS_MAX);
      strip_BC.setBrightness(BRIGHTNESS_MAX);
      strip_BR.fill(mRed);
      strip_BL.fill(mRed);
      strip_BC.fill(mRed);
      strip_BR.show();
      strip_BL.show(); 
      strip_BC.show();
    }
  } 
  else Led_state_def();  
/*----------------------------------------------------------------------------*/
/* Моргание левым поворотником -----------------------------------------------*/
  if(set.Key_left == true)
  {
    if(set.currentMillis - set.previousMillis_L > set.Led_blink_interval) 
    {
      // сохраняем последний момент, когда менялось состояние светодиода
      set.previousMillis_L = set.currentMillis;
      if (set.Led_left_state == true)
      {
        set.Led_left_state = false;
        strip_FL.setBrightness(BRIGHTNESS_MAX); 
        strip_BL.setBrightness(BRIGHTNESS_MAX);
        strip_FL.fill(mYellow);  
        strip_BL.fill(mYellow);  
        strip_FL.show();
        strip_BL.show();        
      }
      else
      {
        set.Led_left_state = true;
        strip_FL.fill(mBlack); 
        strip_BL.fill(mBlack);
        strip_FL.show();
        strip_BL.show();         
      }
    }
  }
  else
  {
    Led_state_def();
  }
/*----------------------------------------------------------------------------*/
/* Моргание правым поворотником ----------------------------------------------*/
  if(set.Key_right == true)
  {
    if(set.currentMillis - set.previousMillis_R > set.Led_blink_interval) 
    {
      // сохраняем последний момент, когда менялось состояние светодиода
      set.previousMillis_R = set.currentMillis;
      if (set.Led_right_state == true)
      {
        set.Led_right_state = false;
        strip_FR.setBrightness(BRIGHTNESS_MAX); 
        strip_BR.setBrightness(BRIGHTNESS_MAX);
        strip_FR.fill(mYellow);  
        strip_BR.fill(mYellow);  
        strip_FR.show();
        strip_BR.show();        
      }
      else
      {
        set.Led_right_state = true;
        strip_FR.fill(mBlack); 
        strip_BR.fill(mBlack);
        strip_FR.show();
        strip_BR.show();         
      }
    }
  }
  else //if (set.Key_S1 != true)
  {
    Led_state_def();
  }
/*----------------------------------------------------------------------------*/
/* Аварийная сигнализация ----- ----------------------------------------------*/
  if(set.Key_S1 == true)
  {
    if(set.currentMillis - set.previousMillis_E > set.Led_blink_interval) 
    {
      // сохраняем последний момент, когда менялось состояние светодиода
      set.previousMillis_E = set.currentMillis;
      if (set.Led_Emergency_state == true)
      {
        set.Led_Emergency_state = false;
        strip_FR.setBrightness(BRIGHTNESS_MAX); 
        strip_BR.setBrightness(BRIGHTNESS_MAX);
        strip_FL.setBrightness(BRIGHTNESS_MAX); 
        strip_BL.setBrightness(BRIGHTNESS_MAX);
        strip_FR.fill(mYellow);  
        strip_BR.fill(mYellow);
        strip_FL.fill(mYellow);  
        strip_BL.fill(mYellow);  
        strip_FR.show();
        strip_BR.show(); 
        strip_FL.show();
        strip_BL.show(); 
      }
      else
      {
        set.Led_Emergency_state = true;
        //strip_FL.setBrightness(50);
        strip_FR.fill(mBlack); 
        strip_BR.fill(mBlack);
        strip_FL.fill(mBlack); 
        strip_BL.fill(mBlack);
        strip_FR.show();
        strip_BR.show(); 
        strip_FL.show();
        strip_BL.show();        
      }
    }
  }
  else //if (set.Key_left != true && set.Key_right != true) 
  Led_state_def();
/*----------------------------------------------------------------------------*/
}
/* END -----------------------------------------------------------------------*/
