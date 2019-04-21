#pragma once

#include <ilcplex\ilocplex.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "Point.h"
using namespace std;

struct RoteamentoDeVeiculos {
	int nClientes;
	vector<vector<float>> c;
	vector<float> q;
	float Q;
	IloArray<IloNumVarArray> x;
	IloArray<IloNumVarArray> f;
	Point deposito;
	vector<Point> clientes;
};

int runRoteamento();
