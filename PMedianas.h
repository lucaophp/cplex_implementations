#pragma once
#include <ilcplex\ilocplex.h>
#include <vector>
#include <time.h>
using namespace std;
struct Cliente {
	int x;
	int y;
	float demanda;
};
struct Place {
	int x;
	int y;
	float capacidade;
};
struct PMedianas {
	int nMedianas;
	int nClientes;
	int nPlaces;
	float capacidade;
	vector<Cliente> clientes;
	vector<Place> places;
	vector<vector<float>> D;
	IloArray<IloNumVarArray> x;
	IloNumVarArray y;
};

int runPMedianas();
