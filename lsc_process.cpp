#include "lsc_process.h"

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;
int PaddingSize = 0;
BYTE* pad = NULL;

int height = 0;
int width = 0;

int blk_width = 17;
int blk_height = 27;

U16* lsc_r = NULL;
U16* lsc_g = NULL;
U16* lsc_b = NULL;

int main()
{

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
	int max = search_max(img);
	

	//图像分块并计算均值
	enblock(img, blk_width, blk_height);
	save_bmp("2.bmp", img, width, height);


	return 0;
}


S32 calc_lsc(RGB* img)
{
	return 0;
}

S32 enblock(RGB* img, S32 blk_width, S32 blk_height)
{

	lsc_r = (U16*)malloc(sizeof(U16) * blk_height * blk_width);
	lsc_g = (U16*)malloc(sizeof(U16) * blk_height * blk_width);
	lsc_b = (U16*)malloc(sizeof(U16) * blk_height * blk_width);
	if (!lsc_r || !lsc_g || !lsc_b) {
		free(lsc_r);
		free(lsc_g);
		free(lsc_b);
		return -1; // 内存分配失败
	}

	for (S32 by = 0; by < blk_height; ++by) {
		for (S32 bx = 0; bx < blk_width; ++bx) {
			U32 sum_r = 0, sum_g = 0, sum_b = 0;
			S32 count = 0;

			for (S32 y = by * (height / blk_height); y < (by + 1) * (height / blk_height); ++y) {
				for (S32 x = bx * (width / blk_width); x < (bx + 1) * (width / blk_width); ++x) {
					sum_r += img[y * width + x].r;
					sum_g += img[y * width + x].g;
					sum_b += img[y * width + x].b;
					++count;
				}
			}

			U16 avg_r = sum_r / count;
			U16 avg_g = sum_g / count;
			U16 avg_b = sum_b / count;

			lsc_r[by * blk_width + bx] = avg_r;
			lsc_g[by * blk_width + bx] = avg_g;
			lsc_b[by * blk_width + bx] = avg_b;

			// 将该块内的所有像素值替换成该块的均值
			for (S32 y = by * (height / blk_height); y < (by + 1) * (height / blk_height); ++y) {
				for (S32 x = bx * (width / blk_width); x < (bx + 1) * (width / blk_width); ++x) {
					img[y * width + x].r = avg_r;
					img[y * width + x].g = avg_g;
					img[y * width + x].b = avg_b;
				}
			}
		}
	}

	free(lsc_r);
	free(lsc_g);
	free(lsc_b);

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
