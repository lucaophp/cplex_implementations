#include "TSP.h"
TSP readData(string filename)
{
	ifstream io(filename);
	TSP tsp;
	io >> tsp.n;
	for (int i = 0; i < tsp.n; i++)
	{
		Point p;
		io >> p.x >> p.y;
		tsp.points.push_back(p);
	}
	for (int i = 0; i < tsp.n; i++)
	{
		vector<float> aux;
		for (int j = 0; j < tsp.n; j++)
		{
			aux.push_back(0);
			io >> aux[j];
		}
		tsp.d.push_back(aux);
	}
	io.close();
	return tsp;
}
void fo(IloEnv env, IloModel &model, TSP &tsp)
{
	IloExpr obj(env);
	IloArray<IloNumVarArray> x(env, tsp.n), f(env, tsp.n);
	for (int i = 0; i < tsp.n; i++)
	{
		x[i] = IloNumVarArray(env, tsp.n, 0, 1, ILOINT);
		f[i] = IloNumVarArray(env, tsp.n, 0, IloInfinity, ILOINT);
	}
	tsp.x = x;
	tsp.f = f;
	for (int i = 0; i < tsp.n; i++)
	{
		for (int j = 0; j < tsp.n; j++)
		{
			obj += tsp.d[i][j] * tsp.x[i][j];

		}
	}
	model.add(IloMinimize(env, obj));
}
void restricoes(IloEnv env, IloModel &model, TSP &tsp)
{
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int i = 0; i < tsp.n; i++)
		{
			r += tsp.x[i][k];
		}
		model.add(r == 1);
	}
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int j = 0; j < tsp.n; j++)
		{
			r += tsp.x[k][j];
		}
		model.add(r == 1);
	}
	for (int k = 1; k < tsp.n; k++)
	{
		IloExpr r(env), r1(env);
		for (int i = 0, j = 0; i < tsp.n; i++, j++)
		{
			r += tsp.f[i][k];

			r1 += tsp.f[k][j];
		}
		model.add(r - r1 == 1);
	}
	for (int i = 0; i < tsp.n; i++)
	{
		for (int j = 0; j < tsp.n; j++)
		{
			if (i != j)
				model.add(tsp.f[i][j] <= (tsp.n - 1)*tsp.x[i][j]);
		}
	}
}
void restricoesModelo2(IloEnv env, IloModel &model, TSP &tsp)
{
	IloNumVarArray u(env, tsp.n-1, 0, tsp.n, ILOINT);
	tsp.u = u;
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int i = 0; i < tsp.n; i++)
		{
			if(i!=k)
				r += tsp.x[i][k];
		}
		model.add(r == 1);
	}
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int j = 0; j < tsp.n; j++)
		{
			if(j!=k)
				r += tsp.x[k][j];
		}
		model.add(r == 1);
	}

	for (int i = 1; i < tsp.n; i++)
	{
		for (int j = 1; j < tsp.n; j++)
			if(i != j)
				model.add((tsp.u[i-1] - tsp.u[j-1] + tsp.n*tsp.x[i][j]) <= (tsp.n - 1));
		
			
	}


}

