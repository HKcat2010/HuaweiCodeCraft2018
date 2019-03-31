#ifndef READ_H
#define READ_H 1

#include <string>
#include "limits.h"

extern int car_order_min;
extern int car_order_max;
extern int car_total_num;   //�ܳ�����
extern int road_order_min;
extern int road_order_max;
extern int road_total_num;   //�ܵ�·��
extern int cross_order_min;
extern int cross_order_max;
extern int cross_total_num;   //��·����Ŀ
extern int car_speed_min;   //�������ܵ���ʻ�ٶ�
extern int car_speed_max;
extern int car_speed_cnt; 

int **GetCar(std::string address);
int **GetRoad(std::string address);
int **GetCross(std::string address);

#endif
