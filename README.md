# 项目学习日志

## 6.30
**6.30.1动画效果**  
*文档:color-theory.md*  

实现可配置的动画变速效果

```cpp
uint8_t animationSpeed = 32;  // 每次起始色相的改变值

void loop() {
    EVERY_N_MILLISECONDS(200) {
	static uint8_t hue = 0;
	
	// 后两个参数分别起始色相与灯珠间色相步进值
	fill_rainbow(leds, NUM_LEDS, hue, 2);
	FastLED.show();
	
	hue += animationSpeed;  // 改变起始色相
    }
}
```

Time-Based Animation

## 6.29
做了一个适配我的8\*8之字形矩阵LED的取模得字符数组的小网页

**6.29.1 FastLED的颜色模型**  *_(尽可能只使用一个颜色模型)_*   
*文档:color-theory.md*  

HSV转换至RGB在FastLED项目中可以由库函数自动进行，但是RGB到HSV必须手动进行操作，尽量在一开始就选择HSV

arduino中有一个map函数可以实现数值映射  
```返回值类型 map(要映射的值,  原始范围下限, 原始范围上限, 目标下限, 目标上限)```

**6.29.2 FastLED的时序与帧率**  
*文档:timing.md*  

帧率计算：帧率 = 1000ms/刷新间隔时间

帧率测量：使用```FastLED.getFPS()```通过监控show函数，跟踪实时帧率，返回一个uint16_t类型的fps值，使用方式如下：

```cpp
void loop() {
	updateLEDs();
	FastLED.show();
	// 串口输出当前FPS
	EVERY_N_SECONDS(1) {
	uint16_t fps = FastLED.getFPS();
	Serial.print("FPS: ");
	Serial.println(fps);
    }

	delay(20);
}
```

最基础的延时方式是使用```delay() ```设置ms延时，但是会造成阻塞

```FastLED.delay()```为FastLED库自己的延迟函数，相较于```delay()```内置处理了抖动，可以获得更好的颜色渲染效果

也可以使用FastLED内置的两种宏实现非阻塞式时序
EVERY_N_MILLISECONDS Macro，毫秒宏，设置精度为毫秒，好处是可以非阻塞，但是精度与灵活度不如自定义时序，使用方式如下:

```cpp
void loop() {
	//毫秒宏
	// 每隔20ms执行一次，帧率为1000/20 = 50 FPS
	EVERY_N_MILLISECONDS(20) 
	{  
      updateLEDs();
      FastLED.show();
      }
      
	// 其余代码可以正常运行
      checkButtons();
      readSensors();
}
```

EVERY_N_SECONDS Macro，秒宏，设置的间隔单位为秒，方式与毫秒宏相似，适用于较慢更新

自定义非阻塞时序，使用```millis()```实现完全控制, 虽然控制精准，但是需要管理更多代码使用方式如下:

```cpp
uint32_t lastUpdate = 0;				// 定义最近更新时间
const uint32_t updateInterval = 20;	// 定义更新间隔为20ms，50 FPS

void loop() {
	uint32_t now = millis();			// 获取当前时间

	// 当前时间距离上次更新已经到达间隔
	if (now - lastUpdate >= updateInterval) {
		lastUpdate = now;			//当前时间设为最近更新时间

		updateLEDs();
		FastLED.show();
    }

    // 其他代码正常运行
    checkButtons();
}
```

多时序间隔与自定义非阻塞时序类似，为每个时序单独设置时间变量：上次更新时间```LastUpdate```与间隔```Interval```


## 6.26

第一个LED闪烁程序比较简单，简要总结一下其中的函数

**6.26.1库中的一些基本函数**
```FastLED.setBrightness(scale)```将亮度设置缩放为scale/256  
实际输出亮度=原始亮度*（scale/256）  
scale为8位无符号整数，如果传给scale的值不在范围内会自动对256取模

