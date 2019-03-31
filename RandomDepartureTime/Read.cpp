#include "Read.h"
#include <fstream>
#include <stdio.h>

using namespace std;
//����ʵ�ʴ�txt�ж���ĳ����� 
#define MAX_CAR_NUM  15000
#define MAX_CROSS_NUM 100
#define MAX_ROAD_NUM  200

int car_order_min = MAX_CAR_NUM;
int car_order_max = 0;
int car_total_num = 0;   //�ܳ�����
int road_order_min = INT_MAX;
int road_order_max = 0;
int road_total_num = 0;   //�ܵ�·��
int cross_order_min = INT_MAX;
int cross_order_max = 0;
int cross_total_num = 0;   //��·����Ŀ
int car_speed_min = INT_MAX;   //�������ܵ���ʻ�ٶ�
int car_speed_max = 0;
int car_speed_cnt = 0;    

int **GetCar(std::string address)
{
	char *add = (char*)address.data();
	char *ch = NULL;
	long int byte_num = 0;
	FILE *car_txt;
	if ((car_txt = fopen(add, "r")) == NULL)
	{
		fprintf(stderr, "Open Error!");
		exit(EXIT_FAILURE);
	}
	//��ȡ�ļ��ֽ���  EOF ���ȥ�� ??
	fseek(car_txt, 0L, SEEK_END);
	byte_num = ftell(car_txt);
	fseek(car_txt, 0L, SEEK_SET);  //Ҫ���ػ�������ͷ

	if ((ch = (char *)malloc(sizeof(char) * byte_num)) == NULL)
	{
		fprintf(stderr, "Malloc Error!");
		exit(EXIT_FAILURE);
	}
	if (fread(ch, sizeof(char), byte_num, car_txt) == byte_num)
		fprintf(stdout, "Car Read Successfully!\n");

	//----���洢����-----
	int **car;
	car = (int **)malloc(sizeof(int*) * MAX_CAR_NUM);  //���ﳵ�������Ը���Ϊ ��̬����  
	for (int i = 0; i < MAX_CAR_NUM; i++)
	{
		car[i] = (int *)malloc(sizeof(int) * 5);
		for (int j = 0; j < 5; j++)
			car[i][j] = 0;
	}
	//-------------------��Ҫ���---------
	long int num = 0;
	unsigned char info = 0, cnt = 0;
	int car_cnt = 0;
	char char2num[15] = { 0 };

	while (num < byte_num - 5)
	{
		if (ch[num] == '#')
			while (ch[num] != '\n')
				num++;
		if (ch[num - 2] == '\n' && ch[num - 1] == '(') //���� + ( ��ͷ
		{
			info = 0;
			//-----��Ϣ���� 0�����  1���  2�յ�  3�ٶ�  4�ƻ�����ʱ��-----
			while (ch[num] != '\n')
			{
				if (ch[num] >= '0' && ch[num] <= '9')
				{
					cnt = 0;
					while (ch[num] != ',' && ch[num] != ')')  //����) ��Ϣ�ɼ��Ľ���
					{
						char2num[cnt++] = ch[num++];
						if (cnt == 14) break;
					}
					if (cnt == 14) break;         //Խ�籣��
					if (num == byte_num) break;   //Խ�籣��
					for (int i = 0; i < cnt; i++)
						car[car_cnt][info] = char2num[i] - '0' + car[car_cnt][info] * 10;
					//printf("%d   ", car[car_cnt][info]);
					info++;
				}
				else
					num++;
			}
			//���³������Сֵ ���ֵ
			if (car[car_cnt][0] < car_order_min) car_order_min = car[car_cnt][0];
			if (car[car_cnt][0] > car_order_max) car_order_max = car[car_cnt][0];
			if (car[car_cnt][3] > car_speed_max) car_speed_max = car[car_cnt][3];
			if (car[car_cnt][3] < car_speed_min) car_speed_min = car[car_cnt][3];
			//printf("\n");
			car_cnt++;
		}
		else
			num++;
	}
	car_speed_cnt = car_speed_max - car_speed_min + 1;
	car_total_num = car_cnt; //��0���� }}ǰ+1 ����
	free(ch);
	return car;
}

