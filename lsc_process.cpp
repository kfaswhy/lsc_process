#include "lsc_process.h"

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;
int PaddingSize = 0;
BYTE* pad = NULL;

#define output_block_img  1

int height = 0;
int width = 0;

int lsc_width = 17;
int lsc_height = 27;

int gain_ratio_perent = 40;

LSC lsc = { 0 };
U32 max = 0;

int main()
{
	U32 a = 18;
	U32 max = 255;
	LOG("%u x 1024 = %u.", max, max * 1024);
	max = max * 1024;

	LOG("%u / %u = %u", max, a, max / a);



	char cfg_setting[] = "setting.config";
	//load_cfg(cfg_setting);


	char bmp_in[] = "1.bmp";
	RGB* img = NULL;
	img = load_bmp(bmp_in);
	if (img == NULL)
	{
		return 0;
	}

	img_process(img);

	t1 = clock();
	U32 d_t = t1 - t0;
	LOG("sum time = %.3f.", (float)d_t / 1000);

	return 0;
}



int img_process(RGB* img)
{
	//查找最亮点
	max = search_max(img);
	
	//图像分块并计算均值
	enblock(img, lsc_width, lsc_height);
	
	//计算块通道增益
	calc_lsc(max, &lsc);

	//执行校正
	do_lsc_cali(img, &lsc);


	return 0;
}


void dump_lsc(LSC* lsc)
{
	for (S32 by = 0; by < lsc_height; ++by)
	{
		for (S32 bx = 0; bx < lsc_width; ++bx)
		{

		}
	}

	return;
}
void do_lsc_cali(RGB* img, LSC* lsc)
{
	U32 lum = 0;
	U32 tmp = 0;
	U32 tmp_gain = 0;
	U32 ratio = 0;

	//色彩校正
	for (S32 by = 0; by < lsc_height; ++by)
	{
		for (S32 bx = 0; bx < lsc_width; ++bx)
		{
			for (S32 y = by * (height / lsc_height + 1); (y < (by + 1) * (height / lsc_height + 1)) && (y < height); ++y)
			{
				for (S32 x = bx * (width / lsc_width + 1); (x < (bx + 1) * (width / lsc_width + 1)) && (x < width); ++x)
				{
					lum = (img[y * width + x].r * 3 + img[y * width + x].g * 6 + img[y * width + x].b) / 10;
					ratio = lum * 100 / max;

					tmp = img[y * width + x].r * lsc->r_gain[by * lsc_width + bx] * ratio / 102400;
					img[y * width + x].r = calc_min(tmp, 255);
					
					tmp = img[y * width + x].g * lsc->g_gain[by * lsc_width + bx] * ratio / 102400;
					img[y * width + x].g = calc_min(tmp, 255);

					tmp = img[y * width + x].b * lsc->b_gain[by * lsc_width + bx] * ratio / 102400;
					img[y * width + x].b = calc_min(tmp, 255);
				}
			}
		}
	}
	save_bmp("4_color_cali.bmp", img, width, height);

	save_bmp("5_y_cali.bmp", img, width, height);
}

