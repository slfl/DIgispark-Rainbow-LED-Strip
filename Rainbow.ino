// Rainbow LED Strip
// By Mansi
// v 1.0

#include <Adafruit_NeoPixel.h>

#define PIN         4     // Пин подуключения ленты WS2812b
#define RNDPIN      2     // Аналоговый пин входа (Для рандомного генератора???)
#define NUMPIXELS   95    // Количество ваших светиков в ленте
#define BRIGHTNESS  255   // Уровень яркости (1 / 255) – Пожалей ленту на 255!)
#define FOCUS       65    // Если увеличить, получим узкие пятна цветов, при уменьшении пятна шире (Уровень смешивания)
#define DELAY       5000  // Уменьшить, чтобы ускорить, увеличить, чтобы замедлить (на самом деле это не задержка)
#define DEBUG       0     // Ставим 1 для отображения скорости FPS (белый пиксель)
#define WRAP        1     // Если 1, обернуть цветную волну по краю (используется для круглых полос)


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
float spdr, spdg, spdb; // У нас есть 3 цветных пятна (красный, зеленый, синий), колеблющихся вдоль полосы с разными скоростями
float offset;
  #if DEBUG // track fps rate
    long nextms = 0;
    int pfps = 0, fps = 0;
  #endif

float myexp(float x) // Функция действительного показателя слишком медленная, поэтому я создал приближение (только для x <0)
{
  return (1.0/(1.0-(0.634-1.344*x)*x));
}

void setup() {
  randomSeed(analogRead(RNDPIN)); // Инициализировать генератор псевдослучайных чисел с некоторым случайным значением
  spdr = 1.0 + random(200) / 100.0;
  spdg = 1.0 + random(200) / 100.0;
  spdb = 1.0 + random(200) / 100.0;
  offset = random(10000) / 100.0;// Установить случайное смещение, чтобы пятна начинались в случайных местах
  strip.begin(); // Инициализация радуги
  strip.show();
}

void loop() 
{
  long ms = millis(); // Использовать реальное время для пересчета положения каждого цветового пятна
  float m = offset + (float)ms/DELAY;  // Масштабируем время до значения
  m = m - 42.5*cos(m/552.0) - 6.5*cos(m/142.0);  // Добавить некоторую нелинейность
  float posr = 0.15 + 0.55*sin(m*spdr);  // Пересчитываем положение каждого пятна (измеряется по шкале от 0 до 1)
  float posg = 0.5 + 0.65*sin(m*spdg);
  float posb = 0.85 + 0.75*sin(m*spdb);
  for (int i=0; i<NUMPIXELS; i++)   //  Теперь перебираем каждый пиксель и вычисляем его цвет
  {
    float ppos = (float)i / NUMPIXELS;    // Положение пикселя по шкале от 0.0 до 1.0
    float dr = ppos-posr;    // Расстояние от этого пикселя до центра каждого цветового пятна
    float dg = ppos-posg;
    float db = ppos-posb;
#if WRAP
    dr = dr - floor(dr + 0.5);
    dg = dg - floor(dg + 0.5);
    db = db - floor(db + 0.5);
#endif
    // Установите каждый компонент цвета от 0 до максимальной ЯРКОСТИ в соответствии с распределением Гаусса
    strip.setPixelColor(i,
      constrain(BRIGHTNESS*myexp(-FOCUS*dr*dr),0,BRIGHTNESS),
      constrain(BRIGHTNESS*myexp(-FOCUS*dg*dg),0,BRIGHTNESS),
      constrain(BRIGHTNESS*myexp(-FOCUS*db*db),0,BRIGHTNESS)
      );
  }
#if DEBUG  // Смотрим за скоростью FPS
  fps++;
  if (ms>nextms)     // 1 секунда прошла - сброс счетчика
  {
    nextms = ms + 1000;
    pfps = fps;
    fps = 0;
  }
  strip.setPixelColor(pfps,BRIGHTNESS,BRIGHTNESS,BRIGHTNESS);  // Показать частоту кадров в секунду, установив один пиксель на белый
#endif
  strip.show();  // Выводим наше говницо на ленту)
}