int **GetRoad(std::string address)
{
	char *add = (char*)address.data();
	char *ch = NULL;
	long int byte_num = 0;
	FILE *road_txt;
	if ((road_txt = fopen(add, "r")) == NULL)
	{
		fprintf(stderr, "Open Error!");
		exit(EXIT_FAILURE);
	}
	//��ȡ�ļ��ֽ���  EOF ���ȥ�� ??
	fseek(road_txt, 0L, SEEK_END);
	byte_num = ftell(road_txt);
	fseek(road_txt, 0L, SEEK_SET);  //Ҫ���ػ�������ͷ

	if ((ch = (char *)malloc(sizeof(char) * byte_num)) == NULL)
	{
		fprintf(stderr, "Malloc Error!");
		exit(EXIT_FAILURE);
	}
	if (fread(ch, sizeof(char), byte_num, road_txt) == byte_num)
		fprintf(stdout, "Road Read Successfully!\n");

	//----���洢����-----
	int **road;
	road = (int **)malloc(sizeof(int*) * MAX_ROAD_NUM);  //���ﳵ�������Ը���Ϊ ��̬����  
	for (int i = 0; i < MAX_ROAD_NUM; i++)
	{
		road[i] = (int *)malloc(sizeof(int) * 7);
		for (int j = 0; j < 7; j++)
			road[i][j] = 0;
	}
	//-------------------��Ҫ���---------
	long int num = 0;
	unsigned char info = 0, cnt = 0;
	int road_cnt = 0;
	char char2num[15] = { 0 };

	while (num < byte_num - 5)
	{
		if (ch[num] == '#')
			while (ch[num] != '\n')
				num++;
		if (ch[num - 2] == '\n' && ch[num - 1] == '(') //���� + ( ��ͷ
		{
			info = 0;
			//-----��Ϣ���� 0·���  1��  2�ٶ�  3������  4��ʼ�ڵ�  5��ֹ�ڵ�  6˫����-----
			while (ch[num] != '\n')
			{
				if (ch[num] >= '0' && ch[num] <= '9')
				{
					cnt = 0;
					while (ch[num] != ',' && ch[num] != ')')  //����) ��Ϣ�ɼ��Ľ���
					{
						char2num[cnt++] = ch[num++];
						if (cnt == 14) break;
					}
					if (cnt == 14) break;         //Խ�籣��
					if (num == byte_num) break;   //Խ�籣��
					for (int i = 0; i < cnt; i++)
						road[road_cnt][info] = char2num[i] - '0' + road[road_cnt][info] * 10;
					//printf("%d   ", road[road_cnt][info]);
					info++;
				}
				else
					num++;
			}
			//�������Сֵ ���ֵ
			if (road[road_cnt][0] < road_order_min) road_order_min = road[road_cnt][0];
			if (road[road_cnt][0] > road_order_max) road_order_max = road[road_cnt][0];
			road_cnt++;
			//printf("\n");
		}
		else
			num++;
	}
	road_total_num = road_cnt; //��0���� }}ǰ+1 ����
	free(ch);
	return road;
}

int **GetCross(std::string address)
{
	char *add = (char*)address.data();
	char *ch = NULL;
	long int byte_num = 0;
	FILE *cross_txt;
	if ((cross_txt = fopen(add, "r")) == NULL)
	{
		fprintf(stderr, "Open Error!");
		exit(EXIT_FAILURE);
	}
	//��ȡ�ļ��ֽ���  EOF ���ȥ�� ??
	fseek(cross_txt, 0L, SEEK_END);
	byte_num = ftell(cross_txt);
	fseek(cross_txt, 0L, SEEK_SET);  //Ҫ���ػ�������ͷ

	if ((ch = (char *)malloc(sizeof(char) * byte_num)) == NULL)
	{
		fprintf(stderr, "Malloc Error!");
		exit(EXIT_FAILURE);
	}
	if (fread(ch, sizeof(char), byte_num, cross_txt) == byte_num)
		fprintf(stdout, "Cross Read Successfully!\n");

	//----���洢����-----
	int **cross;
	cross = (int **)malloc(sizeof(int*) * MAX_CROSS_NUM);  //���ﳵ�������Ը���Ϊ ��̬����  
	for (int i = 0; i < MAX_CROSS_NUM; i++)
	{
		cross[i] = (int *)malloc(sizeof(int) * 5);
		for (int j = 0; j < 5; j++)
			cross[i][j] = 0;
	}
	//-------------------��Ҫ���---------
	long int num = 0;
	unsigned char info = 0, cnt = 0;
	int cross_cnt = 0;
	char char2num[15] = { 0 };

	while (num < byte_num - 5)
	{
		if (ch[num] == '#')
			while (ch[num] != '\n')
				num++;
		if (ch[num - 2] == '\n' && ch[num - 1] == '(') //���� + ( ��ͷ
		{
			info = 0;
			//-----��Ϣ���� 0·�ڱ��  1��  2��  3��  4��-----
			while (ch[num] != '\n')
			{
				if ((ch[num] >= '0' && ch[num] <= '9') || ch[num] == '-')
				{
					cnt = 0;
					while (ch[num] != ',' && ch[num] != ')')  //����) ��Ϣ�ɼ��Ľ���
					{
						char2num[cnt++] = ch[num++];
						if (cnt == 14) break;
					}
					if (cnt == 14) break;         //Խ�籣��
					if (num == byte_num) break;   //Խ�籣��
					if (char2num[0] == '-') //"-1"��·���
						cross[cross_cnt][info] = -1;
					else
						for (int i = 0; i < cnt; i++)
							cross[cross_cnt][info] = char2num[i] - '0' + cross[cross_cnt][info] * 10;
					//printf("%d   ", cross[cross_cnt][info]);
					info++;
				}
				else
					num++;
			}
			//�������Сֵ ���ֵ
			if (cross[cross_cnt][0] < cross_order_min) cross_order_min = cross[cross_cnt][0];
			if (cross[cross_cnt][0] > cross_order_max) cross_order_max = cross[cross_cnt][0];
			cross_cnt++;
			//printf("\n");
		}
		else
			num++;
	}
	cross_total_num = cross_cnt; //��0���� }}ǰ+1 ����
	free(ch);
	return cross;
}

