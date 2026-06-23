#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ========== 灯带参数 ==========
#define LED_PIN     4
#define NUM_LEDS    64
#define MATRIX_W    8
#define MATRIX_H    8
#define BRIGHTNESS  4

// 8x8 全矩阵区域
#define SUB_X       0
#define SUB_Y       0
#define SUB_W       8
#define SUB_H       8
#define SUB_SIZE    (SUB_W * SUB_H)  // 64

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

// ========== 工具函数 ==========
int xyToIndex(int row, int col) {
    return row * MATRIX_W + col;
}

// 将子区域 (0..SUB_W-1, 0..SUB_H-1) 映射到真实坐标并设置颜色
void setSubPixel(int x, int y, uint32_t color) {
    strip.setPixelColor(xyToIndex(SUB_Y + y, SUB_X + x), color);
}

void clearSubMatrix() {
    for (int y = 0; y < SUB_H; y++)
        for (int x = 0; x < SUB_W; x++)
            setSubPixel(x, y, 0);
}

// HSV → RGB 转换（简化版，hue 0-255）
uint32_t hsvColor(uint8_t hue, uint8_t sat = 255, uint8_t val = 255) {
    uint8_t r, g, b;
    uint8_t region = hue / 43;
    uint8_t remainder = (hue - (region * 43)) * 6;
    uint8_t p = (val * (255 - sat)) >> 8;
    uint8_t q = (val * (255 - ((sat * remainder) >> 8))) >> 8;
    uint8_t t = (val * (255 - ((sat * (255 - remainder)) >> 8))) >> 8;
    switch (region) {
        case 0: r = val; g = t;   b = p;   break;
        case 1: r = q;   g = val; b = p;   break;
        case 2: r = p;   g = val; b = t;   break;
        case 3: r = p;   g = q;   b = val; break;
        case 4: r = t;   g = p;   b = val; break;
        default:r = val; g = p;   b = q;   break;
    }
    return strip.Color(r, g, b);
}

// ========== 灯效函数 ==========
// 每个效果返回 true 表示还在运行，false 表示结束

// 1. 颜色扫描 — 从左到右逐列填充颜色
bool effectColorWipe(uint32_t color, int speedMs, unsigned long &timer, int &step) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();
    if (step < SUB_W) {
        for (int y = 0; y < SUB_H; y++)
            setSubPixel(step, y, color);
        step++;
        strip.show();
        return true;
    }
    step = 0;
    return false; // 完成一轮
}

// 2. 彩虹旋转
bool effectRainbow(int speedMs, unsigned long &timer, uint8_t &hue) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    for (int y = 0; y < SUB_H; y++) {
        for (int x = 0; x < SUB_W; x++) {
            uint8_t h = hue + (x + y) * 16;
            setSubPixel(x, y, hsvColor(h));
        }
    }
    strip.show();
    hue++;
    return true; // 无限循环
}

// 3. 剧院追逐 — 跑马灯效果
bool effectTheaterChase(uint32_t color, int speedMs, unsigned long &timer, int &step) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();
    for (int y = 0; y < SUB_H; y++) {
        for (int x = 0; x < SUB_W; x++) {
            if ((x + y + step) % 3 == 0)
                setSubPixel(x, y, color);
        }
    }
    strip.show();
    step++;
    return true;
}

// 4. 呼吸灯
bool effectBreathing(uint32_t color, int speedMs, unsigned long &timer, int &step, bool &up) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    uint8_t brightness = step;
    uint32_t dimColor = strip.Color(
        (uint8_t)((color >> 16 & 0xFF) * brightness / 255),
        (uint8_t)((color >> 8  & 0xFF) * brightness / 255),
        (uint8_t)((color       & 0xFF) * brightness / 255)
    );
    for (int y = 0; y < SUB_H; y++)
        for (int x = 0; x < SUB_W; x++)
            setSubPixel(x, y, dimColor);
    strip.show();

    if (up) {
        step += 3;
        if (step >= 255) { step = 255; up = false; }
    } else {
        step -= 3;
        if (step <= 0) { step = 0; up = true; return false; }
    }
    return true;
}

