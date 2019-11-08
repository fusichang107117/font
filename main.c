#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define FIND_STR "width ="
void test()
{
	FILE *fp;
	char line[1024];
	int i = 0;

	fp = fopen("MI_LanTing_Light_Camera_U16.txt", "rb");
	if (!fp) {
		printf("file open error\n");
		return;
	}

	while(!feof(fp)) {
		fgets(line, 1024, fp);  //读取一行
		if (strstr(line, FIND_STR)) {
			printf("******\n"); //输出
			while(!feof(fp)) {
				fgets(line, 1024, fp);
				int len = strlen(line);
				if (len == 2) {
					break;
				}
				for (i = 0; i < len; i++) {
					if (line[i] == '_')
						printf("0");
					else if (line[i] == 'X')
						printf("1");
				}
				printf("\n");
			}
			printf("======\n"); //输出
		}
	}
	fclose(fp);
}

void rotate()
{
	FILE *fp1, *fp2;
	char line[1024];
	int i = 0, j = 0;
	uint8_t tmp = 0;
	int row = 0, col = 0, index = 0;
	uint8_t data[128][128];
	uint8_t output[128][128];

	fp1 = fopen("MI_LanTing_Light_Camera_U16.txt", "rb");
	if (!fp1) {
		printf("file open error\n");
		return;
	}

	fp2 = fopen("output.ttf", "wb");
	if (!fp2) {
		printf("file open error\n");
		return;
	}

	while(!feof(fp1)) {
		fgets(line, 1024, fp1);  //读取一行
		if (strstr(line, FIND_STR)) {
			row = 0;
			index = 0;
			printf("******\n"); //输出
			while(!feof(fp1)) {
				fgets(line, 1024, fp1);
				int len = strlen(line);
				if (len == 2) {
					break;
				}
				col = 0;
				for (i = 0; i < len; i++) {
					if (line[i] == '_'){
						tmp = (tmp << 1) | 0x00;
						//printf("0");
					} else if (line[i] == 'X'){
						tmp = (tmp << 1) | 0x01;
						//printf("1");
					} else if (line[i] == ',') {
						data[row][col] = tmp;
						printf("0x%02x,", tmp);
						tmp = 0;
						col++;
					}
				}
				row++;
				printf("\n");
			}
			printf("===row: %d, col: %d===\n", row, col); //输出
			for (i = 0; i < row; i++) {
				for(j = 0; j < col; j++) {
					uint8_t x = data[i][j];
					printf("0x%02x,", x);
				}
				printf("\n");
			}
			printf("^^^^^^^^^^^^^^^^\n"); //输出
		}
	}
	fclose(fp1);
	fclose(fp2);
}

uint8_t g_array_2[8][8] = {
	{ 1,1,1,1,1,1,1,1,},
	{ 0,0,0,0,0,0,0,0,},
	{ 1,1,1,1,1,1,0,0,},
	{ 0,0,0,0,0,0,0,0,},
	{ 1,1,1,1,1,1,1,1,},
	{ 0,0,0,0,0,0,0,0,},
	{ 1,1,1,1,1,1,1,1,},
	{ 0,0,0,0,0,0,0,0,},
};

uint8_t g_array[128] = {
	1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
};

uint8_t reverse_byte(uint8_t c)
{
	uint8_t s = 0;
        int i;
        for (i = 0; i < 8; ++i) {
                s <<= 1;
                s |= c & 1;
                c >>= 1;
        }
        return s;
}

void rotate8x8_2(uint8_t (*array)[8], int w, int h)
{
	uint8_t tmp[8][8];
	uint8_t i = 0, j = 0;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			tmp[j][i] = array[i][j];
		}
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			array[i][j] = tmp[i][j];
		}
	}
}

void reverse_array(uint8_t *array, int length)
{
	int i;
	for (i = 0; i < length/2; i++) {
		uint8_t tmp = array[i];
		array[i] = array[length - 1 - i];
		array[length - 1 - i] = tmp;
	}
}

void rotate8x8(uint8_t *array, int col)
{
	uint8_t tmp[8][8];
	uint8_t i = 0, j = 0;

	//printf("===start===\n");
	for (i = 0; i < 8; i++) {
		reverse_array(array + i * col, 8);
		for (j = 0; j < 8; j++) {
			tmp[j][i] = array[i * col + j];
			//printf("%d", tmp[j][i]);
		}
		//printf("\n");
	}
	//printf("=========\n");
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			array[i * col + j] = tmp[i][j];
			//printf("%d", tmp[i][j]);
		}
		//printf("\n");
	}
	//printf("===end===\n");
}

void oled128x64_rotate(const char *file_path)
{
	FILE *fp1, *fp2;
	int i = 0, j = 0, k = 0;

	uint8_t input[128 * 64];
	uint8_t output[128 * 64 / 8];

	fp1 = fopen(file_path, "rb");
	if (!fp1) {
		printf("file open error\n");
		return;
	}

/*	fp2 = fopen("output.ttf", "wb");
	if (!fp2) {
		printf("file open error\n");
		return;
	}
*/
	fseek(fp1, 0, SEEK_END);

	int length = ftell(fp1);

	printf("file length: %d\n", length);

	uint8_t *data = malloc(length);

	fseek(fp1, 0, SEEK_SET);

	fread(data, length, 1, fp1);

	//printf("======raw=====\n");
	for (i = 0; i < 128; i++) {
/*		if (i % 8 == 0 && i != 0)
			printf("\n");*/
		for (j = 0; j < 64; j++) {
			if (*(data + i * 64 + j) >= 128){
				//printf("0");
				input[i * 64 + j] = 0;
			}else{
				input[i * 64 + j] = 1;
				//printf("1");
			}
/*			if (j % 7 == 0 && j != 0)
				printf(" ");*/
		}
		//printf("\n");
	}
	printf("\n");
	free(data);
	fclose(fp1);

/*	printf("======print raw=====\n");
	for (i = 0; i < 16; i++) {
		if (i % 8 == 0 && i != 0)
			printf("\n");
		for (j = 0; j < 64; j++) {
			if (j % 8 == 0 && j != 0)
				printf(" ");
			printf("%x", input[i * 64 + j]);
		}
		printf("\n");
	}
	printf("======end=====\n");*/


	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			rotate8x8(input + i * 8 * 64 + j * 8, 64);
		}
	}

#if 0
	printf("======rotate result=====\n");
	for (i = 0; i < 128; i++) {
		if (i % 8 == 0 && i != 0)
			printf("\n");
		for (j = 0; j < 64; j++) {
			if (j % 8 == 0 && j != 0)
				printf(" ");
			printf("%x", input[i * 64 + j]);
		}
		printf("\n");
	}
	printf("======end=====\n");
#endif

	printf("======byte result=====\n");
	uint8_t tmp = 0;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 64; j+=8) {
			for (k = 0; k < 8; k++) {
				tmp<<=1;
				tmp = tmp | input[i * 64 + j + k];
			}
			printf("0x%02x,", tmp);
		}
		printf("\n");
	}
	printf("======end=====\n");
}

int main(int argc, char const *argv[])
{
	/* code */

	oled128x64_rotate(argv[1]);

	return 0;

	int i,j;


	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			printf("%d", g_array[i * 8 +j]);
		}
		printf("\n");
	}

	rotate8x8(g_array, 8);
	rotate8x8(g_array + 64, 8);

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			printf("%d", g_array[i * 8 +j]);
		}
		printf("\n");
	}

	return 0;
}