void calc_lsc(U32 max, LSC* lsc)
{
#ifdef output_block_img
	U32 lsc_gain_max = 0;
#endif
	
	for (S32 by = 0; by < lsc_height; ++by) 
	{
		for (S32 bx = 0; bx < lsc_width; ++bx) 
		{
			int index = by * lsc_width + bx;
			lsc->r_gain[index] = (max << 10) / lsc->r_gain[index];
			lsc->g_gain[index] = (max << 10) / lsc->g_gain[index];
			lsc->b_gain[index] = (max << 10) / lsc->b_gain[index];

			//lsc->r_gain[index] = (lsc->r_gain[index] - 1024) * gain_ratio_perent / 100 + 1024;
			//lsc->g_gain[index] = (lsc->g_gain[index] - 1024) * gain_ratio_perent / 100 + 1024;
			//lsc->b_gain[index] = (lsc->b_gain[index] - 1024) * gain_ratio_perent / 100 + 1024;
#ifdef output_block_img
			lsc_gain_max = calc_max(lsc_gain_max, lsc->r_gain[index]);
			lsc_gain_max = calc_max(lsc_gain_max, lsc->g_gain[index]);
			lsc_gain_max = calc_max(lsc_gain_max, lsc->b_gain[index]);
#endif
		}
	}

	

#ifdef output_block_img

	RGB* tmp = (RGB*)malloc(sizeof(RGB) * height * width);
	if (tmp == NULL)
	{
		LOG("ERROR.");
		return;
	}
	for (S32 by = 0; by < lsc_height; ++by) {
		for (S32 bx = 0; bx < lsc_width; ++bx) {
			// 将该块内的所有像素值替换成该块的均值
			for (S32 y = by * (height / lsc_height + 1); (y < (by + 1) * (height / lsc_height + 1)) && (y < height); ++y)
			{
				for (S32 x = bx * (width / lsc_width + 1); (x < (bx + 1) * (width / lsc_width + 1)) && (x < width); ++x)
				{
					//y = calc_min(y, height);
					//x = calc_min(x, width); 
					tmp[y * width + x].r = lsc->r_gain[by * lsc_width + bx] * 255 / lsc_gain_max;
					tmp[y * width + x].g = lsc->g_gain[by * lsc_width + bx] * 255 / lsc_gain_max;
					tmp[y * width + x].b = lsc->b_gain[by * lsc_width + bx] * 255 / lsc_gain_max;
				}
			}
		}
	}
	save_bmp("3_lsc.bmp", tmp, width, height);

	
#endif

	return;
}

S32 enblock(RGB* img, S32 lsc_width, S32 lsc_height)
{

	lsc.r_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc.g_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc.b_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	if (!lsc.r_gain || !lsc.g_gain || !lsc.b_gain) {
		free(lsc.r_gain);
		free(lsc.g_gain);
		free(lsc.b_gain);
		return -1; // 内存分配失败
	}

	for (S32 by = 0; by < lsc_height; ++by) 
	{
		for (S32 bx = 0; bx < lsc_width; ++bx) 
		{
			U32 sum_r = 0, sum_g = 0, sum_b = 0;
			S32 count = 0;

			for (S32 y = by * (height / lsc_height + 1); (y < (by + 1) * (height / lsc_height + 1)) && (y < height); ++y)
			{
				for (S32 x = bx * (width / lsc_width + 1); (x < (bx + 1) * (width / lsc_width + 1)) && (x < width); ++x)
				{
					//y = calc_min(y, height);
					//x = calc_min(x, width);
					sum_r += img[y * width + x].r;
					sum_g += img[y * width + x].g;
					sum_b += img[y * width + x].b;
					++count;
				}
			}

			U16 avg_r = sum_r / count;
			U16 avg_g = sum_g / count;
			U16 avg_b = sum_b / count;

			lsc.r_gain[by * lsc_width + bx] = avg_r;
			lsc.g_gain[by * lsc_width + bx] = avg_g;
			lsc.b_gain[by * lsc_width + bx] = avg_b;


		}
	}

#ifdef output_block_img
	RGB* tmp = (RGB*)malloc(sizeof(RGB) * height * width);
	if (tmp == NULL)
	{
		LOG("ERROR.");
		return ERROR;
	}
	for (S32 by = 0; by < lsc_height; ++by) {
		for (S32 bx = 0; bx < lsc_width; ++bx) {
			// 将该块内的所有像素值替换成该块的均值
			for (S32 y = by * (height / lsc_height + 1); (y < (by + 1) * (height / lsc_height + 1)) && (y < height); ++y)
			{
				for (S32 x = bx * (width / lsc_width + 1); (x < (bx + 1) * (width / lsc_width + 1)) && (x < width); ++x)
				{
					//y = calc_min(y, height);
					//x = calc_min(x, width); 
					tmp[y * width + x].r = lsc.r_gain[by * lsc_width + bx];
					tmp[y * width + x].g = lsc.g_gain[by * lsc_width + bx];
					tmp[y * width + x].b = lsc.b_gain[by * lsc_width + bx];
				}
			}
		}
	}
	save_bmp("2_enblock.bmp", tmp, width, height);

#endif
	//free(lsc.r_gain);
	//free(lsc.g_gain);
	//free(lsc.b_gain);
	LOG("done.");
	return 0;
}


