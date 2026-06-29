#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     4      // 灯带数据线接到哪个 GPIO 引脚
#define NUM_LEDS    64     // 灯带上总共有多少颗 LED
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
CRGB leds[NUM_LEDS];

uint16_t XY(uint8_t x, uint8_t y) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0;

    //最常见的布线方式是蜿蜒的之字形布局
    if (y & 0x01) {
        //第奇数行从后向前走
        return (y * MATRIX_WIDTH) + (MATRIX_WIDTH - 1 - x);
    } else {
        //第偶数行从前向后走
        return (y * MATRIX_WIDTH) + x;
    }
}

void ShowFunc(bool char1[64]) {
    CRGB color1 = CRGB(255, 0, 0); // 定义颜色 RGB1
    for(int i = NUM_LEDS - 1; i >= 0; i--) {
        if (char1[i]) {
            leds[i] = color1;       // 对应的 LED 点亮
        } else {
            leds[i] = CRGB::Black;  // 对应的 LED 熄灭
        }
        FastLED.show();             // 实时刷新
        delay(50);                  // 延时 50ms
        
        //color1随机刷新
        color1 = CRGB(random(0, 256), random(0, 256), random(0, 256));
    }
    FastLED.show();
}

void ShowFunc_blink(bool char1[64]) {
    CRGB color1 = CRGB(255, 0, 0); // 定义颜色 RGB1
    while(1)
    {
        for(int i = NUM_LEDS - 1; i >= 0; i--) {
            if (char1[i]) {
                leds[i] = color1;       // 对应的 LED 点亮
            } else {
                leds[i] = CRGB::Black;  // 对应的 LED 熄灭
            }
        }
        FastLED.show();             // 实时刷新
        delay(500);                  // 延时 500ms

        //color1随机刷新
        color1 = CRGB(random(0, 256), random(0, 256), random(0, 256));
    }
    
}

void setup() {
    /**
    - GRB:       颜色字节顺序, WS2812B 大多数是 GRB;
     *           如果颜色不对, 试试 RGB
     *           此处定义为GRB时，实际显示变为RGB
     *               定义为RGB时，实际显示变为GRB
     *               未定义时实际显示为GRB，因为实际用RGB更多些，所以还是在此处定义为GRB
     */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

    //设置全局亮度上限 (0~255)。
    FastLED.setBrightness(16);
}

bool tag = true;//控制首次循环逐个点亮，而后闪烁
uint8_t animationSpeed = 32;  // Adjustable speed

void loop() {

/***************每秒钟向下移动两行灯带y-2**********************************************/

     EVERY_N_MILLISECONDS(200) {
        static uint8_t hue = 0;

        fill_rainbow(leds, NUM_LEDS, hue, 2);  // Fill the LED array with a rainbow pattern
        FastLED.show();

         hue += animationSpeed;  // Speed controls how fast hue changes
    }
    

/**************************************************************************/
    // leds[0] = CRGB(255, 64, 32);    
    // FastLED.show();                 //刷新显示



/****************************RGB与HSV的转换*******************************************/

    // CHSV color1 = CHSV(0, 255, 255);                       //定义颜色 HSV1
    // leds[XY(0, 0)] = color1; // 将第一个 LED 设置为 HSV1
    // CRGB color2 = color1; // 将 HSV1 转换为 RGB

/**************************************************************************/
    //张
    // bool char1[64] = {0,1,1,0,1,1,0,0,
    //                   1,0,0,1,0,1,0,0,
    //                   0,0,1,0,1,0,1,0,
    //                   0,0,1,1,0,1,1,1,
    //                   1,0,0,1,1,1,1,1,
    //                   0,0,1,1,0,1,1,1,
    //                   0,0,1,0,1,0,1,0,
    //                   1,0,0,1,0,1,1,1};
    // if (tag) {
    //     ShowFunc(char1);
    //     tag = false;
    // } else {
    //     ShowFunc_blink(char1);
    // }


/**************************************************************************/
    // CRGB color1 = CRGB(255, 0, 0);                       //定义颜色 RGB1
    // FastLED.showColor(color1);                          //将所有led填充至自定义颜色
    // delay(500);                                         //延时500ms


    // CRGB color2 = CRGB(0, 255, 0);                       //定义颜色 RGB2
    // FastLED.showColor(color2);                          //将所有led填充至自定义颜色
    // delay(500);                                         //延时500ms

    // CRGB color3 = color1 + color2;                                         //定义颜色 RGB3
    // FastLED.showColor(color3);                          //将所有led填充至自定义颜色
    // delay(2000);                                         //延时2000ms



/**************************************************************************/


    // fill_solid(&leds[10], 10, CRGB::Green);     //将10-19对应led填充至绿色
    // FastLED.show();                             //刷新显示
    // delay(500);                                 //延时500ms
    // fill_solid(&leds[10], 10, color);     //将10-19对应led填充至自定义颜色
    // FastLED.show();                             //刷新显示
    // delay(500);                                 //延时500ms
}