```FastLED.show()```将初始化时注册在```FastLED.addLeds```中的数组的值更新至灯带中  
可以进行无参调用直接更新，或者带参调用传入scale参数 对本次画面的亮度进行缩放，规则与```FastLED.setBrightness(scale)```相同

```fill_solid(leds, NUM_LEDS, CRGB::Red)```将leds数组对应的NUM_LEDS数量的 LED 灯填充为一种固体颜色  
该函数存在于自由函数与```CPixelView```类中

```
fill_solid(&leds[10], 10, CRGB::Green);		//将10-19对应led填充至绿色
```
  
进入第二阶段“core-concepts”，首先学习“led-structures”理解LED数据结构  
**6.26.2LED的数据结构**  

FastLED 中最基本的数据结构是 `CRGB` 类数组：

```cpp
CRGB leds[NUM_LEDS];  // RGB 颜色值数组
```

数组中的每个元素表示对应编号led的CRGB值，通过索引可访问对应的单个LED  

```CRGB```类有三个成员对应R、G、B三个*颜色通道*  
对LEDs元素赋值可以使用命名颜色```CRGB::Red```或者直接创建CRGB对象```CRGB(255, 0, 0)```或者创建CHSV对象```CHSV(0, 255, 255)```自动转换为CRGB，转换算法名称为hsv2rgb_rainbow() (图像处理的算法，有点复杂，没看懂)


对于```CRGB```类中的*颜色通道*也可以进行单独操作或者数组操作

单独操作：  

```
RGB color = CRGB::Purple;	//创建颜色
color.r += 50;				// 增加红色
color.g /= 2;				// 绿色减半
color.b = 0;				// 去除蓝色
```

数组操作

```cpp
leds[i].r += 10;  // 对i对应的LED的红色值增加
```

CRGB值之间可以进行数学运算

```cpp
CRGB color1 = CRGB(100, 50, 25);
CRGB color2 = CRGB(50, 100, 25);

// 颜色间叠加
CRGB result = color1 + color2;  // (150, 150, 50)

// 颜色进行放缩
color1 *= 2;  // 所有颜色通道的值翻倍:(200, 100, 50)
color1 /= 2;  // 所有颜色通道值减半:(50, 25, 12)

// 改变各颜色通道的值
leds[0] += CRGB(10, 0, 0);  // 增加红色
```

**6.26.3考虑内存开销**  
每个内存占用3B内存

**6.26.4范围处理**  

使用```fill_solid()```对范围内的颜色进行处理

```cpp
// 10-19范围填充
fill_solid(&leds[10], 10, CRGB::Green);

// 使用指针运算
CRGB* startPos = &leds[20];
fill_solid(startPos, 15, CRGB::Blue);  // LEDs 20-34
```

将一个范围的颜色复制到另一个范围

```cpp
for (int i = 0; i < 10; i++) {
    leds[i + 20] = leds[i];  // 复制LEDs 0-9 至 20-29
}
```

**6.26.5经典LED绘制技术**  
以下是一些基础的LED绘制效果，需要理解这些是如何实现的，大多数库中都没有

```cpp
//传递LED位置与颜色，设置单个像素函数
void drawPixel(int position, CRGB color) {
	if (position >= 0 && position < NUM_LEDS) {
	leds[position] = color;
    }
}

// 使用方式
drawPixel(10, CRGB::Red);
drawPixel(15, CRGB(0, 255, 128));
```

```cpp
//传递起点与终点与颜色，通过设置连续的 LED 来创建线条
void drawLine(int start, int end, CRGB color) {
	if (start > end) {		//使线条无向
	int temp = start;
	start = end;
	end = temp;
    }
    for (int i = start; i <= end && i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}

// 从 LED 5 至 LED 20 画出红色线条
drawLine(5, 20, CRGB::Red);
```

🌟🌟🌟坐标映射

