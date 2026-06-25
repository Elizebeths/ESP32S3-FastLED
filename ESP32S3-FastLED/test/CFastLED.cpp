/// FastLED 的高级控制器接口。
/// 此类管理控制器、全局设置以及亮度和刷新率等跟踪数据，
/// 并通过 show() / showColor() / clear() 方法提供将 LED 数据驱动到控制器的访问函数。
/// 该类以全局对象 FastLED 的形式实例化。
/// @nosubgrouping
class CFastLED {
	// int m_nControllers;
	unsigned char  m_Scale;         ///< 当前全局亮度缩放设置
	        fl::u16 m_nFPS;          ///< 当前每秒帧数 (FPS) 跟踪值
	fl::u32 m_nMinMicros;    ///< 帧之间的最小微秒数，用于限制帧率
	fl::u32 m_nPowerData;    ///< 最大功耗参数
	power_func m_pPowerFunc;  ///< 使用 FastLED.show() 时用于覆盖亮度的函数;

public:
	CFastLED();

	// 当你想知道 onFrameBegin 或 onFrameEnd 等事件何时发生时很有用。
	// 在 AVR 平台上禁用此功能以节省空间。
	void addListener(fl::EngineEvents::Listener *listener) { fl::EngineEvents::addListener(listener); }
	void removeListener(fl::EngineEvents::Listener *listener) { fl::EngineEvents::removeListener(listener); }

	/// @name 手动引擎事件
	/// 当定义了 FASTLED_MANUAL_ENGINE_EVENTS 时，这些方法允许手动控制引擎事件。
	/// 当未定义 FASTLED_MANUAL_ENGINE_EVENTS 时，这些事件由 show() 自动触发。
	/// @{
	
	/// 手动触发帧开始事件
	/// @note 除非定义了 FASTLED_MANUAL_ENGINE_EVENTS，否则此方法由 show() 自动调用
	void onBeginFrame() { fl::EngineEvents::onBeginFrame(); }
	
	/// 手动触发结束显示 LED 事件
	/// @note 除非定义了 FASTLED_MANUAL_ENGINE_EVENTS，否则此方法由 show() 自动调用
	void onEndShowLeds() { fl::EngineEvents::onEndShowLeds(); }
	
	/// @} 手动引擎事件

	/// 向系统添加一个 CLEDController 实例。此方法公开以便用户实现自己的
	/// CLEDController 对象或实例。调用此方法（以及其他 addLeds() 变体）有两种方式。
	/// 第一种使用 3 个参数，此时参数分别是控制器、指向 LED 数据的指针
	/// 以及此控制器使用的 LED 数量。第二种使用 4 个参数，此时
	/// 前两个参数相同，第三个参数是该控制器 CRGB 数据在 CRGB 数组中的偏移量，
	/// 第四个参数是该控制器对象的 LED 数量。
	/// @param pLed 要添加的 LED 控制器
	/// @param data 指向 CRGB 数据结构数组的基指针
	/// @param nLedsOrOffset LED 数量（3 参数版本）或数据数组中的偏移量
	/// @param nLedsIfOffset LED 数量（4 参数版本）
	/// @returns 对所添加控制器的引用
	static CLEDController &addLeds(CLEDController *pLed, struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0);

	/// @name 添加基于 SPI 的控制器
	/// 向系统添加基于 SPI 的 CLEDController 实例。
	///
	/// 调用此方法（以及其他 addLeds() 变体）有两种方式。
	/// 第一种使用 2 个参数，此时参数是指向 LED 数据的指针
	/// 以及此控制器使用的 LED 数量。第二种使用 3 个参数，此时
	/// 第一个参数相同，第二个参数是该控制器 CRGB 数据在 CRGB 数组中的偏移量，
	/// 第三个参数是该控制器对象的 LED 数量。
	///
	/// @param data 指向 CRGB 数据结构数组的基指针
	/// @param nLedsOrOffset LED 数量（2 参数版本）或数据数组中的偏移量
	/// @param nLedsIfOffset LED 数量（3 参数版本）
	/// @tparam CHIPSET 芯片组类型
	/// @tparam DATA_PIN LED 的（可选）数据引脚（如省略，默认使用第一个硬件 SPI MOSI 引脚）
	/// @tparam CLOCK_PIN LED 的（可选）时钟引脚（如省略，默认使用第一个硬件 SPI 时钟引脚）
	/// @tparam RGB_ORDER LED 的 RGB 颜色顺序（即红、绿、蓝数据的写入顺序）
	/// @tparam SPI_DATA_RATE 驱动 SPI 时钟的数据速率，使用 DATA_RATE_MHZ 或 DATA_RATE_KHZ 宏定义
	/// @returns 对所添加控制器的引用
	/// @{


