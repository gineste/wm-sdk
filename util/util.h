/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef UTIL_H
#define UTIL_H

/**
 * \file util.h
 *
 * \section compfunc Comparison functions
 *
 * Comparison functions include comparison for 8, 16, and 32-bit
 * integers (Util_cmpUint8, Util_cmpUint16, Util_cmpUint32).
 * The functions compare two values, and return the comparison
 * result in UTIL_CMP_XXX value. For convenience, the following
 * macros have been defined:
 *
 * - Util_isLtUint{8,16,32}(a,b)
 * - Util_isGtUint{8,16,32}(a,b)
 * - Util_isEqUint{8,16,32}(a,b)
 * - Util_isLtOrEqUint{8,16,32}(a,b)
 * - Util_isGtOrEqUint{8,16,32}(a,b)
 *
 * For example:
 * \code
 * if (Util_isLtUint8(1, 2))
 * {
 *     // The preceding macro will return true ->
 *     // this code will be executed.
 * }
 * \endcode
 *
 * \section lowrestimer Low-resolution timer
 *
 * Low-resolution timers have a granularity of one second. Timers
 * can be added with Util_addTimer(). When a timer is created,
 * the user defines whether the timer is an application timer or not.
 * The application timers generate App_timer() event, other
 * timers must be checked with Util_checkTimer().
 * A timer can be modified and/or removed with Util_setTimer() function.
 */

#include <stdint.h>
#include <stdbool.h>

#define Util_isLtUint8(a, b)        ((int8_t)((a) - (b)) < 0)
#define Util_isGtOrEqUint8(a, b)    ((int8_t)((a) - (b)) >= 0)
#define Util_isLtUint16(a, b)       ((int16_t)((a) - (b)) < 0)
#define Util_isGtOrEqUint16(a, b)   ((int16_t)((a) - (b)) >= 0)
#define Util_isGtOrEqUint32(a, b)   !Util_isLtUint32(a, b)

bool Util_isLtUint32(uint32_t a, uint32_t b);

#define Util_isGtUint8(a, b)    (!Util_isGtOrEqUint8(b, a))
#define Util_isGtUint16(a, b)   (!Util_isGtOrEqUint16(b, a))
#define Util_isGtUint32(a, b)   (!Util_isGtOrEqUint32(b, a))

/**
 * Compares a, b, and c.
 * @return
 *   Returns true, if a<=b and a<=c.
 */
bool Util_isSmallest(const uint32_t a, const uint32_t b, const uint32_t c);

/**
 * Compares a, b, and c.
 * @return
 *   Returns true, if a<=b<=c.
 * @pre
 *   a<=c
 */
bool Util_inBetween(const uint32_t a, const uint32_t b, const uint32_t c);

/* Aligning macros for structs.
 *
 * These can be used in structs to align the struct members correctly,
 * especially when preceeding struct member size varies by some compilation
 * flag
 *
 * Usage:
 * STRUCT_ALIGN_4(prev_type);
 * STRUCT_ALIGN_2(prev_type);
 *
 * These align the next member in struct to 4 or 2 byte boundary.
 *
 * For example:
 * typedef struct
 * {
 *      another_struct_t a;
 *
 *      STRUCT_ALIGN_4(another_struct_t);
 *
 *      uint32_t b;
 * } example_struct_t;
 *
 * In this case, if length of another_struct_t varies, this ensures that b is
 * aligned to 32-bit boundary (4 bytes)
 *
 * Another example:
 * typedef struct
 * {
 *      another_struct_t a;
 *
 *      STRUCT_ALIGN_2(another_struct_t);
 *
 *      uint16_t b;
 * } example_struct_t;
 *
 * This case, align macro ensures that b is aligned to 16-bit boundary
 *
 * NOTE: Do NOT use in PACKED structs!
 */

// These are macro replacement redirection forumulas to do the correct operation
#define SA_MEMBERNAME(line) align ## line
#define SA_BASE(prev_type, size, line) \
    uint8_t SA_MEMBERNAME(line) \
    [sizeof(prev_type)%size ? size-(sizeof(prev_type)%size) : 0]

