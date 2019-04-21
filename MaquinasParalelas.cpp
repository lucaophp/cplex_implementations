#include "MaquinasParalelas.h"
int BIGM;
MaquinasParalelas readData(string filename)
{
	MaquinasParalelas mp;
	ifstream io(filename);
	io >> mp.nTarefas >> mp.nMaquinas;
	for (int i = 0; i <= mp.nTarefas; i++)
	{
		vector<float> aux;
		for (int j = 0; j < mp.nMaquinas; j++)
		{
			aux.push_back(0);
			io >> aux[j];
			BIGM += aux[j];
		}
		mp.tempoProcessamento.push_back(aux);
	}
	for (int i = 0; i <= mp.nTarefas; i++)
	{
		mp.dataEntrega.push_back(0);
		io >> mp.dataEntrega[i];
	}
	vector<vector<vector<float>>> aux (mp.nTarefas+1, vector<vector<float> >(mp.nTarefas+1, vector<float>(mp.nMaquinas)));
	mp.setup = aux;
	for (int k = 0; k < mp.nMaquinas; k++)
	{
		
		for (int i = 0; i <= mp.nTarefas; i++)
		{
			
			for (int j = 0; j <= mp.nTarefas; j++)
			{

				
				if (j != 0)
					io >> mp.setup[i][j][k];
				else
					mp.setup[i][j][k] = 0;
				BIGM += mp.setup[i][j][k];
				
			}
			
			
			
		}

	}
	
	
	


	return mp;

}
void fo(IloEnv env, IloModel &model, MaquinasParalelas &mp)
{
	mp.T = IloNumVarArray(env, mp.nTarefas + 1, 0, IloInfinity, ILOFLOAT);
	IloExpr e(env);
	for (int i = 1; i <= mp.nTarefas; i++)
	{
		e += mp.T[i];

	}
	model.add(IloMinimize(env, e));
	//alocação
	IloArray<IloArray<IloNumVarArray>> x(env, mp.nTarefas+1);
	for (int j = 0; j <= mp.nTarefas; j++)
	{
		IloArray<IloNumVarArray> aux(env, mp.nTarefas+1);
		x[j] = aux;
		for (int k = 0; k <= mp.nTarefas; k++)
		{
			x[j][k] = IloNumVarArray(env, mp.nMaquinas, 0, 1, ILOINT);
		}
	}
	mp.C = IloNumVarArray(env, mp.nTarefas+1, 0, IloInfinity, ILOFLOAT);
	mp.x = x;
}
void restricoes(IloEnv env, IloModel &model, MaquinasParalelas &mp)
{

	
	for (int j = 1; j <= mp.nTarefas; j++)
	{
		IloExpr r(env);
		for (int i = 0; i <= mp.nTarefas; i++)
		{
			for (int k = 0; k < mp.nMaquinas; k++)
			{
				if(i != j)
					r += mp.x[i][j][k];
			}
		}
		model.add(r == 1);

	}
	for (int h = 1; h <= mp.nTarefas; h++)
	{
		
		for (int k = 0; k < mp.nMaquinas; k++)
		{
			IloExpr r(env);
			IloExpr r2(env);
			for (int j = 0,i = 0; j <= mp.nTarefas; j++, i++)
			{
				if (i != h)
					r += mp.x[i][h][k];
				if (j != h)
					r2 += mp.x[h][j][k];
			}
			model.add(r - r2 == 0);
		}

	}

	for (int i = 0; i <= mp.nTarefas; i++)
	{
		for (int j = 1; j <= mp.nTarefas; j++)
		{
			IloExpr sum1(env), sum2(env);
			for (int k = 0; k < mp.nMaquinas; k++)
			{
				sum1 += mp.x[i][j][k] * (mp.setup[i][j][k] + mp.tempoProcessamento[j][k]);
				
			}
			for (int k = 0; k < mp.nMaquinas; k++)
			{
				sum2 += mp.x[i][j][k];
			}
			model.add(mp.C[j] >= mp.C[i] + sum1+BIGM*(sum2-1));
		}
	}

	for (int k = 0; k < mp.nMaquinas; k++)
	{
		IloExpr sum1(env);
		for (int j = 0; j <= mp.nTarefas; j++)
		{
			sum1 += mp.x[0][j][k];
		}
		model.add(sum1 == 1);
	}


	model.add(mp.C[0] == 0);
	for (int j = 1; j <= mp.nTarefas; j++)
	{
		//model.add(mp.CMax >= mp.C[j]);
	}
	model.add(mp.T[0] == 0);
	for (int j = 1; j <= mp.nTarefas; j++)
	{
		model.add(mp.T[j] == (mp.C[j]-mp.dataEntrega[j]));
	}


}
void printSolution(IloCplex cplex, string filename,MaquinasParalelas &mp, float time)
{
	ofstream io(filename+"_result.txt");
	int from = 0;
	io << "GAP: \t " << cplex.getMIPRelativeGap() << endl;
	io << "FO: \t " << cplex.getObjValue() << endl;
	io << cplex.getStatus() << endl;
	io << time << endl;
	for (int k = 0; k < mp.nMaquinas; k++)
	{
		io << "Maquina " << k << ":" << "\t";
		bool stop = false;
		int to = 0;
		while (!stop)
		{
			if (cplex.getValue(mp.x[from][to][k]) == 1) {
				if (to == 0)
					stop = true;
				io << to << "\t";
				
				from = to;
				to = 0;
			}
			else {
				to++;
				if (to > mp.nTarefas)
					stop = true;
			}
			

		}
		io << endl;
	}
}
int runMaquinas()
{
	string files[] = {
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_10_2.txt",
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_10_3.txt",
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_15_2.txt",
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_15_3.txt",
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_20_2.txt",
		"c:/Users/lucao/Downloads/MaquinasParalelas/pmachine_20_3.txt",
	};
	for (int f = 0; f < 6; f++)
	{
		MaquinasParalelas m = readData(files[f]);
		
		IloEnv env;
		try {
			IloModel model(env);
			fo(env, model, m);
			
			restricoes(env, model, m);
			
			IloCplex cplex(env);
			cplex.setParam(IloCplex::ClockType, 2);
			cplex.setParam(IloCplex::TiLim, 3600);
			cplex.setParam(IloCplex::Threads, 8);
			//cplex.setOut(env.getNullStream());

			cplex.extract(model);
			long double time = (clock() / (double)CLOCKS_PER_SEC);
			cplex.solve();
			time = (double)(clock() / (double)CLOCKS_PER_SEC) - time;
			cout << "GAP: " << cplex.getMIPRelativeGap() << endl;
			cout << cplex.getObjValue() << endl;
			printSolution(cplex, files[f], m, time);
			cout << time << endl;

		}
		catch (...) {
			cout << "Error" << endl;
		}
	}
	return 0;
}