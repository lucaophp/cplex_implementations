#include "DesignacaoGeneralizada.h"

Designacao readData(string filename)
{
	Designacao d;
	string descarte;
	ifstream io(filename);
	getline(io, descarte);
	io >> d.nMaquinas;
	io >> d.nTarefas;
	//io >> descarte;
	getline(io, descarte);
	getline(io, descarte);
	for (int i = 0; i < d.nMaquinas; i++)
	{
		vector<float> aux;
		for (int j = 0; j < d.nTarefas; j++)
		{
			aux.push_back(0);
			io >> aux[j];
			cout << aux[j] << "\t";
		}
		cout << endl;
		d.c.push_back(aux);
	}
	getline(io, descarte);
	getline(io, descarte);
	for (int i = 0; i < d.nMaquinas; i++)
	{
		vector<float> aux;
		for (int j = 0; j < d.nTarefas; j++)
		{
			aux.push_back(0);
			io >> aux[j];
		}
		d.a.push_back(aux);
	}
	getline(io, descarte);
	getline(io, descarte);
	for (int i = 0; i < d.nMaquinas; i++)
	{
		d.b.push_back(0);
		io >> d.b[i];

	}

	return d;
}

void fo(IloEnv env, IloModel &model, Designacao &d)
{
	IloExpr obj(env);
	IloArray<IloNumVarArray> x(env, d.nMaquinas);
	for (int i = 0; i < d.nMaquinas; i++)
	{
		x[i] = IloNumVarArray(env, d.nTarefas, 0, 1, ILOINT);

	}
	d.x = x;
	for (int i = 0; i < d.nMaquinas; i++)
	{
		for (int j = 0; j < d.nTarefas; j++)
		{
			obj += d.c[i][j] * d.x[i][j];
		}
	}
	model.add(IloMinimize(env, obj));

}
void restricoes(IloEnv env, IloModel &model, Designacao &d)
{
	//a máquina não pode aceitar trabalhar mais do que sua capacidade.
	for (int i = 0; i < d.nMaquinas; i++)
	{
		IloExpr r(env);
		for (int j = 0; j < d.nTarefas; j++)
		{
			r += d.a[i][j] * d.x[i][j];
		}
		model.add(r <= d.b[i]);
	}
	//uma tarefa só pode ser realizada por uma unica máquina.
	for (int j = 0; j < d.nTarefas; j++)
	{
		IloExpr r(env);
		for (int i = 0; i < d.nMaquinas; i++)
		{
			r += d.x[i][j];
		}
		model.add(r == 1);
	}
}
void printValues(IloCplex cplex, Designacao &r, long double tempo, string filename)
{
	ofstream io(filename + "_result_2.txt");
	io << tempo << endl;
	io << "Função Objetivo: " << cplex.getObjValue() << endl;
	io << "GAP: " << cplex.getMIPRelativeGap() << endl;
	io << endl;
	for (int i = 0; i < r.nMaquinas; i++) {
		io << "Maquina " << i << ":\t";
		for (int j = 0; j < r.nTarefas; j++)
		{

			if (cplex.getValue(r.x[i][j]) == 1)
			{
				io << j << "\t";
			}
			else {
				//io << "\t";
			}

		}
		io << endl;
	}



}
int runDesignacao()
{
	string files[] = {
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_5_100.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_10_100.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_10_200a.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_10_200b.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_10_200c.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_20_200a.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_20_200b.txt",
		"C:/Users/lucao/Downloads/DesignacaoGeneralizada/PDG_20_200c.txt",


	};
	for (int f = 0; f < 8; f++) {
		Designacao m = readData(files[f]);
		IloEnv env;
		try {
			IloModel model(env);
			fo(env, model, m);
			restricoes(env, model, m);

			IloCplex cplex(env);
			cplex.setParam(IloCplex::ClockType, 2);
			cplex.setParam(IloCplex::TiLim, 3600);
			cplex.setParam(IloCplex::Threads, 4);
			//cplex.setOut(env.getNullStream());

			cplex.extract(model);
			long double time = (clock() / (double)CLOCKS_PER_SEC);
			cplex.solve();
			time = (double)(clock() / (double)CLOCKS_PER_SEC) - time;
			cout << "GAP: " << cplex.getMIPRelativeGap() << endl;
			cout << cplex.getObjValue() << endl;
			cout << time << endl;
			printValues(cplex, m, time, files[f]);
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