	// 基础模板：如果使用不支持的 CHIPSET，将导致编译时错误
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN>
	struct ClockedChipsetHelper {
	    // 默认实现，支持的芯片组将提供特化版本
		static const bool IS_VALID = false;
	};

	// 定义一个宏，将 ESPIChipeset 枚举映射到各种模板配置中的控制器类
	#define _FL_MAP_CLOCKED_CHIPSET(CHIPSET_ENUM, CONTROLLER_CLASS)                              \
		template<fl::u8 DATA_PIN, fl::u8 CLOCK_PIN>                                              \
		struct ClockedChipsetHelper<CHIPSET_ENUM, DATA_PIN, CLOCK_PIN> {                           \
		    static const bool IS_VALID = true;                                                     \
			typedef CONTROLLER_CLASS<DATA_PIN, CLOCK_PIN> ControllerType;                          \
			/* 指定了 RGB_ORDER 的控制器类型 */                                         \
			template<EOrder RGB_ORDER>															   \
			struct CONTROLLER_CLASS_WITH_ORDER {                                                   \
				typedef CONTROLLER_CLASS<DATA_PIN, CLOCK_PIN, RGB_ORDER> ControllerType;           \
			};                                                                                     \
			/* 同时指定了 RGB_ORDER 和 SPI 频率的控制器类型 */                       \
			template<EOrder RGB_ORDER, fl::u32 FREQ>                                              \
			struct CONTROLLER_CLASS_WITH_ORDER_AND_FREQ {                                          \
				typedef CONTROLLER_CLASS<DATA_PIN, CLOCK_PIN, RGB_ORDER, FREQ> ControllerType;     \
			};                                                                                     \
		};

	// 为每个支持的 CHIPSET 定义特化
	_FL_MAP_CLOCKED_CHIPSET(LPD6803, LPD6803Controller)
	_FL_MAP_CLOCKED_CHIPSET(LPD8806, LPD8806Controller)
	_FL_MAP_CLOCKED_CHIPSET(WS2801, WS2801Controller)
	_FL_MAP_CLOCKED_CHIPSET(WS2803, WS2803Controller)
	_FL_MAP_CLOCKED_CHIPSET(SM16716, SM16716Controller)
	_FL_MAP_CLOCKED_CHIPSET(P9813, P9813Controller)

	// DOTSTAR 和 APA102 使用相同的控制器类
	_FL_MAP_CLOCKED_CHIPSET(DOTSTAR, APA102Controller)
	_FL_MAP_CLOCKED_CHIPSET(APA102, APA102Controller)

	// DOTSTARHD 和 APA102HD 使用相同的控制器类
	_FL_MAP_CLOCKED_CHIPSET(DOTSTARHD, APA102ControllerHD)
	_FL_MAP_CLOCKED_CHIPSET(APA102HD, APA102ControllerHD)

	_FL_MAP_CLOCKED_CHIPSET(HD107, APA102Controller)
	_FL_MAP_CLOCKED_CHIPSET(HD107HD, APA102ControllerHD)

	_FL_MAP_CLOCKED_CHIPSET(SK9822, SK9822Controller)
	_FL_MAP_CLOCKED_CHIPSET(SK9822HD, SK9822ControllerHD)