// 5. 随机闪烁（星星）
bool effectTwinkle(int speedMs, unsigned long &timer, uint16_t &count) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();
    for (int i = 0; i < 3; i++) {
        int x = random(SUB_W);
        int y = random(SUB_H);
        setSubPixel(x, y, hsvColor(random(255)));
    }
    strip.show();
    count++;
    return true;
}

// 6. 贪吃蛇
bool effectSnake(int speedMs, unsigned long &timer, int &pos, int &direction, int &len) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();

    int total = SUB_SIZE * 2; // 来回一圈
    int p = pos % total;
    int idx;
    if (p < SUB_SIZE) idx = p;
    else              idx = total - 1 - p;

    int x = idx % SUB_W;
    int y = idx / SUB_W;

    // 画尾巴
    for (int i = 0; i < len; i++) {
        int tailP = (pos - i + total) % total;
        int tailIdx;
        if (tailP < SUB_SIZE) tailIdx = tailP;
        else                  tailIdx = total - 1 - tailP;
        int tx = tailIdx % SUB_W;
        int ty = tailIdx / SUB_W;
        uint8_t bright = 255 - i * 50;
        setSubPixel(tx, ty, strip.Color(bright, bright, bright));
    }
    strip.show();
    pos++;
    return true;
}

// 7. 彗星拖尾
bool effectComet(uint32_t color, int speedMs, unsigned long &timer, float &pos, int &dir) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();

    int x = (int)pos % SUB_W;
    int y = ((int)pos / SUB_W) % SUB_H;

    // 拖尾长度 3
    for (int t = 0; t < 3; t++) {
        int trailP = (int)pos - t * dir;
        if (trailP < 0 || trailP >= SUB_SIZE) continue;
        int tx = trailP % SUB_W;
        int ty = trailP / SUB_W;
        uint8_t dim = 255 - t * 85;
        setSubPixel(tx, ty, strip.Color(
            (uint8_t)((color >> 16 & 0xFF) * dim / 255),
            (uint8_t)((color >> 8  & 0xFF) * dim / 255),
            (uint8_t)((color       & 0xFF) * dim / 255)
        ));
    }
    strip.show();

    pos += dir * 0.5f;
    if (pos >= SUB_SIZE - 0.5f) { pos = SUB_SIZE - 1; dir = -1; }
    if (pos <= 0)                { pos = 0;         dir =  1; }
    return true;
}

// 8. 中心扩散波纹
bool effectRipple(int speedMs, unsigned long &timer, int &radius) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();
    float cx = (SUB_W - 1) / 2.0f;
    float cy = (SUB_H - 1) / 2.0f;
    for (int y = 0; y < SUB_H; y++) {
        for (int x = 0; x < SUB_W; x++) {
            float dist = sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy));
            if (fabsf(dist - radius) < 0.6f)
                setSubPixel(x, y, hsvColor(radius * 20));
        }
    }
    strip.show();
    radius++;
    if (radius > 7) { radius = 0; return false; }
    return true;
}

// 9. 弹跳像素
bool effectBounce(int speedMs, unsigned long &timer, float &bx, float &by, float &vx, float &vy) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    clearSubMatrix();
    setSubPixel((int)bx, (int)by, strip.Color(255, 255, 255));
    strip.show();

    bx += vx;
    by += vy;
    if (bx <= 0 || bx >= SUB_W - 1) vx = -vx;
    if (by <= 0 || by >= SUB_H - 1) vy = -vy;
    return true;
}

// 10. 颜色渐变填充（棋盘格交替）
bool effectCheckerboard(int speedMs, unsigned long &timer, uint8_t &hueOffset) {
    if (millis() - timer < (unsigned long)speedMs) return true;
    timer = millis();
    for (int y = 0; y < SUB_H; y++) {
        for (int x = 0; x < SUB_W; x++) {
            if ((x + y) % 2 == 0)
                setSubPixel(x, y, hsvColor(hueOffset));
            else
                setSubPixel(x, y, hsvColor(hueOffset + 128));
        }
    }
    strip.show();
    hueOffset += 2;
    return true;
}

