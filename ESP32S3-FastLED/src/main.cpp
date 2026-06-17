#include <FastLED.h>

#define DATA_PIN 4       // DIN 接 GPIO4

void setup() {
    Serial.begin(115200);
    pinMode(DATA_PIN, OUTPUT);  // GPIO4 初始化为输出模式
}
bool state = true;  // 放到外面，跨循环保持状态

void loop() {
    // //每当i为偶数时，GPIO4置为高电平；当i为奇数时，GPIO4置为低电平
    // if (i % 2 == 0) {
    //     digitalWrite(DATA_PIN, HIGH);
    // Serial.printf("现在是第%d秒，GPIO4为高电平\n", i);
    // } 
    // else {
    //     digitalWrite(DATA_PIN, LOW);
    //     Serial.printf("现在是第%d秒，GPIO4为低电平\n", i);
    // }
    if(state){
        digitalWrite(DATA_PIN, HIGH);
        Serial.println("GPIO4为高电平");
    } 
    else {
        digitalWrite(DATA_PIN, LOW);
        Serial.println("GPIO4为低电平");
    }
    delay(20);
    state = !state;
}