	#if FASTLED_FAKE_SPI_FORWARDS_TO_FAKE_CLOCKLESS
	/// 桩平台在模拟 SPI 控制器方面面临特殊挑战。
	/// 因此对于这些平台，我们将始终委托给 WS2812 无时钟控制器。
	/// 这没问题，因为桩平台上的无时钟控制器本来就是假的。
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN, EOrder RGB_ORDER, fl::u32 SPI_DATA_RATE > CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		// 使用 ClockedChipsetHelper 实例化控制器
		// 始终使用 WS2812 无时钟控制器，因为它是通用路径。
		return addLeds<WS2812, DATA_PIN, RGB_ORDER>(data, nLedsOrOffset, nLedsIfOffset);
	}

	/// 向系统添加基于 SPI 的 CLEDController 实例。
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN > static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		// 始终使用 WS2812 无时钟控制器，因为它是通用路径。
		return addLeds<WS2812, DATA_PIN>(data, nLedsOrOffset, nLedsIfOffset);
	}


	// 使用 ChipsetHelper 的 addLeds 函数
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN, EOrder RGB_ORDER>
	CLEDController& addLeds(struct CRGB* data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		// 始终使用 WS2812 无时钟控制器，因为它是通用路径。
		return addLeds<WS2812, DATA_PIN, RGB_ORDER>(data, nLedsOrOffset, nLedsIfOffset);
	}

	#else


	/// 向系统添加基于 SPI 的 CLEDController 实例。
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN, EOrder RGB_ORDER, fl::u32 SPI_DATA_RATE > CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		// 使用 ClockedChipsetHelper 实例化控制器
		typedef ClockedChipsetHelper<CHIPSET, DATA_PIN, CLOCK_PIN> CHIP;
		typedef typename CHIP::template CONTROLLER_CLASS_WITH_ORDER_AND_FREQ<RGB_ORDER, SPI_DATA_RATE>::ControllerType ControllerTypeWithFreq;
		static_assert(CHIP::IS_VALID, "Unsupported chipset");
		static ControllerTypeWithFreq c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}

	/// 向系统添加基于 SPI 的 CLEDController 实例。
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN > static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		typedef ClockedChipsetHelper<CHIPSET, DATA_PIN, CLOCK_PIN> CHIP;
		typedef typename CHIP::ControllerType ControllerType;
		static_assert(CHIP::IS_VALID, "Unsupported chipset");
		static ControllerType c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}


	// 使用 ChipsetHelper 的 addLeds 函数
	template<ESPIChipsets CHIPSET, fl::u8 DATA_PIN, fl::u8 CLOCK_PIN, EOrder RGB_ORDER>
	CLEDController& addLeds(struct CRGB* data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		typedef ClockedChipsetHelper<CHIPSET, DATA_PIN, CLOCK_PIN> CHIP;
		static_assert(CHIP::IS_VALID, "Unsupported chipset");
		typedef typename CHIP::template CONTROLLER_CLASS_WITH_ORDER<RGB_ORDER>::ControllerType ControllerTypeWithOrder;
		static ControllerTypeWithOrder c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}
	#endif


#ifdef SPI_DATA
	template<ESPIChipsets CHIPSET> static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		return addLeds<CHIPSET, SPI_DATA, SPI_CLOCK, RGB>(data, nLedsOrOffset, nLedsIfOffset);
	}

	template<ESPIChipsets CHIPSET, EOrder RGB_ORDER> static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		return addLeds<CHIPSET, SPI_DATA, SPI_CLOCK, RGB_ORDER>(data, nLedsOrOffset, nLedsIfOffset);
	}

	template<ESPIChipsets CHIPSET, EOrder RGB_ORDER, fl::u32 SPI_DATA_RATE> static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		return addLeds<CHIPSET, SPI_DATA, SPI_CLOCK, RGB_ORDER, SPI_DATA_RATE>(data, nLedsOrOffset, nLedsIfOffset);
	}

#endif
	/// @} 添加基于 SPI 的控制器

