#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>

#define MAX_NODE 100
#define MAX_COST 100
#define MAX_MESSAGE 1000
#define INF 1000000000

using namespace std;

int graph[MAX_NODE][MAX_NODE];
//graph[i][j] => 노드i로부터 노드j까지의 cost

typedef struct Table {
	int cost;
	int next;
	int neighbor;
}Table;
Table table[MAX_NODE][MAX_NODE];
int node_num;

void table_initialize();
void table_update();
void table_print();
void message_print();

ifstream fin_topology, fin_messages, fin_changes;
ofstream fout_output;

void table_initialize() {
	int i, j;
	string buffer = ""; //파일을 읽어들이는 버퍼
	string token = "";
	fin_topology.clear();
	fin_topology.seekg(0, ios::beg);
	getline(fin_topology, buffer);
	node_num = atoi(buffer.c_str());

	int node_from = 0; //출발 노드
	int node_to = 0; //도착 노드
	int cost = 0; //거리
	//graph, table 초기화
	
	for (i = 0; i < MAX_NODE; i++) {
		for (j = 0; j < MAX_NODE; j++) {
			if (i == j) {
				graph[i][j] = 0;
				table[i][j].cost = 0;
				table[i][j].next = i;
				table[i][j].neighbor = 0;
			}
			else {
				graph[i][j] = INF;
				table[i][j].cost = INF;
				table[i][j].next = -1;
				table[i][j].neighbor = 0;
			}
		}
	}
	
	while (getline(fin_topology, buffer)) {
		stringstream ss(buffer);
		for (j = 0; j < 3; j++) {
			getline(ss, token, ' '); //공백 기준으로 나누기
			switch (j) {
			case 0:
				node_from = atoi(token.c_str());
				break;
			case 1:
				node_to = atoi(token.c_str());
				break;
			case 2:
				cost = atoi(token.c_str());
			}
		}
		graph[node_from][node_to] = cost;
		graph[node_to][node_from] = cost;
		table[node_from][node_to].cost = cost;
		table[node_from][node_to].next = node_to;
		table[node_from][node_to].neighbor = 1;
		table[node_to][node_from].neighbor = 1;
		table[node_to][node_from].cost = cost;
		table[node_to][node_from].next = node_from;
	}
	
}

void table_update() {
	int i, j, k;
	int change_flag = 0;
	int original_cost = 0;

	while (true) {
		change_flag = 0;
		for (i = 0; i < node_num; i++) {
			for (j = 0; j < node_num; j++) {
				if (table[i][j].neighbor == 1) { //i와 j는 이웃노드
					for (k = 0; k < node_num; k++) {
						if (graph[i][j] + table[j][k].cost <= table[i][k].cost) { //더 짧은 경로 발견
							change_flag++;
							original_cost = table[i][k].cost;
							table[i][k].cost = graph[i][j] + table[j][k].cost;
							//tie breaking 1
							if ((table[i][k].next != -1) && (graph[i][j] + table[j][k].cost == original_cost)) {
								//처음 발견하는 것이 아니며, 기존 cost와 새로 찾은 cost가 같다면
								change_flag--;
								table[i][k].next = min(j, table[i][k].next); //더 숫자가 작은 노드의 경로를 택한다
							}
							else {
								table[i][k].next = j;
							}
						}
					}
				}

			}
		}
		if (change_flag == 0)
			break;
	}

	table_print();
}


void table_print() {
	//라우팅 테이블 출력
	int i, j;
	for (i = 0; i < node_num; i++) {
		for (j = 0; j < node_num; j++) {
			if (table[i][j].cost < INF) {
				fout_output << j << " " << table[i][j].next << " " << table[i][j].cost << endl;
				//cout << j << " " << table[i][j].next << " " << table[i][j].cost << endl;
			}	
		}	
		fout_output << endl;
		//cout << endl;
	}
}

void message_print() {
	int i = 0;
	string buffer = "";
	int node_from = 0;
	int node_to = 0;
	string message = "";
	int space_check = 0;
	string path = "";
	int current_from = 0;
	int cost = 0;

	//메시지 읽기
	fin_messages.clear();
	fin_messages.seekg(0, ios::beg);
	while (getline(fin_messages, buffer)) {
		i = 0;
		stringstream ss(buffer);
		ss >> node_from >> node_to;
		space_check = 0;

		while (true) {
			if (buffer[i] == '\0') break;
			if (buffer[i] == ' ') space_check++;
			if (space_check == 2) {
				message = buffer.substr(i + 1);
				space_check++;
			}
			i++;
		}
		//메시지 내용 출력
		if (table[node_from][node_to].cost >= INF) {
			fout_output << "from " << node_from << " to " << node_to << " cost infinite hops unreachable message " << message << endl;
		}
		else {
			char tmp[MAX_NODE];
			cost = table[node_from][node_to].cost;
			sprintf(tmp, "%d", node_from);
			path = tmp;
			current_from = node_from;
			while (table[current_from][node_to].next != node_to) {
				for (i = 0; i < MAX_NODE; i++) tmp[i] = 0;
				current_from = table[current_from][node_to].next;
				path += " ";
				sprintf(tmp, "%d", current_from);
				path += tmp;
			}
			fout_output << "from " << node_from << " to " << node_to << " cost " << cost << " hops " << path << " message " << message << endl;
			//cout << "from " << node_from << " to " << node_to << " cost " << cost << " hops " << path << " message " << message << endl;
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		cerr << "usage: distvec topologyfile messagesfile changesfile" << endl;
		exit(1);
	}
	/*
	argument 1 : topologyfile
	argument 2 : messagesfile
	argument 3 : changesfile
	*/

	//topologyfile 읽기
	fin_topology.open(argv[1]);
	if (fin_topology.fail()) {
		cerr << "Error: open input file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	} 
	//messagesfile 읽기
	fin_messages.open(argv[2]);
	if (fin_messages.fail()) {
		cerr << "Error: open input file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}
	//changesfile 읽기
	fin_changes.open(argv[3]);
	if (fin_changes.fail()) {
		cerr << "Error: open input file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}
	//outputfile 쓰기
	fout_output.open("output_dv.txt");
	if (fout_output.fail()) {
		cerr << "Error: open output file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}

	table_initialize();
	table_update(); //최초 테이블 업데이트
	message_print();
	
	
	string buffer = "";
	int change_from = 0;
	int change_to = 0;
	int change_cost = 0;

	//change
	while (getline(fin_changes, buffer)) {
		table_initialize();
		stringstream ss(buffer);
		ss >> change_from >> change_to >> change_cost;
		if (change_cost == -999) {
			graph[change_from][change_to] = INF;
			graph[change_to][change_from] = INF;
			table[change_from][change_to].cost = INF;
			table[change_to][change_from].cost = INF;
			table[change_from][change_to].neighbor = 0;
			table[change_to][change_from].neighbor = 0;
		}
		else {
			graph[change_from][change_to] = change_cost;
			graph[change_to][change_from] = change_cost;
			table[change_from][change_to].cost = change_cost;
			table[change_to][change_from].cost = change_cost;
			table[change_from][change_to].neighbor = 1;
			table[change_to][change_from].neighbor = 1;
			table[change_from][change_to].next = change_to;
			table[change_to][change_from].next = change_from;
		}
		fout_output << "\n";
		table_update();
		message_print();
	}
	
	cout << "Complete. Output file written to output_dv.txt.";
	
	fin_topology.close();
	fin_messages.close();
	fin_changes.close();

	return 0;
}