void restricoesModelo3(IloEnv env, IloModel &model, TSP &tsp)
{
	IloNumVarArray u(env, tsp.n, 0, IloInfinity, ILOINT);
	tsp.u = u;
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int i = 0; i < tsp.n; i++)
		{
			if (i != k)
				r += tsp.x[i][k];
		}
		model.add(r == 1);
	}
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int j = 0; j < tsp.n; j++)
		{
			if (j != k)
				r += tsp.x[k][j];
		}
		model.add(r == 1);
	}
	for (int i = 0; i < tsp.n; i++)
	{
		for (int j = 0; j < tsp.n; j++) 
		{
			if(i != j)
				model.add(tsp.x[i][j]+tsp.x[j][i] <= 1);
		}

	}

	for (int i = 1; i < tsp.n; i++)
	{
		for (int j = 1; j < tsp.n; j++)
			if (i != j)
				model.add((tsp.u[i] - tsp.u[j] + tsp.n*tsp.x[i][j]) <= (tsp.n - 1));


	}
	model.add(tsp.u[0] == 1);
	for (int i = 0; i < tsp.n; i++)
	{
		//model.add(tsp.u[i] == 1);
	}
	for (int i = 1; i < tsp.n; i++)
	{
		model.add(2 <= tsp.u[i] <= tsp.n);
	}



}
void restricoesModelo4(IloEnv env, IloModel &model, TSP &tsp)
{
	IloNumVarArray u(env, tsp.n, 0, IloInfinity, ILOFLOAT);
	tsp.u = u;
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int i = 0; i < tsp.n; i++)
		{
			if(i != k)
				r += tsp.x[i][k];
		}
		model.add(r == 1);
	}
	for (int k = 0; k < tsp.n; k++)
	{
		IloExpr r(env);
		for (int j = 0; j < tsp.n; j++)
		{
			if (j != k)
				r += tsp.x[k][j];
		}
		model.add(r == 1);
	}
	for (int i = 0; i < tsp.n; i++)
	{
		for (int j = 0; j < tsp.n; j++)
		{
			if (i != j)
				model.add(tsp.x[i][j] + tsp.x[j][i] <= 1);
		}

	}
	model.add(tsp.u[0] == 1);
	for (int i = 1; i < tsp.n; i++)
	{
		model.add(2 <= tsp.u[i] <= tsp.n);
	}
	for (int j = 1; j < tsp.n; j++)
	{	
		model.add(tsp.u[j]-tsp.u[0]+(tsp.n-2)*tsp.x[0][j] <= tsp.n-1);
	}

	for (int i = 1; i < tsp.n; i++)
	{
		model.add(tsp.u[0] - tsp.u[i] + (tsp.n-1)*tsp.x[i][0] <= tsp.n - 1);
	}
	for (int j = 1; j < tsp.n; j++)
	{
		//for (int i = 0; i < tsp.n; i++)
			//if (i != j)
				//model.add((tsp.u[i] - tsp.u[j] + tsp.n*tsp.x[j][i]) <= (tsp.n - 1));


	}
	for (int i = 1; i < tsp.n; i++)
	{
		//for (int j = 0; j < tsp.n; j++)
			//if (i != j)
				//model.add((tsp.u[i] - tsp.u[j] + (tsp.n-2)*tsp.x[j][i]) <= (tsp.n - 1));


	}
	for (int j = 1; j < tsp.n; j++)
	{
		for (int i = 1; i < tsp.n; i++)
			if (i != j)
				model.add((tsp.u[i] - tsp.u[j] + tsp.n*tsp.x[i][j]+(tsp.n-2)*tsp.x[j][i]) <= (tsp.n - 1));


	}

	



}
void printValues(IloCplex cplex, TSP &tsp, long double tempo)
{
	ofstream io("c:/Users/lucao/Downloads/CaixeiroViajante/TSP_RESULT4_" + to_string(tsp.n) + ".txt");
	io << tempo << endl;
	io << "Função Objetivo: " << cplex.getObjValue() << endl;
	io << "GAP: " << cplex.getMIPRelativeGap() << endl;
	io << endl;
	for (int i = 0; i < tsp.n; i++) {
		for (int j = 0; j < tsp.n; j++)
		{
			try {
				if (cplex.getValue(tsp.x[i][j]) == 1)
				{
					io << tsp.points[j].x << "\t" << tsp.points[j].y << endl;
				}
				else {
					//io << "\t";
				}

			}
			catch (...)
			{

			}

		}
	}
	for (int i = 0; i < tsp.n; i++) {
		for (int j = 0; j < tsp.n; j++)
		{
			try {
				if (cplex.getValue(tsp.x[i][j]) == 1)
				{
					io << i << "," << j << "\t";
				}
				else {
					//io << "\t";
				}

			}catch(...)
			{ }
			

		}
	}
	io.close();



}

int runTSP()
{
	string files[] = {
		"c:/Users/lucao/Downloads/CaixeiroViajante/TSP_30.txt",
		"c:/Users/lucao/Downloads/CaixeiroViajante/TSP_35.txt",
		"c:/Users/lucao/Downloads/CaixeiroViajante/TSP_40.txt",
		"c:/Users/lucao/Downloads/CaixeiroViajante/TSP_50.txt",
		"c:/Users/lucao/Downloads/CaixeiroViajante/TSP_80.txt",
		"c:/Users/lucao/Downloads/CaixeiroViajante/TSP_100.txt"
	};
	for (int f = 0; f < 6; f++)
	{
		TSP m = readData(files[f]);
		IloEnv env;
		try {
			IloModel model(env);
			fo(env, model, m);
			//restricoesModelo2(env, model, m);
			//restricoesModelo3(env, model, m);
			restricoesModelo4(env, model, m);
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
			printValues(cplex, m, time);

		}
		catch (IloException& e) {
			cerr << e.getMessage() << endl;
			e.end();
		}
		catch (...) {

		}
	}
	return 0;
}