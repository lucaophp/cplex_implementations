#include "RoteamentoDeVeiculos.h"

RoteamentoDeVeiculos readData(string filename)
{
	RoteamentoDeVeiculos r;
	ifstream io(filename);
	io >> r.nClientes;
	io >> r.Q;
	r.nClientes++;

	for (int i = 0; i < r.nClientes; i++)
	{
		Point p;
		io >> p.x >> p.y;
		r.clientes.push_back(p);
	}
	for (int i = 0; i < r.nClientes; i++)
	{
		r.q.push_back(0);
		io >> r.q[i];
	}
	for (int i = 0; i < r.nClientes; i++)
	{
		vector<float> aux;
		for (int j = 0; j < r.nClientes; j++)
		{
			aux.push_back(0);
			io >> aux[j];

		}
		r.c.push_back(aux);
	}
	return r;


}

void fo(IloEnv env, IloModel &model, RoteamentoDeVeiculos &r)
{
	IloExpr obj(env);
	IloArray<IloNumVarArray> x(env, r.nClientes);
	for (int i = 0; i < r.nClientes; i++)
	{
		x[i] = IloNumVarArray(env, r.nClientes, 0, 1, ILOINT);
	}
	IloArray<IloNumVarArray> f(env, r.nClientes);
	for (int i = 0; i < r.nClientes; i++)
	{
		f[i] = IloNumVarArray(env, r.nClientes, 0, IloInfinity, ILOINT);
	}
	r.f = f;
	r.x = x;
	for (int i = 0; i < r.nClientes; i++)
	{
		for (int j = 0; j < r.nClientes; j++)
		{
			obj += r.c[i][j] * r.x[i][j];
		}
	}
	model.add(IloMinimize(env, obj));
}

void restricoes(IloEnv env, IloModel &model, RoteamentoDeVeiculos &r)
{
	for (int k = 1; k < r.nClientes; k++)
	{
		IloExpr restricao(env);
		for (int i = 0; i < r.nClientes; i++)
		{
			restricao += r.x[i][k];

		}
		model.add(restricao == 1);

	}
	for (int k = 1; k < r.nClientes; k++)
	{
		IloExpr restricao(env);
		for (int j = 0; j < r.nClientes; j++)
		{
			restricao += r.x[k][j];

		}
		model.add(restricao == 1);

	}
	IloExpr re(env);
	IloExpr rd(env);
	for (int j = 1; j < r.nClientes; j++)
	{
		re += r.x[0][j];
		rd += r.x[j][0];
	}
	model.add(re == rd);
	for (int k = 1; k < r.nClientes; k++)
	{
		IloExpr re(env);
		IloExpr rd(env);
		for (int i = 0; i < r.nClientes; i++)
		{
			re += r.f[i][k];
			rd += r.f[k][i];
		}
		model.add(re - rd == r.q[k]);

	}

	for (int i = 0; i < r.nClientes; i++)
	{

		for (int j = 0; j < r.nClientes; j++)
		{
			model.add(r.f[i][j] <= r.Q*r.x[i][j]);
		}


	}


}
void printValues(IloCplex cplex, RoteamentoDeVeiculos &r, long double tempo)
{
	ofstream io("C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP" + std::to_string(r.nClientes) + "_result.txt");
	io << tempo << endl;
	io << "Função Objetivo: " << cplex.getObjValue() << endl;
	io << "GAP: " << cplex.getMIPRelativeGap() << endl;
	for (int i = 0; i < r.nClientes; i++)
	{
		Point p = r.clientes[i];
		io << p.x << "\t" << p.y << endl;
	}
	io << endl;
	for (int i = 0; i < r.nClientes; i++) {
		for (int j = 0; j < r.nClientes; j++)
		{
			if (cplex.getValue(r.x[i][j]) == 1)
			{
				io << i << "\t" << j << "\t" << cplex.getValue(r.f[i][j]) << endl;
			}
		}
	}



}
int runRoteamento()
{
	string files[] = {
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP25NORM.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP30NORM.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP40NORM.txt",

		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP50NORM.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP60NORM.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/VRP70NORM.txt"

	};
	for (int f = 2; f < 3; f++) {
		RoteamentoDeVeiculos m = readData(files[f]);
		IloEnv env;
		try {
			IloModel model(env);
			fo(env, model, m);
			restricoes(env, model, m);

			IloCplex cplex(env);
			cplex.setParam(IloCplex::ClockType, 2);
			cplex.setParam(IloCplex::TiLim, 3600);
			cplex.setParam(IloCplex::Threads, 4);
			cplex.setOut(env.getNullStream());

			cplex.extract(model);
			long double time = (clock() / (double)CLOCKS_PER_SEC);
			cplex.solve();
			time = (double)(clock() / (double)CLOCKS_PER_SEC) - time;
			cout << "GAP: " << cplex.getMIPRelativeGap() << endl;
			cout << cplex.getObjValue() << endl;
			cout << time << endl;
			printValues(cplex, m, time);
			model.end();
			env.end();
		}
		catch (IloException& e) {
			cerr << e.getMessage() << endl;
			e.end();
		}
		catch (...) {


		}

	}

	system("pause");
	return 0;
}
