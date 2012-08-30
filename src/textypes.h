
#ifndef textypes_h_included
#define textypes_h_included

#include <stdint.h>
#include <stdio.h>

#include <iostream>

// Quick hacks.

#define incr(i)		++(i)
#define decr(i)	 	--(i)
#define odd(i)		((i) & 1)
#define negate(x)	(x) = -(x)
#define loop		while (1)

// TeX builtin types, here given a fixed length using stdint.h

typedef uint8_t	 byte;
typedef int32_t	 tok;
typedef int32_t    fnt;
typedef float		 ratio;
typedef int32_t 	 scal;
typedef char*		 str;
typedef FILE*		 file;
typedef int32_t    ptr;

// File extensions

const static str	str_tex = ".tex";
const static str	str_dvi = ".dvi";
const static str 	str_log = ".log";
const static str	str_tfm = ".tfm";

// Limits

#define NUL			'\0'
#define EOLN			'\n'
#define FALSE			0
#define TRUE			1
#define EMPTY			0
#define MIN_QUARTERWORD		0
#define MAX_QUARTERWORD		255
#define MIN_HALFWORD		0
#define MAX_HALFWORD		65535

// Streams

#define term_in	 stdin
#define term_out	 stderr
#define errstr     std::cerr
#define outstr     std::cout
#define instr      std::cin

#endif