#ifdef FASTLED_HAS_CLOCKLESS
	/// @name 添加 3 线 LED 控制器
	/// 向系统添加基于无时钟（即 3 线，也支持 DMX）的 CLEDController 实例。
	///
	/// 调用此方法（以及其他 addLeds() 变体）有两种方式。
	/// 第一种使用 2 个参数，此时参数是指向 LED 数据的指针
	/// 以及此控制器使用的 LED 数量。第二种使用 3 个参数，此时
	/// 第一个参数相同，第二个参数是该控制器 CRGB 数据在 CRGB 数组中的偏移量，
	/// 第三个参数是该控制器对象的 LED 数量。
	///
	/// 此方法还需要 2 到 3 个模板参数来指定具体的芯片组、数据引脚、
	/// RGB 颜色顺序和 SPI 数据速率。
	///
	/// @param data 指向 CRGB 数据结构数组的基指针
	/// @param nLedsOrOffset LED 数量（2 参数版本）或数据数组中的偏移量
	/// @param nLedsIfOffset LED 数量（3 参数版本）
	/// @tparam CHIPSET 芯片组类型（必需）
	/// @tparam DATA_PIN LED 的数据引脚（必需）
	/// @tparam RGB_ORDER LED 的 RGB 颜色顺序（即红、绿、蓝数据的写入顺序）
	/// @returns 对所添加控制器的引用
	/// @{

	/// 向系统添加基于无时钟的 CLEDController 实例。
	template<template<fl::u8 DATA_PIN, EOrder RGB_ORDER> class CHIPSET, fl::u8 DATA_PIN, EOrder RGB_ORDER>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		static CHIPSET<DATA_PIN, RGB_ORDER> c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}

	/// 向系统添加基于无时钟的 CLEDController 实例。
	template<template<fl::u8 DATA_PIN, EOrder RGB_ORDER> class CHIPSET, fl::u8 DATA_PIN>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		static CHIPSET<DATA_PIN, RGB> c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}

	/// 向系统添加基于无时钟的 CLEDController 实例。
	template<template<fl::u8 DATA_PIN> class CHIPSET, fl::u8 DATA_PIN>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		static CHIPSET<DATA_PIN> c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}

	template<template<fl::u8 DATA_PIN> class CHIPSET, fl::u8 DATA_PIN>
	static CLEDController &addLeds(class fl::Leds& leds, int nLedsOrOffset, int nLedsIfOffset = 0) {
		CRGB* rgb = leds;
		return addLeds<CHIPSET, DATA_PIN>(rgb, nLedsOrOffset, nLedsIfOffset);
	}

#if defined(__FASTLED_HAS_FIBCC) && (__FASTLED_HAS_FIBCC == 1)
	template<fl::u8 NUM_LANES, template<fl::u8 DATA_PIN, EOrder RGB_ORDER> class CHIPSET, fl::u8 DATA_PIN, EOrder RGB_ORDER=RGB>
	static CLEDController &addLeds(struct CRGB *data, int nLeds) {
		static __FIBCC<CHIPSET, DATA_PIN, NUM_LANES, RGB_ORDER> c;
		return addLeds(&c, data, nLeds);
	}
#endif

	#ifdef FASTSPI_USE_DMX_SIMPLE
	template<EClocklessChipsets CHIPSET, fl::u8 DATA_PIN, EOrder RGB_ORDER=RGB>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0)
	{
		switch(CHIPSET) {
			case DMX: { static DMXController<DATA_PIN> controller; return addLeds(&controller, data, nLedsOrOffset, nLedsIfOffset); }
		}
	}
	#endif
	/// @} 添加 3 线 LED 控制器
