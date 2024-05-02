#ifndef _MATHFUNC_H
#define _MATHFUNC_H
#include "stm32f4xx.h"
#include "stdbool.h"

#define ABS(x) ((x > 0) ? (x) : (-(x)))
#define PlusOrMinus(x) ((x > 0) ? 1 : (-1))
#define PEAK(A, B)       \
    if (ABS(A) > ABS(B)) \
        A = PlusOrMinus(A) * B;

#define EncodeS32Data(f, buff) \
    {                          \
        *(int32_t *)buff = *f; \
    }
#define DecodeS32Data(f, buff) \
    {                          \
        *f = *(int32_t *)buff; \
    }
#define EncodeS16Data(f, buff) \
    {                          \
        *(s16 *)buff = *f;     \
    }
#define DecodeS16Data(f, buff) \
    {                          \
        *f = *(s16 *)buff;     \
    }
#define EncodeU16Data(f, buff) \
    {                          \
        *(u16 *)buff = *f;     \
    }
#define DecodeU16Data(f, buff) \
    {                          \
        *f = *(u16 *)buff;     \
    }

#define AccuracySelect(x) ((x > 0) ? (x + 0.5) : (x - 0.5))

void ChangeDataByte(uint8_t *p1, uint8_t *p2);
void buffer_append_int32(uint8_t *buffer, int32_t source, int32_t *index);
int32_t get_s32_from_buffer(const uint8_t *buffer, int32_t *index);
float buffer_32_to_float(const uint8_t *buffer, float scale, int32_t *index);
int16_t get_s16_from_buffer(const uint8_t *buffer, int32_t *index);
float buffer_16_to_buffer(const uint8_t *buffer, float scale, int32_t *index);
#endif
