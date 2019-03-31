#include "stdio.h"
#include "Mapping.h"
#include "Read.h"
#include <iostream> //����ע�ͣ�

using namespace std;

struct Route ***MAP; //��ͼ3ά [����][���id][�յ�id]

void map_building(int **cross, int **road)
{
	if ((MAP = (struct Route***)malloc(car_speed_cnt * sizeof(struct Route**))) == NULL)
	{
		fprintf(stderr, "No Enough Space!");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < car_speed_cnt; i++)
	{
		if ((MAP[i] = (struct Route**)malloc(cross_total_num * sizeof(struct Route*))) == NULL) //���
		{
			fprintf(stderr, "No Enough Space!");
			exit(EXIT_FAILURE);
		}
		for (int j = 0; j < cross_total_num; j++)                                               //�յ�
			if ((MAP[i][j] = (struct Route*) malloc(cross_total_num * sizeof(struct Route))) == NULL)
			{
				fprintf(stderr, "No Enough Space!");
				exit(EXIT_FAILURE);
			}
	}

	//��ʼ��
	for (int speed_cnt = 0; speed_cnt < car_speed_cnt; speed_cnt++) 
		for (int i = 0; i < cross_total_num; i++)
		{
			for (int j = 0; j < cross_total_num; j++)
			{
				if(i==j)MAP[speed_cnt][i][j].total_road_cost = 0; //�ڵ㵽�������Ϊ0
				else MAP[speed_cnt][i][j].total_road_cost = INT_MAX;  //��������������޴� �ȴ�ˢ��
				MAP[speed_cnt][i][j].finished = 0;      //δ���·����־λ
				if ((MAP[speed_cnt][i][j].Road = (struct RouteRoad *)malloc(sizeof(struct RouteRoad))) == NULL)
				{
					fprintf(stderr, "No Enough Space for New RouteRoad !");
					exit(EXIT_FAILURE);
				}
				MAP[speed_cnt][i][j].Road->road_num = INT_MAX;
				MAP[speed_cnt][i][j].Road->next = NULL;
			}
		}
	//��ʼ����ͼ���õ�ÿһ�����ڽ����ͨ�д���
	for (int speed_cnt = 0; speed_cnt < car_speed_cnt; speed_cnt++) //��ͬ�ٶ� 
		for (int i = 0; i < road_total_num; i++)  // ÿ��·��
		{
			//���´���       //����  ӳ�亯�� һ��ע�� [i] �ı仯������������һ��ע�⣡����������һ��ע�⣡����������  //ͨ�д�·���Ѵ���  ���� / ����� 
			MAP[speed_cnt][CROSS_ORD_SUB(road[i][4])][CROSS_ORD_SUB(road[i][5])].total_road_cost = (road[i][1] * 10) / MIN(SPEED_SUB_REAL(speed_cnt), road[i][2]);
			MAP[speed_cnt][CROSS_ORD_SUB(road[i][4])][CROSS_ORD_SUB(road[i][5])].Road->road_num = road[i][0]; //��¼��·����
			
			/*cout << road[i][4] << " --> " << road[i][5] << ": " << MAP[speed_cnt][CROSS_ORD_SUB(road[i][4])][CROSS_ORD_SUB(road[i][5])].Road->road_num <<":"
				 << MAP[speed_cnt][CROSS_ORD_SUB(road[i][4])][CROSS_ORD_SUB(road[i][5])].total_road_cost <<endl*/
			if (road[i][6] == 1) //˫��
			{
				MAP[speed_cnt][CROSS_ORD_SUB(road[i][5])][CROSS_ORD_SUB(road[i][4])].total_road_cost = MAP[speed_cnt][CROSS_ORD_SUB(road[i][4])][CROSS_ORD_SUB(road[i][5])].total_road_cost;
				MAP[speed_cnt][CROSS_ORD_SUB(road[i][5])][CROSS_ORD_SUB(road[i][4])].Road->road_num = road[i][0]; //��¼��·����

				/*cout << road[i][5] << " --> " << road[i][4] << ": " << MAP[speed_cnt][CROSS_ORD_SUB(road[i][5])][CROSS_ORD_SUB(road[i][4])].Road->road_num << ":"
					<< MAP[speed_cnt][CROSS_ORD_SUB(road[i][5])][CROSS_ORD_SUB(road[i][4])].total_road_cost << endl;*/
			}
		}
}

