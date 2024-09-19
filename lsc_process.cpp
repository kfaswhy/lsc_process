#include "lsc_process.h"

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;
int PaddingSize = 0;
BYTE* pad = NULL;

#define debug_mode  1

int height = 0;
int width = 0;

int lsc_width = 17; 
int lsc_height = 27;

int gain_ratio_perent = 40;
int color_gain_inter = 0;

LSC lsc_color = { 0 };
LSC lsc_lum = { 0 };
RGB_EDGE edge = { 0 };


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

	
	//图像分块并计算均值
	enblock(img, lsc_width, lsc_height);

	//查找最亮点
	edge = search_edge(img);
	
	//计算块通道增益
	calc_lsc();

	//执行校正
	do_lsc_cali(img);

	dump_lsc();

	return 0;
}


U8 gamma(U8 a)
{
	return (U8)((U32)355 * a / (a + 100));
}

void dump_lsc()
{
	long file_size = 0;

	// 计算新的文件大小
	long new_size = 0;
	
	for (S32 by = 0; by < lsc_height; ++by)
	{
		for (S32 bx = 0; bx < lsc_width; ++bx)
		{
			FILE* file = fopen("lsc_output.csv", "w");
			if (!file) {
				perror("Failed to open file");
				return;
			}

//LSC_R
#if 1
			fprintf(file, "LSC_R\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					int index = by * lsc_width + bx;
					fprintf(file, "%u", ((U64)lsc_color.r_gain[index] * lsc_lum.r_gain[index] + 512) >> 10);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//LSC_G
#if 1
			fprintf(file, "LSC_G\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					int index = by * lsc_width + bx;
					fprintf(file, "%u", ((U64)lsc_color.g_gain[index] * lsc_lum.g_gain[index] + 512) >> 10);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//LSC_B
#if 1
			fprintf(file, "LSC_B\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					int index = by * lsc_width + bx;
					fprintf(file, "%u", ((U64)lsc_color.b_gain[index] * lsc_lum.b_gain[index] + 512) >> 10);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//color_shading_R
#if 1
			fprintf(file, "color_shading_R\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					fprintf(file, "%u",	lsc_color.r_gain[by * lsc_width + bx]);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//color_shading_G
#if 1
			fprintf(file, "color_shading_G\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					fprintf(file, "%u", lsc_color.g_gain[by * lsc_width + bx]);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//color_shading_B
#if 1
			fprintf(file, "color_shading_B\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					fprintf(file, "%u", lsc_color.b_gain[by * lsc_width + bx]);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//luma_shading_R
#if 1
			fprintf(file, "luma_shading_R\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					fprintf(file, "%u", lsc_lum.r_gain[by * lsc_width + bx]);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//luma_shading_G
#if 1
			fprintf(file, "luma_shading_G\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					fprintf(file, "%u", lsc_lum.g_gain[by * lsc_width + bx]);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif

//luma_shading_B
#if 1
			fprintf(file, "luma_shading_B\n");
			for (S32 by = lsc_height - 1; by >= 0; --by) {
				for (S32 bx = 0; bx < lsc_width; ++bx) {
					fprintf(file, "%u", lsc_lum.b_gain[by * lsc_width + bx]);
					//if (bx < lsc_width - 1) 
					{
						fprintf(file, ",");
					}
				}
				fprintf(file, "\n");
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);

			// 计算新的文件大小
			new_size = file_size - 3;
			if (new_size < 0) new_size = 0;
			// 截断文件
			if (_chsize(_fileno(file), new_size) == -1) {
				perror("Error truncating file");
				fclose(file);
				return;
			}
			// 移动文件指针到文件末尾
			fseek(file, 0, SEEK_END);
			fprintf(file, "\n\n");
#endif
			fclose(file);
		}
	}
	LOG("Done.");
	return;
}
void do_lsc_cali(RGB* img)
{
	U32 lum = 0;
	U32 tmp = 0;
	U32 tmp_gain = 0;

	U32 x0, y0, a1, a2;

	U32 blk_height = height / lsc_height + 1;
	U32 blk_width = width / lsc_width + 1;

	//色彩校正
	for (S32 by = 0; by < lsc_height; ++by)
	{
		for (S32 bx = 0; bx < lsc_width; ++bx)
		{
			for (S32 y = by * (height / lsc_height + 1); (y < (by + 1) * (height / lsc_height + 1)) && (y < height); ++y)
			{
				for (S32 x = bx * (width / lsc_width + 1); (x < (bx + 1) * (width / lsc_width + 1)) && (x < width); ++x)
				{
					if (color_gain_inter == 1)
					{
						U32 gain1, gain2, gain3, gain4;

						//中心区域
						if (y >= blk_height / 2 && y <= ((lsc_height - 1) * blk_height + blk_height / 2) &&
							x >= blk_width / 2 && x <= ((lsc_width - 1) * blk_width + blk_width / 2))
						{
							U32 x_index = (x - blk_width / 2) / blk_width;
							U32 y_index = (y - blk_height / 2) / blk_height;
							
							gain1 = lsc_color.r_gain[y_index * lsc_width + x_index];
							gain2 = lsc_color.r_gain[y_index * lsc_width + x_index+1];
							gain3 = lsc_color.r_gain[(y_index + 1) * lsc_width + x_index];
							gain4 = lsc_color.r_gain[(y_index + 1) * lsc_width + x_index + 1];
							x0 = x - x_index * blk_width - blk_width / 2;
							y0 = y - y_index * blk_height - blk_height / 2;
							a1 = x0 * (gain2 - gain1) / blk_width + gain1;
							a2 = x0 * (gain4 - gain3) / blk_width + gain3;
							tmp_gain = y0 * (a2 - a1) / blk_height + a1;
							img[y * width + x].r = calc_min(img[y * width + x].r * tmp_gain / 1024, 255);

							gain1 = lsc_color.g_gain[y_index * lsc_width + x_index];
							gain2 = lsc_color.g_gain[y_index * lsc_width + x_index + 1];
							gain3 = lsc_color.g_gain[(y_index + 1) * lsc_width + x_index];
							gain4 = lsc_color.g_gain[(y_index + 1) * lsc_width + x_index + 1];
							x0 = x - x_index * blk_width - blk_width / 2;
							y0 = y - y_index * blk_height - blk_height / 2;
							a1 = x0 * (gain2 - gain1) / blk_width + gain1;
							a2 = x0 * (gain4 - gain3) / blk_width + gain3;
							tmp_gain = y0 * (a2 - a1) / blk_height + a1;
							img[y * width + x].g = calc_min(img[y * width + x].g * tmp_gain / 1024, 255);

							gain1 = lsc_color.b_gain[y_index * lsc_width + x_index];
							gain2 = lsc_color.b_gain[y_index * lsc_width + x_index + 1];
							gain3 = lsc_color.b_gain[(y_index + 1) * lsc_width + x_index];
							gain4 = lsc_color.b_gain[(y_index + 1) * lsc_width + x_index + 1];
							x0 = x - x_index * blk_width - blk_width / 2;
							y0 = y - y_index * blk_height - blk_height / 2;
							a1 = x0 * (gain2 - gain1) / blk_width + gain1;
							a2 = x0 * (gain4 - gain3) / blk_width + gain3;
							tmp_gain = y0 * (a2 - a1) / blk_height + a1;
							img[y * width + x].b = calc_min(img[y * width + x].b * tmp_gain / 1024, 255);
						}




						//边缘判断

						if (y < height / lsc_height / 2)
						{
							//gain1=gain3;
							//gain2=gain4;
						}
						if (x < width / lsc_width / 2)
						{
							//gain1=gain2;
							//gain3=gain4;
						}

					}
					else
					{
						tmp = img[y * width + x].r * lsc_color.r_gain[by * lsc_width + bx] / 1024;
						img[y * width + x].r = calc_min(tmp, 255);
						
						tmp = img[y * width + x].g * lsc_color.g_gain[by * lsc_width + bx] / 1024;
						img[y * width + x].g = calc_min(tmp, 255);

						tmp = img[y * width + x].b * lsc_color.b_gain[by * lsc_width + bx] / 1024;
						img[y * width + x].b = calc_min(tmp, 255);
					}
				}
			}
		}
	}
	save_bmp("5_color_cali.bmp", img, width, height);

	//亮度校正
	for (S32 by = 0; by < lsc_height; ++by)
	{
		for (S32 bx = 0; bx < lsc_width; ++bx)
		{
			for (S32 y = by * (height / lsc_height + 1); (y < (by + 1) * (height / lsc_height + 1)) && (y < height); ++y)
			{
				for (S32 x = bx * (width / lsc_width + 1); (x < (bx + 1) * (width / lsc_width + 1)) && (x < width); ++x)
				{
					tmp = img[y * width + x].r * lsc_lum.r_gain[by * lsc_width + bx] / 1024;
					img[y * width + x].r = calc_min(tmp, 255);

					tmp = img[y * width + x].g * lsc_lum.g_gain[by * lsc_width + bx] / 1024;
					img[y * width + x].g = calc_min(tmp, 255);

					tmp = img[y * width + x].b * lsc_lum.b_gain[by * lsc_width + bx] / 1024;
					img[y * width + x].b = calc_min(tmp, 255);
				}
			}
		}
	}

	save_bmp("6_y_cali.bmp", img, width, height);
	LOG("Done.");
}

void calc_lsc()
{
#ifdef debug_mode
	U32 color_shading_max = 0;
	U32 luma_shading_max = 0;
#endif
	//计算 shading
	U32 max_color = 0;
	//U32 r_gain, b_gain, g_gain;
	//float lum_ratio;

	for (S32 by = 0; by < lsc_height; ++by) 
	{
		for (S32 bx = 0; bx < lsc_width; ++bx) 
		{
			int index = by * lsc_width + bx;

			//r_gain = edge.max.r / lsc_lum.r_gain[index];
			if (edge.max.r >= edge.max.g && edge.max.r >= edge.max.b)
			{
				//主色彩是红色
				//lum_ratio = (float)lsc_lum.r_gain[index] / edge.max.r;
				lsc_color.r_gain[index] = U10_FACTOR;
				lsc_color.g_gain[index] = (U32)U10_FACTOR * edge.max.g * lsc_lum.r_gain[index] / edge.max.r / lsc_lum.g_gain[index];
				lsc_color.b_gain[index] = (U32)U10_FACTOR * edge.max.b * lsc_lum.r_gain[index] / edge.max.r / lsc_lum.b_gain[index];
				max_color = lsc_lum.r_gain[index];
				lsc_lum.r_gain[index] = ((edge.max.r - max_color) * gain_ratio_perent / 100 + max_color) * 1024 / max_color;
				lsc_lum.g_gain[index] = lsc_lum.r_gain[index];
				lsc_lum.b_gain[index] = lsc_lum.r_gain[index];

			}
			else if(edge.max.g >= edge.max.r && edge.max.g >= edge.max.b)
			{
				//主色彩是绿色
				//lum_ratio = (float)lsc_lum.g_gain[index] / edge.max.r;
				lsc_color.g_gain[index] = U10_FACTOR;
				lsc_color.r_gain[index] = (U32)U10_FACTOR * edge.max.r * lsc_lum.g_gain[index] / edge.max.g / lsc_lum.r_gain[index];
				lsc_color.b_gain[index] = (U32)U10_FACTOR * edge.max.b * lsc_lum.g_gain[index] / edge.max.g / lsc_lum.b_gain[index];
				max_color = lsc_lum.g_gain[index];
				lsc_lum.r_gain[index] = ((edge.max.g - max_color) * gain_ratio_perent / 100 + max_color) * 1024 / max_color;
				lsc_lum.g_gain[index] = lsc_lum.r_gain[index];
				lsc_lum.b_gain[index] = lsc_lum.r_gain[index];

			}
			else if (edge.max.b >= edge.max.r && edge.max.b >= edge.max.g)
			{
				lsc_color.b_gain[index] = U10_FACTOR;
				lsc_color.g_gain[index] = (U32)U10_FACTOR * edge.max.g * lsc_lum.b_gain[index] / edge.max.b / lsc_lum.g_gain[index];
				lsc_color.r_gain[index] = (U32)U10_FACTOR * edge.max.r * lsc_lum.b_gain[index] / edge.max.b / lsc_lum.r_gain[index];
				max_color = lsc_lum.b_gain[index];
				lsc_lum.r_gain[index] = ((edge.max.b - max_color) * gain_ratio_perent / 100 + max_color) * 1024 / max_color;
				lsc_lum.g_gain[index] = lsc_lum.r_gain[index];
				lsc_lum.b_gain[index] = lsc_lum.r_gain[index];
			}

#ifdef debug_mode
			color_shading_max = calc_max(color_shading_max, lsc_color.r_gain[index]);
			color_shading_max = calc_max(color_shading_max, lsc_color.g_gain[index]);
			color_shading_max = calc_max(color_shading_max, lsc_color.b_gain[index]);

			//luma_gain 三个通道相同，
			luma_shading_max = calc_max(luma_shading_max, lsc_lum.r_gain[index]);
#endif
		}
	}


#ifdef debug_mode
	// 输出color_shading
	RGB* tmp_color = (RGB*)malloc(sizeof(RGB) * height * width);
	RGB* tmp_luma = (RGB*)malloc(sizeof(RGB) * height * width);
	U32 tmp_channel = 0;
	if (tmp_color == NULL)
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
					tmp_channel = lsc_color.r_gain[by * lsc_width + bx] * 255 / color_shading_max;
					tmp_color[y * width + x].r = calc_min(255, tmp_channel);
					tmp_color[y * width + x].r = gamma(tmp_color[y * width + x].r);

					tmp_channel = lsc_color.g_gain[by * lsc_width + bx] * 255 / color_shading_max;
					tmp_color[y * width + x].g = calc_min(255, tmp_channel);
					tmp_color[y * width + x].g = gamma(tmp_color[y * width + x].g);

					tmp_channel = lsc_color.b_gain[by * lsc_width + bx] * 255 / color_shading_max;
					tmp_color[y * width + x].b = calc_min(255, tmp_channel);
					tmp_color[y * width + x].b = gamma(tmp_color[y * width + x].b);

					tmp_channel = lsc_lum.r_gain[by * lsc_width + bx] * 255 / luma_shading_max;
					tmp_luma[y * width + x].r = calc_min(255, tmp_channel);
					tmp_luma[y * width + x].g = tmp_luma[y * width + x].r;
					tmp_luma[y * width + x].b = tmp_luma[y * width + x].r;

				}
			}
		}
	}
	save_bmp("3_color_gain.bmp", tmp_color, width, height);
	save_bmp("4_luma_gain.bmp", tmp_luma, width, height);

#endif

	LOG("Done.");
	return;
}

S32 enblock(RGB* img, S32 lsc_width, S32 lsc_height)
{

	lsc_lum.r_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc_lum.g_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc_lum.b_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc_color.r_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc_color.g_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	lsc_color.b_gain = (U32*)malloc(sizeof(U32) * lsc_height * lsc_width);
	if (!lsc_lum.r_gain || !lsc_lum.g_gain || !lsc_lum.b_gain) {
		free(lsc_lum.r_gain);
		free(lsc_lum.g_gain);
		free(lsc_lum.b_gain);
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
					img[y * width + x].r = calc_max(1, img[y * width + x].r);
					img[y * width + x].g = calc_max(1, img[y * width + x].g);
					img[y * width + x].b = calc_max(1, img[y * width + x].b);
					++count;
				}
			}

			U16 avg_r = sum_r / count;
			U16 avg_g = sum_g / count;
			U16 avg_b = sum_b / count;

			lsc_lum.r_gain[by * lsc_width + bx] = calc_max(1,avg_r);
			lsc_lum.g_gain[by * lsc_width + bx] = calc_max(1,avg_g);
			lsc_lum.b_gain[by * lsc_width + bx] = calc_max(1,avg_b);
		}
	}

#ifdef debug_mode
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
					tmp[y * width + x].r = lsc_lum.r_gain[by * lsc_width + bx];
					tmp[y * width + x].g = lsc_lum.g_gain[by * lsc_width + bx];
					tmp[y * width + x].b = lsc_lum.b_gain[by * lsc_width + bx];
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

RGB_EDGE search_edge(RGB* img)
{
	U32 max = 0;
	U32 min = 255;
	U32 pos_x_max = 0, pos_y_max = 0;
	U32 pos_x_min = 0, pos_y_min = 0;
	RGB* pixel = img;
	U32 lum = 0;

	for (U32 y = 0; y < lsc_height; y++)
	{
		for (U32 x = 0; x < lsc_width; x++)
		{
			lum = lsc_lum.r_gain[y * lsc_width + x] * 3 + lsc_lum.g_gain[y * lsc_width + x] * 6 + lsc_lum.b_gain[y * lsc_width + x];
			lum /= 10;
			if (lum > max)
			{
				max = lum;
				pos_x_max = x;
				pos_y_max = y;
			}

			if (lum < min) {
				min = lum;
				pos_x_min = x;
				pos_y_min = y;
			}

			pixel++;
		}
	}

	RGB_EDGE tmp = { 0 };
	tmp.min.r = lsc_lum.r_gain[pos_y_min * lsc_width + pos_x_min];
	tmp.min.g = lsc_lum.g_gain[pos_y_min * lsc_width + pos_x_min];
	tmp.min.b = lsc_lum.b_gain[pos_y_min * lsc_width + pos_x_min];
	tmp.max.r = lsc_lum.r_gain[pos_y_max * lsc_width + pos_x_max];
	tmp.max.g = lsc_lum.g_gain[pos_y_max * lsc_width + pos_x_max];
	tmp.max.b = lsc_lum.b_gain[pos_y_max * lsc_width + pos_x_max];

	LOG("Max value: %u at position (%u, %u) with RGB(%u, %u, %u).", max, pos_x_max, pos_y_max,
		tmp.max.r, tmp.max.g, tmp.max.b);
	LOG("Min value: %u at position (%u, %u) with RGB(%u, %u, %u).", min, pos_x_min, pos_y_min,
		tmp.min.r, tmp.min.g, tmp.min.b);

	return tmp;
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
