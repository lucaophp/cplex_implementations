#pragma once

#include <ilcplex\ilocplex.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "Point.h"
using namespace std;

struct Milk {
	//numero de fazendas a serem visitadas
	int nClientes;
	//numero de veiculos para o transporte
	int nVeiculos;
	//custo para cada veiculo v ir de um ponto i a j
	vector<vector<vector<float>>> c;
	//oferta de leite do da fazenda i
	vector<float> q;
	//capacidade do caminhão v em metros cubicos
	vector<float> Q;
	//custo fixo ao usar o veiculo v
	vector<float> cf;
	//data de disponibilização do leite na fazenda i
	vector<float> R;
	//TEMPO DE PROCESSAMENTO DO LEITE NA FAZENDA I
	vector<float> p;
	float VM;
	//distancia entre o ponto i a j;
	vector<vector<float>> d;
	//tempo maximo por rota
	float Dl;
	//variavel de decisaão que define qual a sequencia de viagem de cada veiculo.
	IloArray<IloArray<IloNumVarArray>> x;
	//variavel de decisao que define se o veiculo v é utilizado.
	IloNumVarArray y;
	//variavel de decisão que define o tempo que o veiculo v inicia o processo de coleta no produtor i
	IloArray<IloNumVarArray> s;
	//variavel de decisão que define o fluxo entre os clientes
	IloArray<IloNumVarArray> f;
	Point deposito;
	vector<Point> produtores;
};

int runMilk();
