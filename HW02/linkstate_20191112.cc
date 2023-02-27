#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>

#define MAX_NODE 100
#define MAX_COST 100
#define MAX_MESSAGE 1000
#define INF 1000000000

using namespace std;

int graph[MAX_NODE][MAX_NODE];
//graph[i][j] => ���i�κ��� ���j������ cost

typedef struct Table {
	int cost;
	int next;
	int neighbor;
	int parent;
}Table;
Table table[MAX_NODE][MAX_NODE];
int node_num;
int visited[MAX_NODE] = { 0, };

void table_initialize();
void table_update();
void table_print();
void message_print();

ifstream fin_topology, fin_messages, fin_changes;
ofstream fout_output;

void table_initialize() {
	int i, j;
	string buffer = ""; //������ �о���̴� ����
	string token = "";
	fin_topology.clear();
	fin_topology.seekg(0, ios::beg);
	getline(fin_topology, buffer);

	node_num = atoi(buffer.c_str());

	int node_from = 0; //��� ���
	int node_to = 0; //���� ���
	int cost = 0; //�Ÿ�
	//graph, table �ʱ�ȭ

	for (i = 0; i < MAX_NODE; i++) {
		for (j = 0; j < MAX_NODE; j++) {
			if (i == j) {
				graph[i][j] = 0;
				table[i][j].cost = 0;
				table[i][j].next = i;
				table[i][j].neighbor = 0;
				table[i][j].parent = -1;
			}
			else {
				graph[i][j] = INF;
				table[i][j].cost = INF;
				table[i][j].next = -1;
				table[i][j].neighbor = 0;
				table[i][j].parent = -1;
			}
		}
	}

	while (getline(fin_topology, buffer)) {
		stringstream ss(buffer);
		for (j = 0; j < 3; j++) {
			getline(ss, token, ' '); //���� �������� ������
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
		table[node_to][node_from].cost = cost;
		table[node_from][node_to].next = node_to;
		table[node_to][node_from].next = node_from;
		table[node_from][node_to].neighbor = 1;
		table[node_to][node_from].neighbor = 1;
		table[node_from][node_to].parent = node_from;
		table[node_to][node_from].parent = node_to;
	}

}

int find_nearest_node(int node) {
	int i;
	int min_cost = INF;
	int nearest_node = 0;

	for (i = 0; i < node_num; i++) {
		//���� �湮���� ���� ��� �� ���� ����� ��带 ã�´�.
		if (visited[i] == 0 && table[node][i].cost < min_cost) { 
			//tie breaking 2 : id�� ���� ������ Ž���ϹǷ� �ڿ� �Ÿ��� ���� ��尡 ���͵� id�� ���� ��尡 ���õȴ�.
			min_cost = graph[node][i];
			nearest_node = i;
		}
	}
	return nearest_node;
}

void table_update() {
	int i, j, k;
	int now; //���� ó���ϴ� ���
	int visit_count = 0; //�湮�� ����� ��
	int tie_node = 0; //tie�� �߻����� �� ���� ���

	for (i = 0; i < node_num; i++) {
		visit_count = 1;
		for (j = 0; j < node_num; j++) {
			visited[j] = 0;
		}
		now = i;
		visited[now] = 1;

		while (true) {
			if (visit_count == node_num) break; //��带 ��� �湮�ϸ� ������.
			now = find_nearest_node(i);
			visited[now] = 1;
			visit_count++;
			for (k = 0; k < node_num; k++) {
				if (table[i][now].cost + table[now][k].cost <= table[i][k].cost && now != k) { //�� ª�� ��� �߰�
					//tie breaking 3
					if ((table[i][k].parent != -1) && (table[i][now].cost + table[now][k].cost == table[i][k].cost)) {
						//ó�� �߰��ϴ� ���� �ƴϸ�, ���� cost�� ���� ã�� cost�� ���ٸ�
						//parent�� ���ڰ� ���� ����� ��θ� ���Ѵ�
						tie_node = min(table[i][k].parent, now);
						if (tie_node == now) {
							table[i][k].next = table[i][now].next;
							table[i][k].parent = now;
						}
					}
					else {
						table[i][k].next = table[i][now].next;
						table[i][k].parent = now;
					}
					table[i][k].cost = table[i][now].cost + table[now][k].cost; //�Ÿ� ����
				}
			}
		}
	}
	table_print();
}


void table_print() {
	//����� ���̺� ���
	int i, j;
	for (i = 0; i < node_num; i++) {
		for (j = 0; j < node_num; j++) {
			if (table[i][j].cost < INF){
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

	//�޽��� �б�
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
		//�޽��� ���� ���
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
		cerr << "usage: linkstate topologyfile messagesfile changesfile" << endl;
		exit(1);
	}
	/*
	argument 1 : topologyfile
	argument 2 : messagesfile
	argument 3 : changesfile
	*/

	//topologyfile �б�
	fin_topology.open(argv[1]);
	if (fin_topology.fail()) {
		cerr << "Error: open input file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}
	//messagesfile �б�
	fin_messages.open(argv[2]);
	if (fin_messages.fail()) {
		cerr << "Error: open input file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}
	//changesfile �б�
	fin_changes.open(argv[3]);
	if (fin_changes.fail()) {
		cerr << "Error: open input file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}
	//outputfile ����
	fout_output.open("output_ls.txt");
	if (fout_output.fail()) {
		cerr << "Error: open output file." << endl;
		fout_output << "Error: open input file." << endl;
		exit(1);
	}

	table_initialize();
	table_update(); //���� ���̺� ������Ʈ
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
			table[change_from][change_to].next = change_to;
			table[change_to][change_from].next = change_from;
			table[change_from][change_to].neighbor = 1;
			table[change_to][change_from].neighbor = 1;
		}
		fout_output << "\n";
		table_update();
		message_print();
	}

	cout << "Complete. Output file written to output_ls.txt.";

	fin_topology.close();
	fin_messages.close();
	fin_changes.close();

	return 0;
}
