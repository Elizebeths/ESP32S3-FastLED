#pragma once


/// IMPORTANT!
/// 重要！
/// This file MUST not
/// 此文件绝不能

#include "fl/stdint.h"  // For uintptr_t and size_t
                       // 用于 uintptr_t 和 size_t

// Platform-specific integer type definitions
// 平台相关的整数类型定义
// This includes platform-specific 16/32/64-bit types
// 这包括平台相关的 16/32/64 位类型
#include "platforms/int.h"

namespace fl {
    // 8-bit types - char is reliably 8 bits on all supported platforms
    // 8位类型 - char 在所有支持的平台上都可靠地为 8 位
    // These must be defined BEFORE platform includes so fractional types can use them
    // 这些必须在平台包含之前定义，以便分数类型可以使用它们
    typedef signed char i8;
    typedef unsigned char u8;
    typedef unsigned int uint;
    
    // Pointer and size types are defined per-platform in platforms/int.h
    // 指针和大小类型在每个平台的 platforms/int.h 中定义
    // uptr (pointer type) and size (size type) are defined per-platform
    // uptr（指针类型）和 size（大小类型）在每个平台中定义

}



namespace fl {
    ///////////////////////////////////////////////////////////////////////
    ///
    /// Fixed-Point Fractional Types. 
    /// 定点分数类型。
    /// Types for storing fractional data. 
    /// 用于存储分数数据的类型。
    ///
    /// * ::sfract7 should be interpreted as signed 128ths.
    /// * ::sfract7 应解释为有符号的 128 分之一。
    /// * ::fract8 should be interpreted as unsigned 256ths.
    /// * ::fract8 应解释为无符号的 256 分之一。
    /// * ::sfract15 should be interpreted as signed 32768ths.
    /// * ::sfract15 应解释为有符号的 32768 分之一。
    /// * ::fract16 should be interpreted as unsigned 65536ths.
    /// * ::fract16 应解释为无符号的 65536 分之一。
    ///
    /// Example: if a fract8 has the value "64", that should be interpreted
    /// 示例：如果 fract8 的值为 "64"，则应解释为
    ///          as 64/256ths, or one-quarter.
    ///          64/256，即四分之一。
    ///
    /// accumXY types should be interpreted as X bits of integer,
    /// accumXY 类型应解释为 X 位整数，
    ///         and Y bits of fraction.  
    ///         和 Y 位分数。
    /// E.g., ::accum88 has 8 bits of int, 8 bits of fraction
    /// 例如：::accum88 有 8 位整数，8 位分数
    ///

    /// ANSI: unsigned short _Fract. 
    /// ANSI: unsigned short _Fract。
    /// Range is 0 to 0.99609375 in steps of 0.00390625.  
    /// 范围是 0 到 0.99609375，步长为 0.00390625。
    /// Should be interpreted as unsigned 256ths.
    /// 应解释为无符号的 256 分之一。
    typedef u8   fract8;

    /// ANSI: signed short _Fract. 
    /// ANSI: signed short _Fract。
    /// Range is -0.9921875 to 0.9921875 in steps of 0.0078125.  
    /// 范围是 -0.9921875 到 0.9921875，步长为 0.0078125。
    /// Should be interpreted as signed 128ths.
    /// 应解释为有符号的 128 分之一。
    typedef i8    sfract7;

    /// ANSI: unsigned _Fract.
    /// ANSI: unsigned _Fract。
    /// Range is 0 to 0.99998474121 in steps of 0.00001525878.  
    /// 范围是 0 到 0.99998474121，步长为 0.00001525878。
    /// Should be interpreted as unsigned 65536ths.
    /// 应解释为无符号的 65536 分之一。
    typedef u16  fract16;

    typedef i32   sfract31; ///< ANSI: signed long _Fract. 31 bits int, 1 bit fraction
                           ///< ANSI: signed long _Fract。31 位整数，1 位分数

    typedef u32  fract32;   ///< ANSI: unsigned long _Fract. 32 bits int, 32 bits fraction
                           ///< ANSI: unsigned long _Fract。32 位整数，32 位分数

    /// ANSI: signed _Fract.
    /// ANSI: signed _Fract。
    /// Range is -0.99996948242 to 0.99996948242 in steps of 0.00003051757.  
    /// 范围是 -0.99996948242 到 0.99996948242，步长为 0.00003051757。
    /// Should be interpreted as signed 32768ths.
    /// 应解释为有符号的 32768 分之一。
    typedef i16   sfract15;

    typedef u16  accum88;    ///< ANSI: unsigned short _Accum. 8 bits int, 8 bits fraction
                            ///< ANSI: unsigned short _Accum。8 位整数，8 位分数
    typedef i16   saccum78;   ///< ANSI: signed   short _Accum. 7 bits int, 8 bits fraction
                             ///< ANSI: signed   short _Accum。7 位整数，8 位分数
    typedef u32  accum1616;  ///< ANSI: signed         _Accum. 16 bits int, 16 bits fraction
                            ///< ANSI: signed         _Accum。16 位整数，16 位分数
    typedef i32   saccum1516; ///< ANSI: signed         _Accum. 15 bits int, 16 bits fraction
                             ///< ANSI: signed         _Accum。15 位整数，16 位分数
    typedef u16  accum124;   ///< no direct ANSI counterpart. 12 bits int, 4 bits fraction
                            ///< 无直接的 ANSI 对应类型。12 位整数，4 位分数
    typedef i32   saccum114;  ///< no direct ANSI counterpart. 1 bit int, 14 bits fraction
                             ///< 无直接的 ANSI 对应类型。1 位整数，14 位分数
}

namespace fl {    
    // Size assertions moved to src/platforms/compile_test.cpp.hpp
    // 大小断言已移至 src/platforms/compile_test.cpp.hpp
}

// Make fractional types available in global namespace
// 使分数类型在全局命名空间中可用
using fl::fract8;
using fl::sfract7;
using fl::fract16;
using fl::sfract31;
using fl::fract32;
using fl::sfract15;
using fl::accum88;
using fl::saccum78;
using fl::accum1616;
using fl::saccum1516;
using fl::accum124;
using fl::saccum114;