void ShowRouteRoad(struct RouteRoad *r) //��ʾ·��
{
	while(r != NULL)
	{
		cout << r->road_num << " -> ";
		r = r->next;
	} 
}

void CopyRouteRoad(struct RouteRoad *p1, struct RouteRoad *p2) //��r2·�����Ƶ�r1��
{
	struct RouteRoad *tmp = NULL, *lastp1 = NULL;
	while (p2 != NULL) //˳��r2����·��
	{
		p1->road_num = p2->road_num;
		if (p1->next == NULL && p2->next != NULL)
		{
			if ((p1->next = (struct RouteRoad *)malloc(sizeof(struct RouteRoad))) == NULL)
			{
				fprintf(stderr, "No Enough Space for New Road !");
				exit(EXIT_FAILURE);
			}
			p1->next->next = NULL;
		}
		lastp1 = p1; //��¼p1����������
		p1 = p1->next;
		p2 = p2->next;
	}
   //���ƽ���ʱr1����δ��յ�·��
	while (p1 != NULL )
	{
		tmp = p1->next;
		p1-> next = NULL;
		free(p1);
		p1 = tmp;
	}
	lastp1->next = NULL;//���һ��ע��ָ����NULL
}
void AddRouteRoad(struct RouteRoad *p1, struct RouteRoad *p2) //��r2·�� ���� ���ص�r1ĩβ
{
	while (p1->next != NULL)p1 = p1->next;  //ֱ��r1·�������
	while (p2 != NULL) //˳��r2����·��
	{
		if ((p1->next = (struct RouteRoad *)malloc(sizeof(struct RouteRoad))) == NULL)
			{
				fprintf(stderr, "No Enough Space for New Road !");
				exit(EXIT_FAILURE);
			}
		p1 = p1->next;
		p1->road_num = p2->road_num;
		p1->next = NULL;
		p2 = p2->next;
	}
}

void MAP_Floyd()
{
	for (int speed_cnt = 0; speed_cnt < car_speed_cnt; speed_cnt++)    //��ͬ�ٶ�  
		for (int mid = 0; mid < cross_total_num; mid++)
			for (int i = 0; i < cross_total_num; i++)                  // ÿ�����
				for (int j = 0; j < cross_total_num; j++)              // ÿ���յ�
					if (INT_MAX - MAP[speed_cnt][i][mid].total_road_cost > MAP[speed_cnt][mid][j].total_road_cost)//ע�������� 
						if (MAP[speed_cnt][i][j].total_road_cost > MAP[speed_cnt][i][mid].total_road_cost + MAP[speed_cnt][mid][j].total_road_cost)
						{   
							//��·�ߴ��۸�С   �޸ĸ���·��
							MAP[speed_cnt][i][j].total_road_cost = MAP[speed_cnt][i][mid].total_road_cost + MAP[speed_cnt][mid][j].total_road_cost;
							CopyRouteRoad(MAP[speed_cnt][i][j].Road, MAP[speed_cnt][i][mid].Road);
							AddRouteRoad(MAP[speed_cnt][i][j].Road, MAP[speed_cnt][mid][j].Road);
							//if (i == 13 && j == 11)
							//{
							//	cout << CROSS_SUB_ORD(i) << " -> " << CROSS_SUB_ORD(j) << "  at Speed " << SPEED_SUB_REAL(speed_cnt) << " : " << endl;
							//	ShowRouteRoad(MAP[speed_cnt][i][j].Road);
							//	cout << " TimeCost : " << MAP[speed_cnt][i][j].total_road_cost << endl;
							//}
						}
}