// ========== 主程序 ==========
void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("\n=================================");
    Serial.println("  8x8 LED Effects Demo");
    Serial.println("=================================");

    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.clear();
    strip.show();

    randomSeed(analogRead(0));
}

// 效果枚举
enum Effect {
    EF_COLOR_WIPE,
    EF_RAINBOW,
    EF_THEATER_CHASE,
    EF_BREATHING,
    EF_TWINKLE,
    EF_SNAKE,
    EF_COMET,
    EF_RIPPLE,
    EF_BOUNCE,
    EF_CHECKERBOARD,
    EF_COUNT
};

void loop() {
    static int currentEffect = 0;
    static unsigned long effectStart = millis();
    static const unsigned long EFFECT_DURATION = 5000; // 每个效果运行 5 秒

    // 每个效果的持久状态
    static unsigned long frameTimer = 0;
    static int step = 0;
    static uint8_t hue = 0;
    static bool breathUp = true;
    static uint16_t twinkleCount = 0;
    static int snakePos = 0, snakeDir = 1, snakeLen = 4;
    static float cometPos = 0; static int cometDir = 1;
    static int rippleRadius = 0;
    static float bx = 1.5, by = 1.5, vx = 0.3, vy = 0.2;
    static uint8_t checkerHue = 0;

    bool running = true;

    switch (currentEffect) {
        case EF_COLOR_WIPE: {
            static uint32_t wipeColors[] = {
                strip.Color(255,0,0), strip.Color(0,255,0), strip.Color(0,0,255),
                strip.Color(255,255,0), strip.Color(255,0,255), strip.Color(0,255,255)
            };
            static int colorIdx = 0;
            running = effectColorWipe(wipeColors[colorIdx], 80, frameTimer, step);
            if (!running) { colorIdx = (colorIdx + 1) % 6; }
            Serial.printf("[ColorWipe] color=%d step=%d\n", colorIdx, step);
            break;
        }
        case EF_RAINBOW:
            running = effectRainbow(40, frameTimer, hue);
            break;
        case EF_THEATER_CHASE:
            running = effectTheaterChase(strip.Color(255, 100, 0), 100, frameTimer, step);
            break;
        case EF_BREATHING: {
            static uint32_t breathColor = strip.Color(0, 180, 255);
            running = effectBreathing(breathColor, 8, frameTimer, step, breathUp);
            if (!running) breathColor = hsvColor(random(255));
            break;
        }
        case EF_TWINKLE:
            running = effectTwinkle(60, frameTimer, twinkleCount);
            break;
        case EF_SNAKE:
            running = effectSnake(100, frameTimer, snakePos, snakeDir, snakeLen);
            break;
        case EF_COMET: {
            static uint32_t cometColor = strip.Color(255, 60, 0);
            running = effectComet(cometColor, 50, frameTimer, cometPos, cometDir);
            break;
        }
        case EF_RIPPLE:
            running = effectRipple(200, frameTimer, rippleRadius);
            break;
        case EF_BOUNCE:
            running = effectBounce(50, frameTimer, bx, by, vx, vy);
            break;
        case EF_CHECKERBOARD:
            running = effectCheckerboard(50, frameTimer, checkerHue);
            break;
    }

    // 切换效果
    if (millis() - effectStart > EFFECT_DURATION) {
        currentEffect = (currentEffect + 1) % EF_COUNT;
        effectStart = millis();
        frameTimer = 0;
        step = 0;
        hue = 0;
        breathUp = true;
        twinkleCount = 0;
        snakePos = 0;
        cometPos = 0; cometDir = 1;
        rippleRadius = 0;
        bx = 1.5; by = 1.5; vx = 0.3; vy = 0.2;
        checkerHue = 0;

        Serial.printf("\n--- Switching to Effect #%d ---\n", currentEffect);
        const char* names[] = {
            "Color Wipe", "Rainbow", "Theater Chase", "Breathing",
            "Twinkle", "Snake", "Comet", "Ripple", "Bounce", "Checkerboard"
        };
        Serial.printf("Effect: %s\n", names[currentEffect]);
    }
}
