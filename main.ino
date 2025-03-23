#include <Wire.h>            // Библиотека для работы UART
#include <Adafruit_BMP280.h> // Библиотека для работы модуля BMP280
#include <MPU6050_light.h>

Adafruit_BMP280 bmp; // Объявляем переменную для датчика BMP280
MPU6050 mpu(Wire);   // Объявляем переменную для датчика MPU9250(MPU6050)

float startPressure;
float startAltitude;

int altitudeRounded;

void setup() {
    Serial.begin(9600); // Включаем отладочный вывод
    
    if (!bmp.begin(0x76)) {
        Serial.println(F("ERROR.BMP280"));  // Проверяем работоспособность BMP280
        while (1);
    }

    byte status = mpu.begin();
    if (status != 0) {
        Serial.print("MPU6050 ERROR: ");    // Проверяем работоспособность MPU9250(MPU6050)
        Serial.println(status);
        while (1);
    }
    
    mpu.calcOffsets(); // Автоматическая калибровка
    
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Режим работы
                    Adafruit_BMP280::SAMPLING_X2,     // Точность измерения температуры
                    Adafruit_BMP280::SAMPLING_X1,     // Точность измерения давления
                    Adafruit_BMP280::FILTER_X16,      // Уровень фильтрации
                    Adafruit_BMP280::STANDBY_MS_500); // Время ожидания между измерениями

    delay(1000); // Ждём стабилизации датчика

    // Считаем начальные значения
    startPressure = bmp.readPressure() / 100.0;      // Давление в hPa
    startAltitude = bmp.readAltitude(startPressure); // Высота относительно начального давления

    while (!Serial) {
        ;
    }
    Serial.println(F("[RocketBoard-25.03.23.1-MahcinKirill]"));
}

void loop() {
    mpu.update();
    
    float temperature = bmp.readTemperature();                  // Температура
    float pressure = bmp.readPressure() / 100.0;                // Давление в hPa
    float currentAltitude = bmp.readAltitude(startPressure);    // Высота относительно стартового давления
    float relativeAltitude = currentAltitude - startAltitude;   // Разница высоты (измерение от старта)
    
    if (relativeAltitude > 0.0) {
       altitudeRounded = floor(relativeAltitude); 
    }                                             // Округление высоты до метров
    else {
       altitudeRounded = ceil(relativeAltitude);
    }

    if (pressure > startPressure) {
       startPressure = pressure; 
    }                                             
    
    Serial.print(F("Temp: "));
    Serial.print(temperature);         // Вывод температуры в °C
    Serial.print(F(" °C, "));

    Serial.print(F("Pres: "));
    Serial.print(pressure);            // Вывод давления в hPa
    Serial.print(F(" hPa, "));

    Serial.print(F("Alt: "));
    Serial.print(altitudeRounded);     // Вывод высоты (в метрах(очень условных и относительных))
    Serial.print(F(" m, "));
    
    Serial.print(F("Roll: "));
    Serial.print(mpu.getAngleX());     // Крен (наклон влево-вправо)
    Serial.print(F("°, "));
    
    Serial.print(F("Pitch: "));
    Serial.print(mpu.getAngleY());     // Тангаж (наклон вперёд-назад)
    Serial.println(F("°"));

    delay(250);
}
