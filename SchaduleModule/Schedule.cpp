#include <iostream>
#include "Mapping.h"
#include "Read.h"
#include "Schedule.h"

//-------------
#define DUPLEX 1
#define SINGLE 0
//-------------
#define NO_ROAD -1 
#define NO_CAR -1
#define IS_CAR 1
#define UP_ROAD 0
#define RIGHT_ROAD 1
#define DOWN_ROAD 2
#define LEFT_ROAD 3
//------------- ·�ڳ����ķ���
#define GO_STRAIGHT 0
#define TURN_LEFT 1
#define TURN_RIGHT 2
//-------------
#define END_STATUS 0
#define WAIT_STATUS 1
//-------------  ���ڵ�·����
#define FORWARD 0
#define BACKWARD 1
//-------------
#define FINISHED 1
#define UNFINISHED 0
//--------------
#define NO_CONFLICT 0
#define CONFLICT 1

//ֱ�Ӹ�����������ӿ� ·�ڳ�ͻ�ж� ���޷���ͷ�趨ΪINT_MAX   Ҳ���±���� switch �õ�����
using namespace std;
const int DIRECTION_MAP[4][4] = {{INT_MAX,TURN_LEFT,GO_STRAIGHT,TURN_RIGHT},
                                 {TURN_RIGHT,INT_MAX,TURN_LEFT,GO_STRAIGHT},
                                 {GO_STRAIGHT,TURN_RIGHT,INT_MAX,TURN_LEFT},
                                 {TURN_LEFT,GO_STRAIGHT,TURN_RIGHT,INT_MAX} };


class CAR
{
public:
	int car_order = INT_MAX;
	int car_start_cross = INT_MAX;
	int car_end_cross = INT_MAX;
	int car_speed = INT_MAX;
	int car_start_time = INT_MAX;
	int car_status = WAIT_STATUS;
	int car_current_slot = INT_MAX;//������ǰλ��
	int car_next_slot = INT_MAX; //������һʱ�̵�λ��
	int car_current_speed = 0;   //��ǰ�����ϵ������ʻ�ٶ�
	int car_current_road = INT_MAX; //�������ڵ�·���
	int car_next_road = INT_MAX;  //��һ·�α��
	int car_current_direction = GO_STRAIGHT; //�洢��ǰ��ת��
	int car_schedule_finished = UNFINISHED;    //���Tʱ���ڵ��ȱ�־
	int car_route_finish = UNFINISHED;//��������Ŀ���־
	Route *car_real_route = NULL; //����ʵ����ʻ��·��

	CAR(int *car);
	static CAR **BuildEmptyCar(int car_num);
};
CAR ** CAR::BuildEmptyCar(int car_num)
{
	CAR ** tmp = NULL;
	if ((tmp = (CAR **)malloc(sizeof(CAR *) * car_num)) == NULL)
		fprintf(stderr, "No Space!!\n");
	for (int i = 0; i < car_num; i++)
		tmp[i] = NULL;
	return tmp;
}
CAR::CAR(int *car)
{
	this->car_order = car[0];
	this->car_start_cross = car[1];
	this->car_end_cross = car[2];
	this->car_speed = car[3];
	this->car_start_time = car[4];
	this->car_status = WAIT_STATUS;
	this->car_schedule_finished = UNFINISHED;    //���Tʱ���ڵ��ȱ�־
	this->car_current_slot = INT_MAX;
	this->car_route_finish = UNFINISHED;     //��������Ŀ���־
	this->car_current_direction = GO_STRAIGHT; //�洢��ǰ��ת��
	if ((this->car_real_route = (Route *)malloc(sizeof(Route))) == NULL)
		fprintf(stderr, "No Space!!\n"); 
	if ((this->car_real_route->Road = (RouteRoad *)malloc(sizeof(RouteRoad))) == NULL)
		fprintf(stderr, "No Space!!\n");
	car_real_route->Road->next = NULL; //����NULL �޷����� ���ǣ�������
	CopyRouteRoad(car_real_route->Road, MAP[SPEED_REAL_SUB(car_speed)][CROSS_ORD_SUB(car_start_cross)][CROSS_ORD_SUB(car_end_cross)].Road);//��r2·�����Ƶ�r1��
	car_real_route->total_road_cost = MAP[SPEED_REAL_SUB(car_speed)][CROSS_ORD_SUB(car_start_cross)][CROSS_ORD_SUB(car_end_cross)].total_road_cost;
}
//--car-- 0�����  1���  2�յ�  3�ٶ�  4�ƻ�����ʱ��-----
class ROAD
{
public :
	int road_ord = INT_MAX;
	int road_len = INT_MAX;
	int road_speed = INT_MAX;
	int road_start_cross = INT_MAX;
	int road_lane_num = INT_MAX;
	int road_end_cross = INT_MAX;
	int road_duplex = DUPLEX;
	int frontest_car_direction[2] = { NO_CAR ,NO_CAR};
	CAR ****car_slot = NULL;  //��·�ϳ�����λ
	ROAD(int *road);
	static ROAD **BuildEmptyRoad(int road_num);
};
ROAD ** ROAD::BuildEmptyRoad(int road_num)
{
	ROAD ** tmp = NULL;
	if ((tmp = (ROAD **)malloc(sizeof(ROAD *) * road_num)) == NULL)
		fprintf(stderr, "No Space!!\n");
	for (int i = 0; i < road_num; i++)
		tmp[i] = NULL;
	return tmp;
}
ROAD::ROAD(int *road)
{
	this->road_ord = road[0];
	this->road_len = road[1];
	this->road_speed = road[2];
	this->road_lane_num = road[3];
	this->road_start_cross = road[4];
	this->road_end_cross = road[5];
	this->road_duplex = road[6];
	this->frontest_car_direction[0] = NO_CAR;
	this->frontest_car_direction[0] = NO_CAR;
	if (this->road_duplex == DUPLEX) //˫�򳵵� 
	{
		if ((this->car_slot = (CAR ****)malloc(sizeof(CAR ***) * 2)) == NULL)
			fprintf(stderr, "No Space!!\n");   //˫��  0�� 1��
		for (int i = 0; i < 2; i++)
		{
			if ((this->car_slot[i] = (CAR ***)malloc(sizeof(CAR **) * this->road_lane_num)) == NULL)
				fprintf(stderr, "No Space!!\n");   //����
			for (int j = 0; j < road_lane_num; j++)
			{
				if ((this->car_slot[i][j] = (CAR **)malloc(sizeof(CAR *) * this->road_len)) == NULL)
					fprintf(stderr, "No Space!!\n");  //ÿ�������ϵĳ�����λ
				for (int k = 0; k < road_len; k++)
					car_slot[i][j][k] = NULL;
			}
		}
	}
	else    //���򳵵�
	{
		if ((this->car_slot = (CAR ****)malloc(sizeof(CAR ***))) == NULL)
			fprintf(stderr, "No Space!!\n");   //����  0��
		if ((this->car_slot[0] = (CAR ***)malloc(sizeof(CAR **) * this->road_lane_num)) == NULL)
				fprintf(stderr, "No Space!!\n");   //����
		for (int j = 0; j < road_lane_num; j++)
		{
			if ((this->car_slot[0][j] = (CAR **)malloc(sizeof(CAR *) * this->road_len)) == NULL)
				fprintf(stderr, "No Space!!\n");  //ÿ�������ϵĳ�����λ
			for (int k = 0; k < road_len; k++)
				car_slot[0][j][k] = NULL;
		}
	}
	//[len-1][len-2]...[0]  ·��[-1]
}
//--road---0·���  1��  2�ٶ�  3������  4��ʼ�ڵ�  5��ֹ�ڵ�  6˫����-----

