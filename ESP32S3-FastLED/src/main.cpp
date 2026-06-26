#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     4      // 灯带数据线接到哪个 GPIO 引脚
#define NUM_LEDS    64     // 灯带上总共有多少颗 LED

CRGB leds[NUM_LEDS];

void setup() {
    /**
     * FastLED.addLeds<芯片型号, 数据引脚, 颜色顺序>(数组名, LED总数)
     * 来自 <FastLED.h>
     *
     * - WS2812B:   你的灯珠芯片型号
     * - LED_PIN:   数据引脚 (GPIO 4)
     * - GRB:       颜色字节顺序, WS2812B 大多数是 GRB;
     *              如果颜色不对, 试试 RGB
     * - leds:      存颜色的数组
     * - NUM_LEDS:  灯珠总数
     */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

    /**
     * 设置全局亮度上限 (0~255)。
     * 即使你写 leds[i] = CRGB::Red (满红),
     * 实际输出也会被限制到这个亮度。
     * 值越小越省电, 对眼睛也友好。
     *
     * setBrightness() 来自 <FastLED.h>
     */
    FastLED.setBrightness(32);
}

void loop() {
    //设置前10个灯珠循环增加H值
    static uint8_t hue = 0;             // 色相值
    static uint8_t value = 0;             // 明度
    static uint8_t saturation = 0;        // 饱和度
    static int i = 0;                    // 当前灯珠索引
    CHSV hsvColor = CHSV(hue, saturation, value + 16); // 初始颜色为红色
    leds[i] = hsvColor;                 // 设置颜色
    hue += 2;                              // 色相值增加, 颜色会变化
    hue %= 256;                         // 保证 hue 在 0~255 范围内循环
    ++i;                                // 下一个灯珠
    i %= 64;                            // 保证 i 在 0~63 范围内循环
    value += 2;
    value %= 64;
    saturation += 2;
    saturation %= 130;
    fill_solid(leds, NUM_LEDS, hsvColor); // 将所有灯珠设置为当前颜色
    FastLED.show();
    delay(50); // 延迟 100ms
}
