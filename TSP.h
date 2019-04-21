#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <ilcplex\ilocplex.h>
#include "Point.h"
using namespace std;

struct TSP {
	vector<vector<float>> d;
	vector<Point> points;
	int n;
	IloArray<IloNumVarArray> x, f;
	IloNumVarArray u;
	
};
int runTSP();