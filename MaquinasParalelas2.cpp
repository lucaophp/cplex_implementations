#include "MaquinasParalelas.h"
MaquinasParalelas readData(string filename) {
	MaquinasParalelas m;
	ifstream io(filename);
	io >> m.nMaquinas;
	io >> m.nTarefas;
	m.nTarefas++;
	for (int i = 0; i < m.nTarefas; i++)
	{
		vector<float> aux;
		for (int k = 0; k < m.nMaquinas; k++)
		{
			if (k == 0)
			{
				aux.push_back(0);
			}
			else {
				aux.push_back(0);
				io >> aux[k];
			}

		}
		m.tempoProcessamento.push_back(aux);
	}
	for (int i = 0; i < m.nTarefas; i++)
	{
		m.dataEntrega.push_back(0);
		io >> m.dataEntrega[i];
	}
	
	
	return m;

}

void fo(IloEnv env, IloModel &model, MaquinasParalelas &m)
{
	IloExpr obj(env);
	m.T = IloNumVarArray(env, m.nTarefas, 0, IloInfinity, ILOFLOAT);
	for (int i = 1; i < m.nTarefas; i++)
	{
		obj += m.T[i];
	}
	
	model.add(IloMinimize(env, obj));
}
void restricoes(IloEnv env, IloModel &model, MaquinasParalelas &m)
{
	//alocação
	IloArray<IloArray<IloNumVarArray>> x(env, m.nTarefas);
	for (int j = 0; j < m.nTarefas; j++)
	{
		IloArray<IloNumVarArray> aux(env, m.nTarefas);
		x[j] = aux;
		for (int k = 0; k < m.nTarefas; k++)
		{
			x[j][k] = IloNumVarArray(env, m.nMaquinas, 0, 1, ILOINT);
		}
	}
	IloArray<IloNumVarArray> C(env, m.nTarefas);
	for (int i = 0; i < m.nTarefas; i++)
	{
		C[i] = IloNumVarArray(env, m.nMaquinas, 0, IloInfinity, ILOFLOAT);
	}
	m.C = C;
	m.x = x;
	//----------------
	for (int j = 1; j < m.nTarefas; j++)
	{
		IloExpr r(env);
		for (int k = 0; k < m.nMaquinas; k++)
		{
			for (int i = 0; i < m.nTarefas; i++)
			{
				r += m.x[i][j][k];
			}
		}
		model.add(r == 1);

	}
	for (int k = 0; k < m.nMaquinas; k++)
	{
		IloExpr r(env);
		for (int j = 1; j < m.nTarefas; j++)
		{
			r += m.x[0][j][k];
		}
		model.add(r <= 1);

	}
	for (int h = 1; h < m.nTarefas; h++)
	{
		for (int k = 1; k < m.nMaquinas; k++)
		{
			IloExpr re(env);
			IloExpr rd(env);
			for (int i = 0,j = 0; i < m.nTarefas; i++,j++)
			{
				if(i !=h)
					re += m.x[i][h][k];
				if(j != h)
					rd += m.x[h][j][k];

			}
			model.add(re - rd == 0);

		}
	}
	for (int k = 0; k < m.nMaquinas; k++)
	{
		model.add(m.C[0][k] == 0);
	}
	for (int k = 1; k < m.nMaquinas; k++)
	{
		for (int i = 0; i < m.nTarefas; i++)
		{
			for (int j = 1; j < m.nTarefas; j++)
			{
				model.add(m.C[j][k]  >= ((m.C[i][k]-BIG_M)+((BIG_M+m.tempoProcessamento[j][k])*m.x[i][j][k])));
			}

		}
	}
	for (int i = 1; i < m.nTarefas; i++)
	{
		for (int k = 1; k < m.nMaquinas; k++)
		{
			model.add(m.T[i] >= (m.C[i][k]-m.dataEntrega[i]));

		}
	}

}

int runMaquinas()
{
	string files[] = {
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_10_2.txt"
	};
	for (int f = 0; f < 1; f++)
	{
		MaquinasParalelas m = readData(files[f]);
		system("pause");
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
			cout << time << endl;
			
		}
		catch (...) {

		}
	}
	return 0;
}
