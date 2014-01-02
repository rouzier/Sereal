#ifndef _MAIN_H
#define _MAIN_H
#include <ruby.h>
#include <ruby/encoding.h>
#include <ruby/io.h>
#include <ruby/st.h>
#include <string.h>     /* memcpy,memset */
#include "proto.h"

typedef unsigned long long      u64;
typedef unsigned int            u32;
typedef unsigned short          u16;
typedef unsigned char           u8;
typedef struct _sereal          sereal_t;
typedef struct _track_entry     track_t;

#define TRUE 1
#define FALSE 0
#define MAX_RECURSION_DEPTH 100
#define COPY(src,dst,len) memcpy((dst),(src),len)
#define ZERO(src,len) memset((src),0,len)

#define is_ascii_string(str) (rb_enc_str_coderange(str) == ENC_CODERANGE_7BIT)
#define THRESH(x,min,max) ((x) >= (min) && (x) <= (max))

#define IS_SHORT_BINARY(t) THRESH(t,SRL_HDR_SHORT_BINARY_LOW,SRL_HDR_SHORT_BINARY_HIGH)
#define IS_ARRAYREF(t) THRESH(t,SRL_HDR_ARRAYREF_LOW,SRL_HDR_ARRAYREF_HIGH)
#define IS_HASHREF(t) THRESH(t,SRL_HDR_HASHREF_LOW,SRL_HDR_HASHREF_HIGH)
#define IS_STRING(t) ((t) == SRL_HDR_STR_UTF8 || (t) == SRL_HDR_BINARY || IS_SHORT_BINARY((t)))

#define SRL_HDR_SYM SRL_HDR_RESERVED_LOW
#define SRL_HDR_RB_OBJ (SRL_HDR_RESERVED_LOW+1)

#ifdef ONIGURUMA_H
#define IGNORECASE ONIG_OPTION_IGNORECASE
#define MULTILINE ONIG_OPTION_MULTILINE
#define EXTENDED ONIG_OPTION_EXTEND
#else
#define IGNORECASE RE_OPTION_IGNORECASE
#define MULTILINE RE_OPTION_MULTILINE
#define EXTENDED RE_OPTION_EXTENDED
#endif

#define FORMAT(fmt,arg...) fmt " [%s():%s:%d @ %u]\n",##arg,__func__,__FILE__,__LINE__,(unsigned int) time(NULL)
#define E(fmt,arg...) fprintf(stderr,FORMAT(fmt,##arg))
#define D(fmt,arg...) printf(FORMAT(fmt,##arg))

#define s_raise(what,ex,arg...)         \
do {                                    \
    s_destroy(what);                    \
    rb_raise(ex,##arg);                 \
} while(0);

#define FLAG_NOT_MINE 1
#define FLAG_STREAM   2
struct _sereal {
    u8 *data;
    u32 size;
    u32 pos;
    u32 rsize;
    u32 level;
    u8 flags;
    VALUE tracked;
    u32 hdr_end;
    int fd;
};

VALUE method_sereal_encode(VALUE self, VALUE args);
VALUE method_sereal_decode(VALUE self, VALUE payload);

#define S_RECURSE_INC(s)                                        \
    do {                                                        \
        if((s)->level++ > MAX_RECURSION_DEPTH)                  \
        s_raise((s),rb_eArgError,                               \
                "max recursion depth reached: %d (level: %d)",  \
                MAX_RECURSION_DEPTH, s->level);                 \
    } while(0);

#define S_RECURSE_DEC(s) ((s)->level--)
#ifndef HAVE_RB_INTERN_STR
#define rb_intern_str(string) SYM2ID(rb_str_intern(string))
#endif
#ifndef HAVE_RB_SYM_TO_S
#define rb_sym_to_s(object) rb_funcall(object,rb_intern("to_s"),0)
#endif

#define __RAW           0
#define __SNAPPY        1
#define __SNAPPY_INCR   2
#define __MIN_SIZE      6
#endif