#endif

	/// @name 添加第三方库控制器
	/// 向系统添加基于第三方库的 CLEDController 实例。
	///
	/// 调用此方法（以及其他 addLeds() 变体）有两种方式。
	/// 第一种使用 2 个参数，此时参数是指向 LED 数据的指针
	/// 以及此控制器使用的 LED 数量。第二种使用 3 个参数，此时
	/// 第一个参数相同，第二个参数是该控制器 CRGB 数据在 CRGB 数组中的偏移量，
	/// 第三个参数是该控制器对象的 LED 数量。此类包含 SmartMatrix
	/// 和 OctoWS2811 等控制器。
	///
	/// 此方法还需要 1 到 2 个模板参数来指定具体的芯片组和
	/// RGB 颜色顺序。
	///
	/// @param data 指向 CRGB 数据结构数组的基指针
	/// @param nLedsOrOffset LED 数量（2 参数版本）或数据数组中的偏移量
	/// @param nLedsIfOffset LED 数量（3 参数版本）
	/// @tparam CHIPSET 芯片组类型（必需）
	/// @tparam RGB_ORDER LED 的 RGB 颜色顺序（即红、绿、蓝数据的写入顺序）
	/// @returns 对所添加控制器的引用
	/// @{

	/// 向系统添加基于第三方库的 CLEDController 实例。
	template<template<EOrder RGB_ORDER> class CHIPSET, EOrder RGB_ORDER>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		static CHIPSET<RGB_ORDER> c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}

	/// 向系统添加基于第三方库的 CLEDController 实例。
	template<template<EOrder RGB_ORDER> class CHIPSET>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		static CHIPSET<RGB> c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}

#ifdef USE_OCTOWS2811
	/// 向系统添加基于 OCTOWS2811 的 CLEDController 实例。
	/// @see https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
	/// @see https://github.com/PaulStoffregen/OctoWS2811
	template<OWS2811 CHIPSET, EOrder RGB_ORDER>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0)
	{
		switch(CHIPSET) {
			case OCTOWS2811: { static COctoWS2811Controller<RGB_ORDER,WS2811_800kHz> controller; return addLeds(&controller, data, nLedsOrOffset, nLedsIfOffset); }
			case OCTOWS2811_400: { static COctoWS2811Controller<RGB_ORDER,WS2811_400kHz> controller; return addLeds(&controller, data, nLedsOrOffset, nLedsIfOffset); }
#ifdef WS2813_800kHz
      case OCTOWS2813: { static COctoWS2811Controller<RGB_ORDER,WS2813_800kHz> controller; return addLeds(&controller, data, nLedsOrOffset, nLedsIfOffset); }
#endif
		}
	}

	/// 向系统添加基于 OCTOWS2811 库的 CLEDController 实例。
	/// @see https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
	/// @see https://github.com/PaulStoffregen/OctoWS2811
	template<OWS2811 CHIPSET>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0)
	{
		return addLeds<CHIPSET,GRB>(data,nLedsOrOffset,nLedsIfOffset);
	}

#endif

#ifdef USE_WS2812SERIAL
	/// 向系统添加基于 WS2812Serial 库的 CLEDController 实例。
	/// @see https://www.pjrc.com/non-blocking-ws2812-led-library/
	/// @see https://github.com/PaulStoffregen/WS2812Serial
	template<SWS2812 CHIPSET, int DATA_PIN, EOrder RGB_ORDER>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0)
	{
		static CWS2812SerialController<DATA_PIN,RGB_ORDER> controller;
		return addLeds(&controller, data, nLedsOrOffset, nLedsIfOffset);
	}
#endif

#ifdef SmartMatrix_h
	/// 向系统添加基于 SmartMatrix 库的 CLEDController 实例。
	/// @see https://github.com/pixelmatix/SmartMatrix
	template<ESM CHIPSET>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0)
	{
		switch(CHIPSET) {
			case SMART_MATRIX: { static CSmartMatrixController controller; return addLeds(&controller, data, nLedsOrOffset, nLedsIfOffset); }
		}
	}
#endif
	/// @} 添加第三方库控制器