```cpp
//定义矩阵长宽
#define MATRIX_WIDTH 4
#define MATRIX_HEIGHT 4

/*
3	2	1	0//第1行	 ^
4	5	6	7				|
11	10	9	8		HEIGHT
12	13	14	15			|
<---WIDTH--->		 v
*/

//给一对x，y值，返回其对应的led位置
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

//在X=5, Y=10位置处画像素
leds[XY(5, 10)] = CRGB::Blue;

//画一条水平线
for (int x = 0; x < MATRIX_WIDTH; x++) {
    leds[XY(x, 8)] = CRGB::Green;
}
```

```cpp
//多使用颜色结合，少使用颜色赋值（应该是为了平滑过度）

// 使用加法混合变亮
leds[i] += CRGB(20, 20, 20);

// 平均混合颜色
CRGB color1 = leds[i];
CRGB color2 = CRGB::Blue;
// blend(颜色1，颜色2，颜色2所占比例(256为100%))
leds[i] = blend(color1, color2, 128); //原色与蓝色进行55混合
```

```cpp
// 实现Larson Scanner，制作逐渐消失的拖尾
//将所有LED变暗，再将新出现的点涂亮
void larsonScanner() {
    static int pos = 0;
    static int direction = 1;

    // 淡出所有LEDs
    //将leds数组中的NUM_LEDS个灯的亮度降低20/256
    fadeToBlackBy(leds, NUM_LEDS, 20);

    // 绘制新的亮点
    leds[pos] = CRGB::Red;

    // 移动至下一个位置
    pos += direction;
    if (pos >= NUM_LEDS - 1 || pos <= 0) {
        direction = -direction;
    }
}
```
matrix.md里面还有其他的更高阶的空间映射

## 6.25 

今天浪费了一会儿在纠结于```CFastLED```，后面发现了项目自带有一个cookbook文件夹，里面详细记载了不同阶段应该学什么，今天学习第一阶段“getting-started”中的“concepts”，学习内容如下

**6.25.1 单个LED的寻址与控制**  
灯带中每个LED都有一个索引，这个索引也就是位置，从下标0开始，通过控制灯带数组中的颜色值控制LED，颜色值的类型为CRGB  

```cpp
CRGB leds[60];  			 //一个含有64个LED的灯带

leds[0] = CRGB::Red;    	// 第一个LED 设置为红色
leds[5] = CRGB::Blue;   	// 第六个LED 设置为蓝色
```


**6.25.2 RGB与HSV颜色模型**  

FastLED有两种颜色模式，分别为RGB与HSV，两种颜色模式分别有各自的cpp类```CRGB```和```CHSV```

*RGB：*

```cpp
//RGB模式通过分别控制红色R，绿色G，蓝色B，实现控制灯的颜色
//每个颜色的取值范围分别为0-255
CRGB color_Red = CRGB(255, 0, 0);      // Red
CRGB color_Green = CRGB(0, 255, 0);		// Green
CRGB color_Blue = CRGB(0, 0, 255);		// Blue
CRGB color_White = CRGB(255, 255, 255);	// White
```
*HSV:*

```cpp
//HSV模式通过分别控制色调Hue，饱和度Saturation，明度Value（感觉和亮度没区别），实现控制灯的颜色
//每个值的取值范围与RGB为0-255
//色调Hue，对应的颜色范围为：red→ orange → yellow → green → blue → purple → red
//饱和度Saturation，对应的颜色强度为：		0：白色，255：纯色
//明度Value，对应亮度为：					0：熄灭，255：最亮
CHSV hsv_Red = CHSV(0, 255, 255);      		// Red
CHSV hsv2_Green = CHSV(96, 255, 255);     	// Green
CHSV hsv3_Blue = CHSV(160, 255, 255);    	// Blue

// 自动转化至RGB
leds[0] = hsv_Red;
```

文档建议使用HSV，因为可以只通过改变色调Hue实现丝滑的颜色过渡实现彩虹效果和更简单的亮度控制

**6.25.3 程序的基本setup与loop模式**  
文档规定每个FastLED程序应该遵循：