class CROSS
{
public:
	int cross_ord = INT_MAX;
	int start_car_num = 0; //��¼�Ӵ�·�ڳ����ĳ�����Ŀ
	int *start_car = NULL; //������������
	int start_car_cnt = 0; //��������
	int cross_locked_time = 0;
	ROAD **road_in_cross = NULL; // 0��  1��  2��  3��
	int travel_order[4] = { INT_MAX }; //��·�������� �洢���Ƿ������ 
	CROSS(int *cross, ROAD **road);
	static CROSS **BuildEmptyCross(int cross_num);
};
//----------------------
CROSS ** CROSS::BuildEmptyCross(int cross_num)
{
	CROSS ** tmp = NULL;
	if ((tmp = (CROSS **)malloc(sizeof(CROSS *) * cross_num)) == NULL)
		fprintf(stderr, "No Space!!\n");
	for (int i = 0; i < cross_num; i++)
		tmp[i] = NULL;
	return tmp;
}
//-----------

CROSS::CROSS(int *cross, ROAD **road)
{
	this->start_car_num = 0;
	this->cross_ord = cross[0];
	this->start_car_cnt = 0;
	if ((this->road_in_cross = (ROAD **)malloc(sizeof(ROAD *) * 4)) == NULL)//ÿ��·�����ĸ�ָ��
		fprintf(stderr, "No Space!!\n");
	//--------��ű��ʼ��----------
	int ordertable[4][2] = {0};
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 2; j++)
			ordertable[i][j] = INT_MAX;
	//--------�������ڵ�·��Ϣ----------
	for (int cross_road_cnt = 0; cross_road_cnt < 4; cross_road_cnt++)
	{
		if (cross[cross_road_cnt + 1] == NO_ROAD) //�˷���û��Road
		{
			this->road_in_cross[cross_road_cnt] = NULL;                       //��·������ -> ָ��=NULL -> ���=INT_MAX
			ordertable[cross_road_cnt][0] = INT_MAX;
		}
		else
		{
			this->road_in_cross[cross_road_cnt] =
				road[ROAD_ORD_SUB(cross[cross_road_cnt + 1])]; //ָ��ָ����Ӧ��Road
			ordertable[cross_road_cnt][0] = road_in_cross[cross_road_cnt]->road_ord;
		}
		ordertable[cross_road_cnt][1] = cross_road_cnt;
	}
	//ð������ȷ������˳�� �����ڵĵ�·�������
	int tmp = 0; 
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3 - i; j++)
		{
			if (ordertable[j][0] > ordertable[j + 1][0])
			{
				tmp = ordertable[j + 1][0];  //�������
				ordertable[j + 1][0] = ordertable[j][0];
				ordertable[j][0] = tmp;
				tmp = ordertable[j + 1][1];  //�������
				ordertable[j + 1][1] = ordertable[j][1];
				ordertable[j][1] = tmp;
			}
		}
	for (int i = 0; i < 4; i++) //���� ·�ڷ��� // ��  ��  ��  ��  �޵�·
		this->travel_order[i] = ordertable[i][1]; 
}
void StartCarSort(CAR **car, CROSS *cross);//����ʱ��>���ð��
int RoadDirectionInCross(int road_ord,CROSS *cross); //��·��·���еķ�λ 0�� 1�� 2�� 3��
int TravelDirection(ROAD *road, CROSS *cross); //�����ķ���
int IntoRoadTravelDiraction(ROAD *road, CROSS *cross);//���복������������
int DirectionConflict(ROAD *road, CROSS *cross); //��road����cross�ĳ��� ת���ͻ�ж�
int FindCarNextRoad(int road, RouteRoad *r); //���뵱ǰ��·�õ���һ����·