#ifdef FASTLED_HAS_BLOCKLESS

	/// @name 添加并行输出控制器
	/// 向系统添加基于块式并行输出的 CLEDController 实例。
	///
	/// 调用此方法（以及其他 addLeds() 变体）有两种方式。
	/// 第一种使用 2 个参数，此时参数是指向 LED 数据的指针
	/// 以及此控制器使用的 LED 数量。第二种使用 3 个参数，此时
	/// 第一个参数相同，第二个参数是该控制器 CRGB 数据在 CRGB 数组中的偏移量，
	/// 第三个参数是该控制器对象的 LED 数量。
	///
	/// 此方法还需要 2 到 3 个模板参数来指定具体的芯片组、
	/// RGB 颜色顺序和 SPI 数据速率。
	///
	/// @param data 指向 CRGB 数据结构数组的基指针
	/// @param nLedsOrOffset LED 数量（2 参数版本）或数据数组中的偏移量
	/// @param nLedsIfOffset LED 数量（3 参数版本）
	/// @tparam CHIPSET 芯片组/端口类型（必需）
	/// @tparam NUM_LANES 并行输出通道数
	/// @tparam RGB_ORDER LED 的 RGB 颜色顺序（即红、绿、蓝数据的写入顺序）
	/// @returns 对所添加控制器的引用
	/// @{

	/// 向系统添加基于块式并行输出的 CLEDController 实例。
	template<EBlockChipsets CHIPSET, int NUM_LANES, EOrder RGB_ORDER>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		switch(CHIPSET) {
		#ifdef PORTA_FIRST_PIN
				case WS2811_PORTA: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case WS2811_400_PORTA: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
        case WS2813_PORTA: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), data, nLedsOrOffset, nLedsIfOffset);
				case TM1803_PORTA: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case UCS1903_PORTA: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
		#endif
		#ifdef PORTB_FIRST_PIN
				case WS2811_PORTB: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case WS2811_400_PORTB: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
        case WS2813_PORTB: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), data, nLedsOrOffset, nLedsIfOffset);
				case TM1803_PORTB: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case UCS1903_PORTB: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
		#endif
		#ifdef PORTC_FIRST_PIN
				case WS2811_PORTC: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case WS2811_400_PORTC: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
        case WS2813_PORTC: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), data, nLedsOrOffset, nLedsIfOffset);
				case TM1803_PORTC: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case UCS1903_PORTC: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
		#endif
		#ifdef PORTD_FIRST_PIN
				case WS2811_PORTD: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case WS2811_400_PORTD: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
        case WS2813_PORTD: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), data, nLedsOrOffset, nLedsIfOffset);
				case TM1803_PORTD: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case UCS1903_PORTD: return addLeds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
		#endif
		#ifdef HAS_PORTDC
				case WS2811_PORTDC: return addLeds(new SixteenWayInlineBlockClocklessController<NUM_LANES,NS(320), NS(320), NS(640), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case WS2811_400_PORTDC: return addLeds(new SixteenWayInlineBlockClocklessController<NUM_LANES,NS(800), NS(800), NS(900), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
        case WS2813_PORTDC: return addLeds(new SixteenWayInlineBlockClocklessController<NUM_LANES, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), data, nLedsOrOffset, nLedsIfOffset);
				case TM1803_PORTDC: return addLeds(new SixteenWayInlineBlockClocklessController<NUM_LANES, NS(700), NS(1100), NS(700), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
				case UCS1903_PORTDC: return addLeds(new SixteenWayInlineBlockClocklessController<NUM_LANES, NS(500), NS(1500), NS(500), RGB_ORDER>(), data, nLedsOrOffset, nLedsIfOffset);
		#endif
		}
	}

	/// 向系统添加基于块式并行输出的 CLEDController 实例。
	template<EBlockChipsets CHIPSET, int NUM_LANES>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		return addLeds<CHIPSET,NUM_LANES,GRB>(data,nLedsOrOffset,nLedsIfOffset);
	}
	/// @} 添加并行输出控制器
