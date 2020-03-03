/*
  Скетч к проекту "Радужная лента на Digispark"
  Исходники на GitHub: https://github.com/slfl/Attiny-85-Rainbow-LED-Strip
  Автор: SLFL, 2020
  
   Версия 1.3
*/

// ---------------------------------- НАСТРОЙКИ ------------------------------------
#define PIN         0       // Пин подуключения ленты WS2812b
#define NUMPIXELS   95      // Количество ваших светиков в ленте
#define BRIGHTNESS  200     // Уровень яркости (1 / 255) – Пожалей ленту на 255!)
#define FOCUS       70      // Если увеличить, получим узкие пятна цветов, при уменьшении пятна шире (Уровень смешивания)
#define DELAY       4000    // Скорость смены цветов. Меньше - быстрее, больше - медленнее
#define DEBUG       0       // Отображение скорости FPS (белый пиксель), первый пиксель
#define WRAP        1       // Режим кольца. Если выключено, некоторые участки тухнут в ноль

// ------------------------------ ДЛЯ РАЗРАБОТЧИКОВ --------------------------------
#include <Adafruit_NeoPixel.h> // Подключаем библиотеку, для работы с нашей лентой

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
float spdr, spdg, spdb;
float offset;
 
#if DEBUG
 long nextms = 0;
 int pfps = 0, fps = 0;
#endif

// Функция действительного показателя слишком медленная, поэтому я создал приближение (только для x <0)
float myexp(float x) { return (1.0/(1.0-(0.634-1.344*x)*x)); }

// ------------------------------ ДЛЯ РАЗРАБОТЧИКОВ --------------------------------
void setup() {
  spdr = 1.0 + random(200) / 100.0;
  spdg = 1.0 + random(200) / 100.0;
  spdb = 1.0 + random(200) / 100.0;
  offset = random(10000) / 100.0;// Установить случайное смещение, чтобы пятна начинались в случайных местах
  strip.begin(); // Инициализация радуги
}

void loop() 
{
  long ms = millis();  // Использовать реальное время для пересчета положения каждого цветового пятна
  float m = offset + (float)ms/DELAY;  // Масштабируем время до значения
  m = m - 42.5*cos(m/552.0) - 6.5*cos(m/142.0);  // Добавить некоторую нелинейность
  float posr = 0.20 + 0.50*sin(m*spdr);  // Пересчитываем положение каждого пятна (измеряется по шкале от 0 до 1)
  float posg = 0.5 + 0.65*sin(m*spdg);
  float posb = 0.80 + 0.75*sin(m*spdb);
  for (int i=0; i<NUMPIXELS; i++)  // Теперь перебираем каждый пиксель и вычисляем его цвет
  {
    float ppos = (float)i / NUMPIXELS;
    float dr = ppos-posr;
    float dg = ppos-posg;
    float db = ppos-posb;
#if WRAP
    dr = dr - floor(dr + 0.5);
    dg = dg - floor(dg + 0.5);
    db = db - floor(db + 0.5);
#endif
    // Тут идет момент смешивания, прицип немного похож на размытие по Гауссу
    strip.setPixelColor(i,
      constrain(BRIGHTNESS*myexp(-FOCUS*dr*dr),0,BRIGHTNESS),
      constrain(BRIGHTNESS*myexp(-FOCUS*dg*dg),0,BRIGHTNESS),
      constrain(BRIGHTNESS*myexp(-FOCUS*db*db),0,BRIGHTNESS)
      );
  }
#if DEBUG
  fps++;
  // Сброс счетчика через 1сек.
  if (ms>nextms)
  {
    nextms = ms + 1000;
    fps = 0;
    pfps = fps;
  }
  strip.setPixelColor(pfps,BRIGHTNESS,BRIGHTNESS,BRIGHTNESS);  // Показать частоту кадров в секунду, установив один пиксель на белый
#endif

  strip.show();  // Выводим наше говницо на ленту)
}