void ScheduleModule(int **car, int **road, int **cross)
{
	CAR **Car = CAR::BuildEmptyCar(car_total_num);
	ROAD **Road = ROAD::BuildEmptyRoad(road_total_num); //����·��ָ������ռ�
	CROSS **Cross = CROSS::BuildEmptyCross(cross_total_num);  //����·��ָ������ռ�
	//��ʼ��·
	for (int road_cnt = 0; road_cnt < road_txt_num; road_cnt++)
		Road[ROAD_ORD_SUB(road[road_cnt][0])] = new ROAD(road[road_cnt]);
	//��ʼ��·��
	for (int cross_cnt = 0; cross_cnt < cross_txt_num; cross_cnt++)
		Cross[CROSS_ORD_SUB(cross[cross_cnt][0])] = new CROSS(cross[cross_cnt], Road);
	//��ʼ������

	for (int car_cnt = 0; car_cnt < car_txt_num; car_cnt++)
	{//���������ص�ͳ��
		Car[CAR_ORD_SUB(car[car_cnt][0])] = new CAR(car[car_cnt]);
		Cross[CROSS_ORD_SUB(car[car_cnt][1])]->start_car_num++;
	}
	for (int cross_cnt = 0; cross_cnt < cross_txt_num; cross_cnt++)
	{//���ٳ���
		if ((Cross[CROSS_ORD_SUB(cross[cross_cnt][0])]->start_car = (int *)malloc(sizeof(int) * Cross[CROSS_ORD_SUB(cross[cross_cnt][0])]->start_car_num)) == NULL)
			fprintf(stderr, "No Space!!\n");
		for (int i = 0; i < Cross[CROSS_ORD_SUB(cross[cross_cnt][0])]->start_car_num; i++)
			Cross[CROSS_ORD_SUB(cross[cross_cnt][0])]->start_car[i] = NO_CAR; //Ĭ��û�г�
		Cross[CROSS_ORD_SUB(cross[cross_cnt][0])]->start_car_num = 0; //������������
	}
	for (int car_cnt = 0; car_cnt < car_txt_num; car_cnt++)
	{   //������� ��ʼ��������Ϣ
		Cross[CROSS_ORD_SUB(car[car_cnt][1])]->start_car[Cross[CROSS_ORD_SUB(car[car_cnt][1])]->start_car_num] = car[car_cnt][0];
		Car[CAR_ORD_SUB(car[car_cnt][0])]->car_current_road = Car[CAR_ORD_SUB(car[car_cnt][0])]->car_real_route->Road->road_num;
		Car[CAR_ORD_SUB(car[car_cnt][0])]->car_schedule_finished = UNFINISHED;
		Car[CAR_ORD_SUB(car[car_cnt][0])]->car_current_speed = MIN(car[car_cnt][3], Road[ROAD_ORD_SUB(Car[CAR_ORD_SUB(car[car_cnt][0])]->car_current_road)]->road_speed);
		Car[CAR_ORD_SUB(car[car_cnt][0])]->car_next_slot = Road[ROAD_ORD_SUB(Car[CAR_ORD_SUB(car[car_cnt][0])]->car_current_road)]->road_len - Car[CAR_ORD_SUB(car[car_cnt][0])]->car_current_speed;
		Cross[CROSS_ORD_SUB(car[car_cnt][1])]->start_car_num++;
	}

	for (int cross_cnt = 0; cross_cnt < cross_total_num; cross_cnt++)
	{
		StartCarSort(Car, Cross[cross_cnt]); //����ÿ����·�������ȼ�
		Cross[cross_cnt]->start_car_cnt = 0; //��0��ʼ�����ȼ�����  ������0
	}
	
	int FinishedFlag = UNFINISHED;
	while (FinishedFlag == UNFINISHED)
	{
		FinishedFlag = FINISHED;
		int ScheduleTime = 0; //ȫ�ֵ����ܼ���ʱ��ʼ��
		int ScheduleFinishedFlag = UNFINISHED; //ȫ�ֵ��Ƚ���
		int FinishedCar = 0;
		while (ScheduleFinishedFlag == UNFINISHED)
		{
			ScheduleFinishedFlag = FINISHED;
			//STEP1  ������г�����״̬ 
			for (int cross_cnt = 0; cross_cnt < cross_total_num; cross_cnt++) //����ÿ��
			{
				for (int road_cnt = 0; road_cnt < 4; road_cnt++) //ÿ��·�����4����
				{
					if (TravelDirection(Cross[cross_cnt]->road_in_cross[road_cnt], Cross[cross_cnt]) != NO_ROAD)//·�ڿ��Ա���
					{
						int DrectionConfirmFlag = UNFINISHED;
						int travel_direction = TravelDirection(Cross[cross_cnt]->road_in_cross[road_cnt], Cross[cross_cnt]); //�����ķ���
						CAR **CarForward;
						if ((CarForward = (CAR **)malloc(sizeof(CAR *) * Cross[cross_cnt]->road_in_cross[road_cnt]->road_lane_num)) == NULL)//ǰ���洢
							fprintf(stderr, "No Space!!\n");
						for (int lane_cnt = 0; lane_cnt < Cross[cross_cnt]->road_in_cross[road_cnt]->road_lane_num; lane_cnt++)
							CarForward[lane_cnt] = NULL;

						Cross[cross_cnt]->road_in_cross[road_cnt]->frontest_car_direction[travel_direction] = NO_CAR;

						for (int car_slot_cnt = 0; car_slot_cnt < Cross[cross_cnt]->road_in_cross[road_cnt]->road_len; car_slot_cnt++)
						{ //ÿ�����ϵ�ÿ����λ    [len-1][len-2]...[0]  ·��[-1]
							for (int lane_cnt = 0; lane_cnt < Cross[cross_cnt]->road_in_cross[road_cnt]->road_lane_num; lane_cnt++)
							{  //ÿ����·
								if (Cross[cross_cnt]->road_in_cross[road_cnt]->car_slot[travel_direction][lane_cnt][car_slot_cnt] != NULL)//ĳ����ĳ������ĳ��λ���г�
								{
									CAR *temp_car = Cross[cross_cnt]->road_in_cross[road_cnt]->car_slot[travel_direction][lane_cnt][car_slot_cnt];

									ScheduleFinishedFlag = UNFINISHED;

									temp_car->car_schedule_finished = UNFINISHED;  //ˢ�µ��ȱ�־ ���еĳ�����δ����

									//��ǰ��·�� �ó�λ��������ʻ������ٶ�
									temp_car->car_current_speed = MIN(temp_car->car_speed, Cross[cross_cnt]->road_in_cross[road_cnt]->road_speed);

									//��ǰ��λ
									temp_car->car_current_slot = car_slot_cnt;

									//��һʱ�̳��� ��û���赲�������  �ܹ���ʻ����λ��
									temp_car->car_next_slot = car_slot_cnt - temp_car->car_current_speed;

									if (FindCarNextRoad(temp_car->car_current_road, temp_car->car_real_route->Road) == NO_ROAD)//���뵱ǰ��·�õ���һ����·
										temp_car->car_route_finish = FINISHED; //�ִ��յ��־


									if (CarForward[lane_cnt] == NULL)
									{
										if (temp_car->car_next_slot < 0)//�������ٶ��ܹ���·��
										{
											if (temp_car->car_route_finish == FINISHED)
											{
												temp_car->car_status = WAIT_STATUS;//�������Ϊ�ȴ�
												temp_car->car_current_direction = GO_STRAIGHT;
												if (DrectionConfirmFlag == UNFINISHED)
												{
													Cross[cross_cnt]->road_in_cross[road_cnt]->frontest_car_direction[travel_direction] = temp_car->car_current_direction;
													DrectionConfirmFlag = FINISHED;
												}
											}
											else
												if (Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_speed > temp_car->car_current_slot)
												{
													temp_car->car_status = WAIT_STATUS;//�������Ϊ�ȴ�
													temp_car->car_current_direction =
														DIRECTION_MAP[RoadDirectionInCross(temp_car->car_current_road, Cross[cross_cnt])][RoadDirectionInCross(temp_car->car_next_road, Cross[cross_cnt])];
													if (DrectionConfirmFlag == UNFINISHED)
													{
														Cross[cross_cnt]->road_in_cross[road_cnt]->frontest_car_direction[travel_direction] = temp_car->car_current_direction;
														DrectionConfirmFlag = FINISHED;
													}
												}
												else
												{
													temp_car->car_status = END_STATUS;
													temp_car->car_next_slot = 0;
												}
										}
										else   //�ٶȲ�����·�� ǰ��û��
											temp_car->car_status = END_STATUS;
									}
									else //ǰ���г�
									{
										if (temp_car->car_next_slot <= CarForward[lane_cnt]->car_current_slot) //���赲
										{
											if (CarForward[lane_cnt]->car_status == WAIT_STATUS)
												temp_car->car_status = WAIT_STATUS;
											else
											{
												temp_car->car_status = END_STATUS;
												temp_car->car_next_slot = CarForward[lane_cnt]->car_current_slot + 1;
											}
										}
										else
											temp_car->car_status = END_STATUS;
									}
									//---------ǰ��ˢ��һ�������
									CarForward[lane_cnt] = temp_car;//ˢ��ǰ��
								}
							}
						}
						free(CarForward); //�ǵ��ͷ��ڴ�!
					}

				}

			}
			//STEP2  ���ȵȴ�����  
			int Schedule_In_T_Finishied = UNFINISHED;
			while (Schedule_In_T_Finishied == UNFINISHED)
			{
				Schedule_In_T_Finishied = FINISHED;
				for (int cross_cnt = 0; cross_cnt < cross_total_num; cross_cnt++)
				{
					int DirectionConflictFlag = CONFLICT;
					int NextRoadWait = NO_CONFLICT;
					while (DirectionConflictFlag == CONFLICT)
					{
						for (int road_cnt = 0; road_cnt < 4; road_cnt++) //ÿ��·�����4����
						{
							DirectionConflictFlag = NO_CONFLICT;
							if (TravelDirection(Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]], Cross[cross_cnt]) != NO_ROAD)
							{
								int travel_direction = TravelDirection(Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]], Cross[cross_cnt]); //�����ķ���
								CAR **CarForward;
								if ((CarForward = (CAR **)malloc(sizeof(CAR *) * Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->road_lane_num)) == NULL)//ǰ��  ״̬�ж�
									fprintf(stderr, "No Space!!\n");
								for (int lane_cnt = 0; lane_cnt < Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->road_lane_num; lane_cnt++)
									CarForward[lane_cnt] = NULL;

								for (int car_slot_cnt = 0; car_slot_cnt < Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->road_len; car_slot_cnt++)
								{
									for (int lane_cnt = 0; lane_cnt < Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->road_lane_num; lane_cnt++)
									{
										if (Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] != NULL)//ĳ����ĳ������ĳ��λ���г�
										{ //ĳ��λ���г�
											CAR *temp_car = Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt];

											if (temp_car->car_schedule_finished == UNFINISHED)
											{
												Schedule_In_T_Finishied = UNFINISHED;

												if (temp_car->car_status == WAIT_STATUS)
												{
													if (car_slot_cnt < temp_car->car_current_speed)     //S1 < V1 �ܹ�·�� 
													{
														if (CarForward[lane_cnt] == NULL) //ǰ��û�г�  
														{
															if (temp_car->car_route_finish == FINISHED) //�����յ�
															{   //ˢ�µ�·״̬
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = NO_CAR;
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
																//ˢ�³���״̬ 
																cout << temp_car->car_order << endl;
																temp_car->car_schedule_finished = FINISHED;
																temp_car->car_status = END_STATUS;
																cout << FinishedCar++ << "" << endl;
																break; //��һ����
															}
															else
															{   //δ�����յ� V2����  ���Գ�·��
																if (Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_speed > car_slot_cnt)
																{
																	if (DirectionConflict(Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]], Cross[cross_cnt]) == CONFLICT)
																	{
																		DirectionConflictFlag = CONFLICT;
																		break;  //�г�ͻ �ж���һ����·
																	}

																	int IntoDrection = IntoRoadTravelDiraction(Road[ROAD_ORD_SUB(temp_car->car_next_road)], Cross[cross_cnt]);

																	if (IntoDrection != NO_ROAD) //˫�� �������
																	{
																		int temp_speed = MIN(temp_car->car_speed, Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_speed); //����ʻ������ٶ�
																		temp_car->car_next_slot = Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_len - (temp_speed - car_slot_cnt); //����һ��·�ϵ�λ��

																		for (int next_lane_cnt = 0; next_lane_cnt < Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_lane_num; next_lane_cnt++)
																		{
																			for (int next_slot_cnt = Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_len - 1; next_slot_cnt >= temp_car->car_next_slot; next_slot_cnt--)
																			{
																				//Ҫ�Ӻ���ǰ�ѣ��������·�ڳ���
																				if (Road[ROAD_ORD_SUB(temp_car->car_next_road)]->car_slot[IntoDrection][next_lane_cnt][next_slot_cnt] != NULL)//����λ�г� 
																				{
																					if (Road[ROAD_ORD_SUB(temp_car->car_next_road)]->car_slot[IntoDrection][next_lane_cnt][next_slot_cnt]->car_status == END_STATUS)
																					{
																						//�ó���ֹ״̬�ſ��Խ���
																						if (next_slot_cnt < Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_len - 1) //�ó���û����
																						{
																							//ˢ�µ�·״̬    û�г�ͻ �������ˢ��
																							Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = NO_CAR;
																							Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
																							Road[ROAD_ORD_SUB(temp_car->car_next_road)]->car_slot[IntoDrection][next_lane_cnt][next_slot_cnt + 1] = temp_car;

																							//ˢ�³���״̬ 
																							temp_car->car_status = END_STATUS;
																							temp_car->car_current_road = temp_car->car_next_road;
																							temp_car->car_current_speed = MIN(temp_car->car_speed, Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_speed);
																							temp_car->car_current_slot = next_slot_cnt + 1;
																							temp_car->car_next_road = FindCarNextRoad(temp_car->car_current_road, temp_car->car_real_route->Road);  //��һ·�α��
																							temp_car->car_schedule_finished = FINISHED;
																							break;
																						}
																						else
																							break; //�ó����Ѿ���������һ������
																					}
																					else //��һ��������Ҳ���ڵȴ�״̬  
																					{
																						Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = temp_car->car_current_direction;
																						CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
																						NextRoadWait = CONFLICT;
																						LockedTime++; 
																						//cout << "�¸������еȴ�" << endl;

																						break;
																						//�ó�״̬�������ȴ��´ε��� ����ǰ���ȴ�ʹ�øó��ȴ�!!���ܳ������� !!!!
																					}
																				}
																				else
																					if (next_slot_cnt == temp_car->car_next_slot) //������������û����������
																					{
																						//ˢ�µ�·״̬
																						Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = NO_CAR;
																						Road[ROAD_ORD_SUB(temp_car->car_next_road)]->car_slot[IntoDrection][next_lane_cnt][next_slot_cnt] = temp_car;
																						Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;

																						//ˢ�³���״̬ 
																						temp_car->car_status = END_STATUS;
																						temp_car->car_current_road = temp_car->car_next_road;
																						temp_car->car_current_speed = MIN(temp_car->car_speed, Road[ROAD_ORD_SUB(temp_car->car_next_road)]->road_speed);
																						temp_car->car_current_slot = next_slot_cnt;
																						temp_car->car_next_road = FindCarNextRoad(temp_car->car_current_road, temp_car->car_real_route->Road);  //��һ·�α��
																						temp_car->car_schedule_finished = FINISHED;
																						break;
																					}
																			}
																			if (temp_car->car_schedule_finished == FINISHED) break;  //�ó����Ƚ��� ��һ����
																			if (NextRoadWait == CONFLICT)break; //�ȴ���ͻ �������� ֱ�ӵ�����һ��·�� �ٷ��ص��ȸ�·��
																		}
																		if (DirectionConflictFlag == CONFLICT)break; //���г�ͻ����������·ֱ�ӱ�����һ����·
																		if (NextRoadWait == CONFLICT)break;     //�ȴ���ͻ �������� ֱ�ӵ�����һ��·�� �ٷ��ص��ȸ�·��
																		if (temp_car->car_schedule_finished == UNFINISHED) //�������� ���� ��һ��·������ û�г�λ 
																		{
																			//ˢ�µ�·״̬
																			Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = NO_CAR;
																			Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
																			Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][0] = temp_car;

																			//ˢ�³���״̬ 
																			temp_car->car_status = END_STATUS;
																			temp_car->car_schedule_finished = FINISHED;
																			temp_car->car_current_slot = 0;
																			CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
																			continue; //��һ���� ����
																		}
																	}
																	else
																		printf("�������򳵵�����!!");
																}
																else //V2���� ����·�� ǰ��û��
																{
																	//ˢ�µ�·״̬
																	Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = NO_CAR;
																	Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
																	Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][0] = temp_car;
																	//ˢ�³���״̬ 
																	temp_car->car_status = END_STATUS;
																	temp_car->car_schedule_finished = FINISHED;
																	temp_car->car_current_slot = 0;
																	CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��  ��ֹ
																	break;
																}
															}
														}
														else
															if (CarForward[lane_cnt]->car_status == END_STATUS)//��ֹ̬ ����ס����ȥ
															{
																//ˢ�µ�·״̬
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = NO_CAR;
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][CarForward[lane_cnt]->car_current_slot + 1] = temp_car;
																//ˢ�³���״̬ 
																temp_car->car_schedule_finished = FINISHED;
																temp_car->car_status = END_STATUS;
																temp_car->car_next_slot = CarForward[lane_cnt]->car_current_slot + 1;
																CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
																break; //��һ����
															}
															else  //�ȴ�״̬
															{
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->frontest_car_direction[travel_direction] = temp_car->car_current_direction;
																CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
																NextRoadWait = CONFLICT;
																//cout << "ǰ���еȴ�" << endl;
																break;
																//�ó�״̬�������ȴ��´ε��� ����ǰ���ȴ�ʹ�øó��ȴ�!!���ܳ������� !!!!
															}
													}
													else // ���ܳ�·�� �󷽵ȴ�
													{
														temp_car->car_next_slot = temp_car->car_current_slot - temp_car->car_current_speed;
														if ((CarForward[lane_cnt] != NULL) && (temp_car->car_next_slot <= CarForward[lane_cnt]->car_current_slot))
														{
															if (CarForward[lane_cnt]->car_status == END_STATUS)//��ֹ̬ ����ס
															{
																//ˢ�µ�·״̬
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
																Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][CarForward[lane_cnt]->car_current_slot + 1] = temp_car;
																//ˢ�³���״̬ 
																temp_car->car_schedule_finished = FINISHED;
																temp_car->car_status = END_STATUS;
																temp_car->car_next_slot = CarForward[lane_cnt]->car_current_slot + 1;
																CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
																break; //��һ����
															}
															else  //�ȴ�״̬
															{
																CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
																NextRoadWait = CONFLICT;
																//cout << "ǰ���еȴ�" << endl;
																break;
																//�ó�״̬�������ȴ��´ε��� ����ǰ���ȴ�ʹ�øó��ȴ�!!���ܳ������� !!!!
															}
														}
														else //���赲  ��û�г�
														{
															//ˢ�µ�·״̬
															Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
															Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][temp_car->car_next_slot] = temp_car;
															//ˢ�³���״̬ 
															temp_car->car_status = END_STATUS;
															temp_car->car_current_slot = temp_car->car_next_slot;
															temp_car->car_schedule_finished = FINISHED;
															CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
															break; //�ó����Ƚ�����������һ����
														}
													}
												}
												else //END״̬�ĳ��� temp_car->car_next_slot �Ѿ�������� ��ʻ����Ȼ�ڵ�ǰ����
												{
													//ˢ�µ�·״̬
													Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][car_slot_cnt] = NULL;
													Cross[cross_cnt]->road_in_cross[Cross[cross_cnt]->travel_order[road_cnt]]->car_slot[travel_direction][lane_cnt][temp_car->car_next_slot] = temp_car;
													//ˢ�³���״̬ 
													temp_car->car_status = END_STATUS;
													temp_car->car_current_slot = temp_car->car_next_slot;
													temp_car->car_schedule_finished = FINISHED;
													CarForward[lane_cnt] = temp_car;  //ˢ��ǰ��
													break; //�ó����Ƚ�����������һ����)
												}
											}
											if (DirectionConflictFlag == CONFLICT)break; //���г�ͻ����������·ֱ�ӱ�����һ����·
											if (NextRoadWait == CONFLICT)break; //��ǰ��·���� �ȴ��´ε���
										}
									}
									if (DirectionConflictFlag == CONFLICT)break; //���г�ͻ����������·ֱ�ӱ�����һ����·
									if (NextRoadWait == CONFLICT)break; //��ǰ��·���� �ȴ��´ε���
								}
								free(CarForward); //�ǵ��ͷ��ڴ棡������
								CarForward = NULL;
								if (NextRoadWait == CONFLICT)break; //��ǰ��·���� �ȴ��´ε���
							}
						}
					}
				}
			}
			//STEP3 �³���·
			for (int cross_cnt = 0; cross_cnt < cross_total_num; cross_cnt++)
			{
				while (Cross[cross_cnt]->start_car_cnt < Cross[cross_cnt]->start_car_num)//�ó�������û�з������
				{
					ScheduleFinishedFlag = UNFINISHED;
					if (Car[CAR_ORD_SUB(Cross[cross_cnt]->start_car[Cross[cross_cnt]->start_car_cnt])]->car_start_time <= ScheduleTime)//���㷢��
					{
						CAR *Start_Car_temp = Car[CAR_ORD_SUB(Cross[cross_cnt]->start_car[Cross[cross_cnt]->start_car_cnt])];

						int IntoDrection = IntoRoadTravelDiraction(Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)], Cross[cross_cnt]);

						if (IntoDrection != NO_ROAD)
						{
							for (int next_lane_cnt = 0; next_lane_cnt < Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)]->road_lane_num; next_lane_cnt++)
							{
								for (int next_slot_cnt = Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)]->road_len - 1; next_slot_cnt >= Start_Car_temp->car_next_slot; next_slot_cnt--)
								{
									//Ҫ�Ӻ���ǰ�ѣ��������·�ڳ���
									if (Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)]->car_slot[IntoDrection][next_lane_cnt][next_slot_cnt] != NULL)//����λ�г� 
									{
										if (next_slot_cnt < Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)]->road_len - 1) //�ó���û����
										{
											//ˢ�µ�·״̬
											Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)]->car_slot[IntoDrection][next_lane_cnt][next_slot_cnt + 1] = Start_Car_temp;
											//ˢ�³���״̬ 
											Cross[cross_cnt]->start_car_cnt++; //�ɹ���������
											Start_Car_temp->car_status = END_STATUS;
											Start_Car_temp->car_current_slot = next_slot_cnt + 1;
											Start_Car_temp->car_next_road = FindCarNextRoad(Start_Car_temp->car_current_road, Start_Car_temp->car_real_route->Road);  //��һ·�α��
											Start_Car_temp->car_schedule_finished = FINISHED;
											break;
										}
										else //�ó����� ��һ����
											break;
									}
									else
										if (next_slot_cnt == Start_Car_temp->car_next_slot) //������������û����������
										{
											Road[ROAD_ORD_SUB(Start_Car_temp->car_current_road)]->car_slot[IntoDrection][next_lane_cnt][Start_Car_temp->car_next_slot] = Start_Car_temp;
											//ˢ�³���״̬ 
											Cross[cross_cnt]->start_car_cnt++; //�ɹ���������
											Start_Car_temp->car_status = END_STATUS;
											Start_Car_temp->car_current_slot = next_slot_cnt;
											Start_Car_temp->car_next_road = FindCarNextRoad(Start_Car_temp->car_current_road, Start_Car_temp->car_real_route->Road);  //��һ·�α��
											Start_Car_temp->car_schedule_finished = FINISHED;
											break;
										}
								}
								if (Start_Car_temp->car_schedule_finished == FINISHED) break;//�ó����Ƚ��� ��һ����
							}
						}
						else
							printf("�������򲻶�!");
						//ȫ���������û�г�λ ���ܷ��� 
						if (Start_Car_temp->car_schedule_finished == UNFINISHED)break; //��������ѭ��
					}
					else
						break;//û����
				}
			}
			ScheduleTime++;
			//cout<< ScheduleTime <<endl;
			/*for (int i = 0; i < car_txt_num; i++)
			if (Car[i]->car_current_road==5051)
			{
				cout << Car[i]->car_order << ":" << Car[i] ->car_current_road << ":" <<  " --- " << Car[i]->car_current_slot << endl;
			}*/
			//cout << endl;
			//cout << Car[83]->car_order << ":" << Car[83]->car_current_road << ":" << " --- " << Car[83]->car_current_slot << endl;
			//cout << Car[87]->car_order << ":" << Car[87]->car_current_road << ":" << " --- " << Car[87]->car_current_slot << endl;
		}
		for (int i = 0; i < car_total_num; i++)
			if (Car[i] != NULL && Car[i]->car_route_finish != FINISHED)
			{
				cout << Car[i]->car_order << endl;
				ShowRouteRoad(Car[i]->car_real_route->Road);
				cout << " --- " << Car[i]->car_next_road << endl;
			}
		cout << endl;
		//ShowRouteRoad(Car[10]->car_real_route->Road);
		//ShowRouteRoad(Car[87]->car_real_route->Road);
	}
}
int RoadDirectionInCross(int road_ord,CROSS *cross) //��·��·���еķ�λ 0�� 1�� 2�� 3��
{
	for (int cnt = 0; cnt < 4; cnt++)
		if (cross->road_in_cross[cnt] != NULL) //��·����
			if (cross->road_in_cross[cnt]->road_ord == road_ord)
				return cnt;  
	return NO_ROAD;
}
int TravelDirection(ROAD *road, CROSS *cross)
{
	if (road != NULL)
	{
		if (road->road_duplex == DUPLEX)//˫��
		{
			if (road->road_end_cross == cross->cross_ord) //��·��Ϊ�յ�
				return FORWARD;   //����ǰ���·  ������ʻ��·�� [0]
			else                              //��·��Ϊ���
				return BACKWARD;   //���������· ������ʻ��·�� [1]
		}
		else
		{
			if (road->road_end_cross == cross->cross_ord) //��·��Ϊ�յ�
				return  SINGLE;   //���������·
			else
				return NO_ROAD;   //����Ϊ����򲻱���
		}
	}
	else
		return NO_ROAD;
}
int DirectionConflict(ROAD *road, CROSS *cross) //��road����cross�ĳ��� ת���ͻ�ж�
{
	switch (road->frontest_car_direction[TravelDirection(road, cross)])
	{
	    case TURN_LEFT:
		{
			switch (RoadDirectionInCross(road->road_ord, cross))
			{  //��תֻ��ֱ�г�ͻ
				case UP_ROAD: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[3], cross);
					if ((JudgeDirection!=NO_ROAD) && (cross->road_in_cross[3]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					break;
				}
				case 1: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[0], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[0]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					break;
				}
				case 2: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[1], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[1]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					break;
				}
				case 3: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[2], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[2]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					break;
				}
			}
			break;
		}
		case TURN_RIGHT:
		{
			switch (RoadDirectionInCross(road->road_ord, cross))
			{ //��ת��ֱ����ת����ͻ
				case 0: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[1], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[1]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					    JudgeDirection = TravelDirection(cross->road_in_cross[2], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[2]->frontest_car_direction[JudgeDirection] == TURN_LEFT))
						return CONFLICT;
					break;
				}
				case 1: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[2], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[2]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					JudgeDirection = TravelDirection(cross->road_in_cross[3], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[3]->frontest_car_direction[JudgeDirection] == TURN_LEFT))
						return CONFLICT;
					break;
				}
				case 2: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[3], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[3]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					JudgeDirection = TravelDirection(cross->road_in_cross[0], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[0]->frontest_car_direction[JudgeDirection] == TURN_LEFT))
						return CONFLICT;
					break;
				}
				case 3: 
				{
					int JudgeDirection = TravelDirection(cross->road_in_cross[0], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[0]->frontest_car_direction[JudgeDirection] == GO_STRAIGHT))
						return CONFLICT;
					JudgeDirection = TravelDirection(cross->road_in_cross[1], cross);
					if ((JudgeDirection != NO_ROAD) && (cross->road_in_cross[1]->frontest_car_direction[JudgeDirection] == TURN_LEFT))
						return CONFLICT;
					break;
				}
			}
			break;
		}
	}
	return NO_CONFLICT;
}
int FindCarNextRoad(int road, RouteRoad *r) //���뵱ǰ��·�õ���һ����·
{
	while (r != NULL)
	{
		if (r->road_num == road && r->next != NULL) //�ҵ���ǰ·  �µ�·����
			return r->next->road_num; //����Ѱ��
		r = r->next;
	}
	return NO_ROAD; //δ�ҵ�·
}
void StartCarSort(CAR **car, CROSS *cross) //����ĳ������ĸ������������ȼ�����
{
	int temp_ord = 0;
	if (cross->start_car_num <= 1)return;  //����һ���� �����Ŷ�
	for (int i = 0; i < cross->start_car_num - 1; i++)
	{
		for (int j = 0; j < cross->start_car_num - 1 - i; j++)
		{
			//����ð��
			if (car[CAR_ORD_SUB(cross->start_car[j])]->car_start_time > car[CAR_ORD_SUB(cross->start_car[j + 1])]->car_start_time)//�ȿ�ʱ��
			{
				temp_ord = cross->start_car[j];
				cross->start_car[j] = cross->start_car[j + 1];
				cross->start_car[j + 1] = temp_ord;
			}
			else
			{
				if ((car[CAR_ORD_SUB(cross->start_car[j])]->car_start_time == car[CAR_ORD_SUB(cross->start_car[j + 1])]->car_start_time)
					&& (car[CAR_ORD_SUB(cross->start_car[j])]->car_order > car[CAR_ORD_SUB(cross->start_car[j + 1])]->car_order))//ʱ����ͬ ����Ų�ͬ
				{
					temp_ord = cross->start_car[j];
					cross->start_car[j] = cross->start_car[j + 1];
					cross->start_car[j + 1] = temp_ord;
				}
			}
		}
	}
}
int IntoRoadTravelDiraction(ROAD *road, CROSS *cross)
{
	if (road != NULL)
	{
		if (road->road_duplex == DUPLEX)//˫��
		{
			if (road->road_end_cross == cross->cross_ord) //��·��Ϊ�յ�
				return BACKWARD;   //����ǰ���·  ������ʻ��·�� [0]
			else                              //��·��Ϊ���
				return FORWARD;   //���������· ������ʻ��·�� [1]
		}
		else
		{
			if (road->road_end_cross == cross->cross_ord) //��·��Ϊ�յ�
				return  NO_ROAD;   //����Ϊ�յ��򲻱���
			else
				return SINGLE;   //���������·
		}
	}
	else
		return NO_ROAD;
}