```cpp
//配置LED
void setup() {
    // 1. 初始化FastLED
    
    //告诉程序所用的灯珠芯片LED_TYPE、所接引脚DATA_PIN、以及颜色顺序RGB/GRB，将定义的 LED 数组注册给程序管理。
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
    
    //设置LED亮度
    FastLED.setBrightness(50);
}

void loop() {
    // 2. 计算/更新 LED 颜色
    leds[0] = CRGB::Red;

    // 3. 将颜色显示到硬件上，只有调用FastLED.show()才能更新物理LED
    FastLED.show();

    // 4. 可选: 延时或设置帧率
    delay(20);
}
```

**6.25.4 如何为特定硬件配置FastLED**  
配置LED，在```setup()```前告诉程序硬件信息：

```cpp
#define LED_PIN     5			// 数据引脚
#define NUM_LEDS    60			// LED灯珠数量
#define LED_TYPE    WS2812B     // LED灯带的控制芯片
#define COLOR_ORDER	GRB	// 颜色模式

CRGB leds[NUM_LEDS];			//创建一个灯珠数组，记载每个灯珠的颜色
```

**6.25.5 亮度控制与静态变量**  
亮度控制:```FastLED.setBrightness(50);```亮度值的范围为0-255  
使用```static```变了可以在每次loop循环中，记住变量的值


## 6.23 请，一定要共地

今天整了一下午这个灯带，限制不住亮度，也试了限制区域，始终是所有灯珠都在乱闪，已经陷入绝境走投无路了，以为是因为电流电压不够，这几天每天忙着买材料，人都已经麻了，准备放弃这个项目去做另一个智能家居的项目了，和AI聊天的过程中突然发现灯带和ESP32需要共地，一开始还觉得共地应该不会是影响乱闪的决定因素，因为乱闪不仅乱，而且亮，非常的亮，抱着试一试的想法，进行了共地！！！结果灯带居然正常了，然后把共地的线一拔，又乱了！居然真的是共地的问题，我折腾了一下午啊😭😭😭😭！！！请，一定要共地！！！  
而后用ai写了一波测试程序，效果非常完美，现在将完整的测试程序上传

## 6.17

非常难受，DC电源和压线端子都没到，然后发现一块面包板的宽度不够插S3，又下单了一块开发板，并且拓展坞接线不良也要紧急下单，估计又要等几天才能开始了😭（预计端午期间不会搞项目），今天看课学了点arduino的基础操作函数，试了下最小系统板的5Vin端口，确实有电压，但是非常微弱（与3.3V比起），视图点亮灯带但是无论如何都点不亮，这个裸线真的很难办，不知道后面还缺什么材料，项目无进展  
然后做了一下算法题

## 6.16

货还没到，今天配置了一下软件环境，在arduino（阿杜依诺🤓）和PlatformIO中选择了后者，因为编译速度和项目构建便利上后者有显著优势，然后把FsatLED开源库给下好了，花了些时间了解一下大致啥情况，准备一个月内完成（希望吧），B站上找了一下，有个课真不错（BV1b441177VF）好多年前的老视频，UP现在已经停更，非常的互联网精神🫡纯分享，计划结合这个视频进行学习，然后找ai做了一份学习计划供参考

## 6.15

暑假闲来无事，想做个项目玩玩，顺便提升一下自己的技术，今天选题选了一天，看见github上star数多的一个项目，有两个项目比较好，第一个项目就是这个项目FastLED，目前还没想好到底要做出来什么产品，但是觉得比较有意思，所以先把材料买了，找AI做了个项目学习报告，理清项目。第二个项目暂定是ESPHome，先把第一个做完再看情况，不知道暑假能不能做的完

主控用ESP32-S3-N16R8，选这个芯片主要是看中它性能比较好，网上有个小智的小机器人都用的他，但是感觉这个东西做的人太多了，也不够新，所以没做这个，但是以后如果有ai方面的需求好像也能用这个，所以买了这个

灯带选的就是最多人用的WS2812B，本来想选8\*16，想着大点的屏幕可以花活多一点😂但是问了AI，说灯珠多了电源要求比较高，遂放弃，最终选择了8\*8
