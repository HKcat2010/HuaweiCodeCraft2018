#ifndef MAPPING_H 
#define MAPPING_H 1

#include "limits.h"

#define MIN(x,y) ((x)<(y)?(x):(y))  //���ű�֤�������ȼ�

//hash ���� ���ڿ�����ѹ���ռ�
//MAP �� ����txt �����±�һֱ������ʵ�ʱ��[0]��һ��
#define CAR_ORD_SUB(x)  (x-car_order_min)  //���ƺ�    ->  �����±�
#define CAR_SUB_ORD(x)  (x+car_order_min)  //�����±�  ->  ���ƺ�
#define ROAD_ORD_SUB(x) (x-road_order_min)  //·��   ->  �����±�
#define ROAD_SUB_ORD(x) (x+road_order_min)  //�����±�   ->   ·��
#define CROSS_ORD_SUB(x) (x-cross_order_min)  //·�ڱ��   ->   �����±�
#define CROSS_SUB_ORD(x) (x+cross_order_min)  //�����±�   ->   ·�ڱ��
#define SPEED_REAL_SUB(x) (x-car_speed_min)  //��ʵ�ٶ�   ->   �ٶ��±�
#define SPEED_SUB_REAL(x) (x+car_speed_min)  //�ٶ��±�   ->   ��ʵ�ٶ�

//�ֽ׶������ռ� ��ȡʱ�䣬�ռ䲻�� ����hash

struct RouteRoad  //;��·��
{
	int road_num = 0;
	struct RouteRoad *next = NULL;
};

struct Route   //��·
{
	int total_road_cost = INT_MAX;   //����/�ٶ� = �����ܴ��� 
	struct RouteRoad *Road;
};

extern struct Route ***MAP;

void Mapping(int **cross, int **road);
void ShowRouteRoad(struct RouteRoad *r); //��ʾ·��
void CopyRouteRoad(struct RouteRoad *p1, struct RouteRoad *p2); //��r2·�����Ƶ�r1��
void AddRouteRoad(struct RouteRoad *p1, struct RouteRoad *p2); //��r2·�� ���� ���ص�r1ĩβ
void MAP_Floyd();

#endif