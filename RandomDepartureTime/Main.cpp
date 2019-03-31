#include <iostream>
#include <string>
#include "time.h"  //ע��ע�͵�
#include "Mapping.h"
#include "Read.h"
#include "Output.h"
using namespace std;

int main()
{
	int **car;           //��������
	int **road;          //��·����
	int **cross;         //·������
	long int start_time = clock();
	car = GetCar("D:/competition/Maps/Map1/car.txt");
	road = GetRoad("D:/competition/Maps/Map1/road.txt");
	cross = GetCross("D:/competition/Maps/Map1/cross.txt");
	string answerPath("D:/competition/Maps/Map1/answer.txt"); //ע�����

	map_building(cross, road); //�����ڽӾ���
	MAP_Floyd();  //��С���۾���

	PrintTXT(MAP, car, answerPath); //���answer.txt
	cout << (double)(clock() - start_time) / CLOCKS_PER_SEC << endl;
	system("pause");
	return 0;
}