U8 search_max(RGB* img)
{
	U8 max = 0;
	U32 pos_x = 0, pos_y = 0;
	RGB* pixel = img;

	for (U32 y = 0; y < height; y++) {
		for (U32 x = 0; x < width; x++) {
			if (pixel->r > max) {
				max = pixel->r;
				pos_x = x;
				pos_y = y;
			}
			else if (pixel->g > max) {
				max = pixel->g;
				pos_x = x;
				pos_y = y;
			}
			else if (pixel->b > max) {
				max = pixel->b;
				pos_x = x;
				pos_y = y;
			}
			pixel++;
		}
	}

	LOG("Max value: %u at position (%u, %u) with RGB(%u, %u, %u).", max, pos_x, pos_y, 
		img[pos_y * width + pos_x].r, img[pos_y * width + pos_x].g, img[pos_y * width + pos_x].b);

	return max;
}

S32 calc_interpolation_array(S32* array_x, S32* array_y, S32 size, S32 x)
{
	if (x <= array_x[0]) {
		return array_y[0];
	}
	if (x >= array_x[size - 1]) {
		return array_y[size - 1];
	}
	for (U32 i = 0; i < size - 1; ++i) {
		if (x == array_x[i]) {
			return array_y[i];
		}
		if (x > array_x[i] && x < array_x[i + 1]) {
			S32 x0 = array_x[i];
			S32 x1 = array_x[i + 1];
			S32 y0 = array_y[i];
			S32 y1 = array_y[i + 1];
			return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
		}
	}

	// This should not be reached if input x is within the bounds
	return array_y[0];
}

void print_prog(U32 cur_pos, U32 tgt)
{
	end = clock();

	if ((end - start) >= 1000)
	{
		LOG("Processing: %d%%.", cur_pos * 100 / tgt);
		start = clock();
	}
}

float fast_sqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;                       // 将 float 解释为 long 类型
	i = 0x5f3759df - (i >> 1);            // 魔术数字
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));  // 近似值调整

	return 1.0f / y;
}

RGB* load_bmp(const char* filename)
{
	FILE* f_in = fopen(filename, "rb");

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, f_in);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, f_in);

	height = infoHeader.biHeight;
	width = infoHeader.biWidth;



	int LineByteCnt = (((width * infoHeader.biBitCount) + 31) >> 5) << 2;
	//int ImageDataSize = LineByteCnt * height;
	PaddingSize = 4 - ((width * infoHeader.biBitCount) >> 3) & 3;
	pad = (BYTE*)malloc(sizeof(BYTE) * PaddingSize);
	RGB* img = (RGB*)malloc(sizeof(RGB) * height * width);

	if (infoHeader.biBitCount == 24) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int index = i * width + j;
				fread(&img[index], sizeof(RGB), 1, f_in);
			}
			if (PaddingSize != 0)
			{
				fread(pad, 1, PaddingSize, f_in);
			}
		}
	}
	else
	{
		printf("此程序不支持非24位图片");
		return NULL;
	}

	fclose(f_in);
	return img;
}

void save_bmp(const char* filename, RGB* img, int width, int height)
{
	FILE* f_out = fopen(filename, "wb");

	fileHeader.bfType = 0x4D42; // 'BM'
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	fileHeader.bfSize = fileHeader.bfOffBits + width * height * sizeof(RGB);
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;

	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = width * height * sizeof(RGB);
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	fwrite(&fileHeader, sizeof(fileHeader), 1, f_out);
	fwrite(&infoHeader, sizeof(infoHeader), 1, f_out);


	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			fwrite(&img[i * width + j], sizeof(RGB), 1, f_out);
		if (PaddingSize != 0)
		{
			fwrite(pad, 1, PaddingSize, f_out);
		}
	}
	fclose(f_out);
	return;
}
