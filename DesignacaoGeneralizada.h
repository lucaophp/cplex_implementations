#pragma once
#include <ilcplex\ilocplex.h>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
struct Designacao {
	int nMaquinas;
	int nTarefas;
	vector<vector<float>> a;
	vector<vector<float>> c;
	vector<float> b;
	IloArray<IloNumVarArray> x;

};

int runDesignacao();

