#include "PMedianas.h"
PMedianas readData(string filename)
{
	PMedianas m;
	ifstream io(filename);
	io >> m.nMedianas;
	io >> m.nClientes;
	io >> m.nPlaces;
	for (int i = 0; i < m.nClientes; i++)
	{
		Cliente c;
		io >> c.x;
		io >> c.y;
		m.clientes.push_back(c);
	}
	for (int i = 0; i < m.nClientes; i++)
	{
		io >> m.clientes[i].demanda;
	}
	for (int i = 0; i < m.nPlaces; i++)
	{
		Place p;
		io >> p.x;
		io >> p.y;
		m.places.push_back(p);
	}
	io >> m.capacidade;
	for (int i = 0; i < m.nPlaces; i++)
	{
		m.places[i].capacidade = m.capacidade;
	}
	for (int i = 0; i < m.nClientes; i++)
	{
		vector<float> aux;
		for (int j = 0; j < m.nPlaces; j++)
		{
			aux.push_back(0);
			io >> aux[j];
		}
		//system("pause");
		m.D.push_back(aux);
	}
	return m;
}
IloExpr fo(IloEnv env, PMedianas &m)
{
	IloExpr obj(env);
	IloArray<IloNumVarArray> x(env, m.nClientes);
	IloNumVarArray y(env, m.nPlaces, 0, 1, ILOINT);
	for (int i=0; i < m.nClientes; i++)
	{
		x[i] = IloNumVarArray(env, m.nPlaces, 0, 1, ILOINT);
	}
	for (int i = 0; i < m.nClientes; i++)
	{
		for (int j = 0; j < m.nPlaces; j++) 
		{
			obj += m.D[i][j] * x[i][j];
		}
	}
	m.x = x;
	m.y = y;
	return obj;

}
void restricaoUnicidade(IloEnv env, IloModel &model, PMedianas &m)
{
	
	for (int i = 0; i < m.nClientes; i++)
	{
		IloExpr r(env);
		for (int j = 0; j < m.nPlaces; j++)
		{
			r += m.x[i][j];
		}
		model.add(r == 1);
		r.end();
	}
}
void restricaoFacilidades(IloEnv env, IloModel &model, PMedianas &m)
{
	IloExpr r(env);
	for (int j = 0; j < m.nPlaces; j++)
	{
		r += m.y[j];
	}
	model.add(r == m.nMedianas);
	for (int i = 0; i < m.nClientes; i++)
	{
		for (int j = 0; j < m.nPlaces; j++)
		{
			IloExpr ry(env);
			ry += m.x[i][j];
			model.add(ry <= m.y[j]);
		}
	}

}
void restricaoCapacidade(IloEnv env, IloModel &model,PMedianas &m)
{
	
	for (int j = 0; j < m.nPlaces; j++)
	{
		IloExpr r(env);
		for (int i = 0; i < m.nClientes; i++)
		{
			r += m.clientes[i].demanda*m.x[i][j];
		}
		model.add(r <= m.places[j].capacidade);
		r.end();
	}
}

void printValues(IloCplex cplex, PMedianas m, long double time)
{
	ofstream io("c:/Users/lucao/Downloads/pmedianas/pm_result2_" + std::to_string(m.nMedianas) + "_" + std::to_string(m.nClientes) + "_" + std::to_string(m.nPlaces) + ".txt");
	cout << "Facilidade Aberta nos locais..." << endl;
	io << time << endl;
	for (int i = 0; i < m.nPlaces; i++)
	{
		if (cplex.getValue(m.y[i]) == 1) {
			cout << i << "\t";
			io << endl << i << endl;
			io << endl << m.places[i].x << "," << m.places[i].y<< endl;
			for (int j = 0; j < m.nClientes; j++)
			{
				if (cplex.getValue(m.x[j][i]) == 1) 
				{
					io << j  << ";";
				}
			}
			io << endl;
		}
			
	}
	io << endl;
	io << cplex.getObjValue() << endl;
	
	
}

int runPMedianas()
{
	string files[] = {
		"c:/Users/lucao/Downloads/pmedianas/pm_5_70_50.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_5_80_50.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_5_100_50.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_5_200_50.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_10_100_50.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_10_200_100.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_10_300_100.txt",
		"c:/Users/lucao/Downloads/pmedianas/pm_10_400_100.txt"

	};
	for (int f = 0; f < 8; f++) {
		PMedianas m = readData(files[f]);
		IloEnv env;
		try {
			IloModel model(env);
			model.add(IloMinimize(env, fo(env, m)));
			restricaoUnicidade(env, model, m);
			restricaoCapacidade(env, model, m);
			restricaoFacilidades(env, model, m);

			IloCplex cplex(env);
			cplex.setParam(IloCplex::ClockType, 2);
			cplex.setParam(IloCplex::TiLim, 3600);
			cplex.setParam(IloCplex::Threads, 4);
			cplex.setOut(env.getNullStream());

			cplex.extract(model);
			long double time = (clock() / (double)CLOCKS_PER_SEC);
			cplex.solve();
			time = (double)(clock() / (double)CLOCKS_PER_SEC) - time;
			cout << "GAP: " <<cplex.getMIPRelativeGap() <<endl;
			cout << time << endl;
			printValues(cplex, m, time);
			model.end();
			env.end();
		}
		catch (IloException& e) {
			cerr << e.getMessage() << endl;
			e.end();
		}
		catch (...) {}

	}
	
	system("pause");
	return 0;
}