#ifndef BOBO_LANG_TYPES_H
#define BOBO_LANG_TYPES_H

#include <sys/types.h>

typedef union {
    u_int64_t bits;
    double dbl;
} Value;

# if defined(__x86_64__)
#   define PTR_TYPE u_int64_t
# else
#   define PTR_TYPE u_int32_t
# endif

#define QNAN        ((u_int64_t) 0x7ffc000000000000)
#define SIGN_BIT    ((u_int64_t) 0x8000000000000000)

#define TAG_NIL     1
#define TAG_FALSE   2
#define TAG_TRUE    3

#define NIL         ((Value) (u_int64_t) (QNAN | TAG_NIL))
#define FALSE       ((Value) (u_int64_t) (QNAN | TAG_FALSE))
#define TRUE        ((Value) (u_int64_t) (QNAN | TAG_TRUE))
#define WRAP_NUM(n) ((Value) (double) (n))
#define WRAP_PTR(p) ((Value) (SIGN_BIT | QNAN | (PTR_TYPE) (p)))

#define IS_NIL(v)   ((v.bits) == (NIL.bits))
#define IS_BOOL(v)  ((v.bits) == (TRUE.bits) || (v.bits) == (FALSE.bits))
#define IS_NUM(v)   (((v.bits) & QNAN) != QNAN)
#define IS_PTR(v)   (((v.bits) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define AS_BOOL(v)  ((v.bits) == (TRUE.bits))
#define AS_NUM(v)   (v.dbl)
#define AS_PTR(v)   ((void *) (PTR_TYPE) ((v.bits) & ~(SIGN_BIT | QNAN)))

#endif