#endif

	/// 设置全局亮度缩放
	/// @param scale 一个 0-255 的值，表示在写入 LED 之前对所有 LED 进行缩放的程度
	void setBrightness(fl::u8 scale) { m_Scale = scale; }

	/// 获取当前的全局亮度设置
	/// @returns 当前的全局亮度值
	fl::u8 getBrightness() { return m_Scale; }

	/// 设置最大功耗，以伏特和毫安为单位。
	/// @param volts LED 驱动电压（通常为 5）
	/// @param milliamps 期望的最大功耗毫安数
	inline void setMaxPowerInVoltsAndMilliamps(fl::u8 volts, fl::u32 milliamps) { setMaxPowerInMilliWatts(volts * milliamps); }

	/// 设置最大功耗，以毫瓦为单位。
	/// @param milliwatts 期望的最大功耗，单位为毫瓦
	inline void setMaxPowerInMilliWatts(fl::u32 milliwatts) { m_pPowerFunc = static_cast<power_func>(&calculate_max_brightness_for_power_mW); m_nPowerData = milliwatts; }

	/// 使用传入的亮度值，以当前 LED 颜色更新所有控制器
	/// @param scale 用于替代已存储值的亮度值
	void show(fl::u8 scale);

	/// 以当前 LED 颜色更新所有控制器
	void show() { show(m_Scale); }

	// 在 show() 结束时自动调用。
	void onEndFrame();

	/// 清除 LED 数据，擦除本地数据数组。你也可以选择
	/// 将清除后的数据发送到 LED。
	/// @param writeData 是否同时将数据写入 LED
	void clear(bool writeData = false);

	/// 清除本地数据数组
	void clearData();

	/// 将所有控制器上的所有 LED 设置为指定的颜色/亮度。
	/// @param color 要设置的颜色
	/// @param scale 显示时使用的亮度缩放值
	void showColor(const struct CRGB & color, fl::u8 scale);

	/// 将所有控制器上的所有 LED 设置为指定的颜色
	/// @param color 要设置的颜色
	void showColor(const struct CRGB & color) { showColor(color, m_Scale); }

	/// 延迟指定的毫秒数。提供此方法是为了让库能在没有 delay 函数的平台上使用
	///（使代码更具可移植性）。
	/// @note 此方法会不断调用 show() 来驱动抖动引擎（并且至少会调用一次 show()）。
	/// @param ms 暂停的毫秒数
	void delay(unsigned long ms);

	/// 设置全局色温。为所有已添加的 LED 灯带设置色温，
	/// 覆盖这些控制器之前可能具有的任何色温设置。
	/// @param temp 描述色温的 CRGB 结构体
	void setTemperature(const struct CRGB & temp);

	/// 设置全局颜色校正。为所有已添加的 LED 灯带设置颜色校正，
	/// 覆盖这些控制器之前可能具有的任何颜色校正设置。
	/// @param correction 描述颜色校正的 CRGB 结构体
	void setCorrection(const struct CRGB & correction);

	/// 设置抖动模式。为所有已添加的 LED 灯带设置抖动模式，
	/// 覆盖这些控制器之前可能具有的任何抖动设置。
	/// @param ditherMode 要使用的抖动类型，可选 BINARY_DITHER 或 DISABLE_DITHER
	void setDither(fl::u8 ditherMode = BINARY_DITHER);

	/// 设置最大刷新率。这对所有 LED 全局生效。
	/// 尝试以高于此速率的频率调用 show() 将等待。
	/// @note 刷新率默认为通过 addLeds() 添加的所有 LED 中最慢的刷新率。
	/// 如果你想设置/覆盖此速率，请确保在添加完所有 LED _之后_
	/// 再调用 setMaxRefreshRate()。
	/// @param refresh 最大刷新率，单位为 Hz
	/// @param constrain 是否将刷新率限制为已设置的最慢速度
	        void setMaxRefreshRate(fl::u16 refresh, bool constrain=false);

	/// 用于调试，跟踪 countFPS() 调用之间的时间。每 `nFrames` 次调用后，
	/// 会更新一个内部计数器以反映当前 FPS。
	/// @todo 将其改为滚动计数器
	/// @param nFrames 用于计算 FPS 的帧数
	void countFPS(int nFrames=25);

	/// 获取每秒写入的帧数
	/// @returns 最近计算的 FPS 值
	        fl::u16 getFPS() { return m_nFPS; }

	/// 获取已注册的控制器数量
	/// @returns 通过 addLeds() 添加的控制器（灯带）数量
	int count();

	/// 获取已注册控制器的引用
	/// @returns 第 N 个控制器的引用
	CLEDController & operator[](int x);

	/// 获取第一个控制器中的 LED 数量
	/// @returns 第一个控制器中的 LED 数量
	int size();

	/// 获取第一个控制器的 LED 数据指针
	/// @returns 指向第一个控制器 CRGB 缓冲区的指针
	CRGB *leds();
};