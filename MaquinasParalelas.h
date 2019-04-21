#pragma once
#include <ilcplex\ilocplex.h>
#include <iostream>
#include <fstream>
#include <vector>
#define BIG_M long int(1000000)

using namespace std;



struct MaquinasParalelas {
	int nMaquinas;
	int nTarefas;
	vector<float> dataEntrega;
	vector<vector<float>> tempoProcessamento;
	vector<vector<vector<float>>> setup;
	IloArray<IloArray<IloNumVarArray>> x;
	IloNumVarArray C;
	IloNumVarArray T;
	IloNumVar CMax;
};
int runMaquinas();