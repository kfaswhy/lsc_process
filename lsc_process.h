#ifndef _LSC_H_
#define _LSC_H_

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h> 
#include <time.h>
#include <omp.h>
#include "cJSON.h"

#define U64 unsigned long long
#define S64 long long
#define U32 unsigned int
#define S32 int
#define U16 unsigned short
#define U8 unsigned char 
#define U16MAX (0xFFFF)
#define U8MAX (255)
#define U8MIN (0)

#define M_PI 3.1415926

#define calc_min(a,b) ((a)>(b)?(b):(a))
#define calc_max(a,b) ((a)<(b)?(b):(a))
#define calc_abs(a) ((a)>0?(a):(-a))
#define clp_range(min,x,max) calc_min(calc_max((x), (min)), (max))

#define LOG(...) printf("%s [%d]: ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n");


U32 start = clock();
U32 end;
U8 prog_print = 1;

U32 t0;
U32 t1;



typedef enum SAMP_METHOD
{
	NEAREST = 0,
	LINEAR,
	BILINEAR,
};

typedef struct _RGB
{
	BYTE b;
	BYTE g;
	BYTE r;
}RGB;

typedef struct {
	S32 h;
	BYTE s;
	BYTE v;
}HSV;

typedef struct
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
}RGBA;

typedef struct _LSC
{
	U32* r_gain;
	U32* g_gain;
	U32* b_gain;
}LSC;


int main();

S32 calc_interpolation_array(S32* array_x, S32* array_y, S32 size, S32 x);

void print_prog(U32 cur_pos, U32 tgt);

float fast_sqrt(float number);

int img_process(RGB* img);


void do_lsc_cali(RGB* img, LSC* lsc);

void calc_lsc(U32 max, LSC* lsc);

S32 enblock(RGB* img, S32 blk_width, S32 blk_height);

U8 search_max(RGB* img);

RGB* load_bmp(const char* filename);

void save_bmp(const char* filename, RGB* img, int width, int height);


#endif