#define STRUCT_ALIGN_4(prev_type) SA_BASE(prev_type, 4, __LINE__)
    //uint8_t align ## __LINE__ [sizeof(prev_type)%4 ? 4-(sizeof(prev_type)%4) : 0]
#define STRUCT_ALIGN_2(prev_type) SA_BASE(prev_type, 2, __LINE__)
    // uint8_t align ## __LINE__ [sizeof(prev_type)%2 ? 1 : 0]

/**
 * \brief   Calculate the '1' bits in 8-bit value
 * \param   value
 *          Input value from where the amount of '1' bits is calculated
 * \return  Amount of bits that are 1
 */
uint8_t Util_bitCountU8(uint8_t value);


// These are macro replacement redirection forumulas to do the correct operation
#define SA_MEMBERNAME(line) align ## line
#define SA_BASE(prev_type, size, line) \
    uint8_t SA_MEMBERNAME(line) \
    [sizeof(prev_type)%size ? size-(sizeof(prev_type)%size) : 0]

#define STRUCT_ALIGN_4(prev_type) SA_BASE(prev_type, 4, __LINE__)
    //uint8_t align ## __LINE__ [sizeof(prev_type)%4 ? 4-(sizeof(prev_type)%4) : 0]
#define STRUCT_ALIGN_2(prev_type) SA_BASE(prev_type, 2, __LINE__)
    // uint8_t align ## __LINE__ [sizeof(prev_type)%2 ? 1 : 0]

#define BIT_MASK(val,mask,shift)    ((((uint32_t)val) & ((uint32_t)mask)) >> ((uint32_t)shift))

/** The higher 8 bits of a upper 16 bits of a 32 bit value */
#define XMSB_32B_TO_8B(x)   ((uint8_t)((((uint32_t)x) & 0xFF000000) >> 24u))
/** The lower 8 bits of a upper 16 bits of a 32 bit value */
#define MSB_32B_TO_8B(x)    ((uint8_t)((((uint32_t)x) & 0x00FF0000) >> 16u))
/** The higher 8 bits of a lower 16 bits of a 32 bit value */
#define LSB_32B_TO_8B(x)    ((uint8_t)((((uint32_t)x) & 0x0000FF00) >> 8u))
/** The lowest 8 bits of a 32 bit value */
#define XLSB_32B_TO_8B(x)   ((uint8_t)(((uint32_t)x) & 0x000000FF))

/** The lower 16 bits of a 32 bit value */
#define MSB_32B_TO_16B(x)    ((uint16_t)((((uint32_t)x) & 0xFFFF0000) >> 16u))
/** The higher 16 bits of a 32 bit value */
#define LSB_32B_TO_16B(x)    ((uint16_t)(((uint32_t)x) & 0x0000FFFF))

/** The upper 8 bits of a 16 bit value */
#define MSB_16B_TO_8B(x)    ((uint8_t)((((uint16_t)x) & 0xFF00) >> 8u))
/** The lower 8 bits (of a 16 bit value) */
#define LSB_16B_TO_8B(x)    ((uint8_t)(((uint16_t)x) & 0x00FF))

/** The upper 4 bits of a 8 bit value */
#define MSB_8B(x)    ((uint8_t)(((uint8_t)x) & 0xF0))
/** The lower 4 bits (of a 8 bit value) */
#define LSB_8B(x)    ((uint8_t)(((uint8_t)x) & 0x0F))

/** Assembly two 8 bits into 16 bit value */
#define U8_TO_U16(msb,lsb)             ((uint16_t) ((((uint16_t)msb)<<8u) + ((uint16_t)lsb)) )
/** Assembly four 8 bits into 32 bit value */
#define U8_TO_U32(xmsb,msb,lsb,xlsb)   ((uint32_t) ((((uint32_t)xmsb)<<24u) + (((uint32_t)msb)<<16u) + \
                                                    (((uint32_t)lsb) <<8u)  +  ((uint32_t)xlsb)) )
/** Assembly two 16 bits into 32 bit value */
#define U16_TO_U32(msb,lsb)            ((uint32_t) ((((uint32_t)msb)<<16u) + ((uint32_t)lsb)) )

#endif // #ifndef __UTIL_H__
