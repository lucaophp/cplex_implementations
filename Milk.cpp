#include "Milk.h"
#define foreach(var,fim) for(var = 0; var < fim;var++)
#define foreach1(var,fim) for(var = 1; var < fim;var++)
Milk readData(string filename)
{
	Milk m;
	string descarte;
	int i=0,j=0,k = 0,v = 0;
	ifstream io(filename);
	io >> m.nVeiculos;
	
	io >> m.nClientes;
	m.nClientes++;
	cout << m.nVeiculos<<"---"<<m.nClientes << endl;
	
	io >> m.VM;
	
	//pego os custos fixos de cada veiculo
	vector<float> cf(m.nVeiculos);
	m.cf = cf;
	foreach(i,m.nVeiculos) 
	{
		io >> m.cf[i];
	}
	
	//custo variavel
	vector<float> cv(m.nVeiculos);
	foreach(i, m.nVeiculos)
	{
		io >> cv[i];
	}
	
	//capacidade de cada caminhão
	vector<float> Q(m.nVeiculos);
	m.Q = Q;
	foreach(i, m.nVeiculos)
	{
		io >> m.Q[i];
	}
	
	//ponto do deposito
	io >> m.deposito.x >> m.deposito.y;
	
	//pontos de coleta
	vector<Point> coletas(m.nClientes);
	m.produtores = coletas;
	foreach(i, m.nClientes-1)
	{
		Point aux;
		
		io >> aux.x >> aux.y;
		m.produtores.push_back(aux);
	}
	
	cout << descarte << endl;
	//tempo de processamento
	vector<float> p(m.nClientes);
	m.p = p;
	foreach(i, m.nClientes)
	{
		io >> m.p[i];
		m.p[i] /= 60;
	}
	
	//data minima
	vector<float> R(m.nClientes);
	m.R = R;
	foreach(i, m.nClientes)
	{
		io >> m.R[i];
		m.p[i] /= 60;
	}
	
	//oferta de cada ponto
	vector<float> q(m.nClientes);
	m.q = q;
	foreach(i, m.nClientes)
	{
		io >> m.q[i];
	}
	
	//distancia de cada ponto ate outro ponto
	vector<vector<float>> d(m.nClientes,vector<float>(m.nClientes));
	vector<vector<vector<float>>> c(m.nClientes, vector<vector<float>>(m.nClientes,vector<float>(m.nVeiculos)));
	m.d = d;
	m.c = c;
	foreach(i, m.nClientes)
	{
		foreach(j, m.nClientes)
		{
			io >> m.d[i][j];
			foreach(v, m.nVeiculos)
			{
				m.c[i][j][v] = m.d[i][j] * cv[v];
			}
		}
	}
	return m;

}
void fo(IloEnv env, IloModel &model, Milk &r)
{
	IloExpr obj(env);
	IloArray<IloArray<IloNumVarArray>> x(env, r.nClientes);
	for (int i = 0; i < r.nClientes; i++)
	{
		IloArray<IloNumVarArray> aux(env,r.nClientes);
		for (int j = 0; j < r.nClientes; j++)
			aux[j] = IloNumVarArray(env, r.nClientes, 0, 1, ILOINT);
		x[i] = aux;

	}
	IloNumVarArray y(env, r.nVeiculos,0,1,ILOINT);
	IloArray<IloNumVarArray> s(env, r.nClientes);
	for (int i = 0; i < r.nClientes; i++)
		s[i] = IloNumVarArray(env, r.nVeiculos,0,IloInfinity,ILOFLOAT);

	IloArray<IloNumVarArray> f(env, r.nClientes);
	for (int i = 0; i < r.nClientes; i++)
	{
		f[i] = IloNumVarArray(env, r.nClientes, 0, IloInfinity, ILOINT);
	}
	r.f = f;
	r.x = x;
	r.s = s;
	r.y = y;
	for (int i = 1; i < r.nClientes; i++)
	{
		for (int j = 1; j < r.nClientes; j++)
		{
			for (int v = 0; v < r.nVeiculos;v++)
				obj += r.c[i][j][v] * r.x[i][j][v];
		}
	}
	for (int v = 0; v < r.nVeiculos; v++)
		obj += r.cf[v] * r.y[v];
	obj = IloExpr(env);
	for (int i = 0; i < r.nClientes; i++) {
		for (int j = 1; j < r.nClientes; j++)
		{
			for (int v = 0; v < r.nVeiculos; v++)
			{
				obj += r.c[i][j][v]*r.x[i][j][v];

			}
		}
	}
	
	
	model.add(IloMinimize(env, obj));
}


void restricoes(IloEnv env, IloModel &model, Milk &r)
{
	int i, j, l, v;
	for (int i = 1; i < r.nClientes; i++)
	{
		IloExpr restricao(env);
		for (int j = 0; j < r.nClientes; j++)
		{
			for (int v = 0; v < r.nVeiculos; v++) {
				if(i!=j)
				restricao += r.x[i][j][v];
			}
				

		}
		model.add(restricao == 1);

	}
	for (int j = 1; j < r.nClientes; j++)
	{
		IloExpr restricao(env);
		for (int i = 0; i < r.nClientes; i++)
		{
			for (int v = 0; v < r.nVeiculos; v++) {
				if (i != j)
					restricao += r.x[i][j][v];
			}

		}
		model.add(restricao == 1);

	}
	for (int v = 0; v < r.nVeiculos; v++) 
	{
		for (int l = 0; l < r.nClientes; l++)
		{
			IloExpr restricao(env);
			for (int i = 0; i < r.nClientes; i++)
			{
				if(i != l)
					restricao += r.x[i][l][v];
			}
			IloExpr restricao2(env);
			for (int j = 0; j < r.nClientes; j++)
			{
				if (j != l)
					restricao2 += r.x[l][j][v];
			}
			model.add(restricao-restricao2 == 0);
		}
	}
	for (int v = 0; v < r.nVeiculos; v++)
	{
		IloExpr restricao(env);
		for (int i = 0; i < r.nClientes; i++)
		{
			IloExpr soma(env);
			for (int j = 0; j < r.nClientes; j++)
			{
				if (i != j)
					soma += r.x[i][j][v];

			}
			
			restricao += r.q[i] * soma;
		}
		cout << r.Q[v] << endl;
		model.add(restricao <= r.y[v]*r.Q[v]);
		
	}
	for (int i = 0; i < r.nClientes; i++)
	{
		for (int v = 0; v < r.nVeiculos; v++)
		{
			model.add(r.s[i][v] >= r.R[i]);
		}
	}
	int BIGM = r.nClientes*r.nVeiculos;
	r.Dl = 48*60;
	for (int v = 0; v < r.nVeiculos; v++)
	{
		for (int i = 0; i < r.nClientes; i++)
		{
			for (int j = 0; j < r.nClientes; j++) {
				IloExpr soma(env);
				for (int k = 0; k < r.nClientes; k++)
				{
					soma += (1 - r.x[i][k][v]);

				}
				model.add(r.s[j][v]+r.p[j]+((r.d[0][j]/r.VM))-BIGM*soma <= r.Dl+r.R[i]);
			}
				
		}
	}
	for (int v = 0; v < r.nVeiculos; v++)
	{
		for (int i = 0; i < r.nClientes; i++)
		{
			for (int j = 0; j < r.nClientes; j++) 
			{
				model.add(r.s[j][v] >= r.s[i][v]+r.p[i]+((r.d[i][j]/r.VM))-BIGM*(1-r.x[i][j][v]));
			}

		}
	}
	for (int v = 0; v < r.nVeiculos; v++)
	{
		for (int i = 0; i < r.nClientes; i++)
		{
			IloExpr soma(env);
			for (int j = 0; j < r.nClientes; j++)
			{
				if(i!=j)
				soma += r.x[i][j][v];
			}
			model.add(soma <= 1);

		}
	}
	for (int v = 0; v < r.nVeiculos; v++)
	{
		for (int j = 0; j < r.nClientes; j++)
		{
			IloExpr soma(env);
			for (int i = 0; i < r.nClientes; i++)
			{
				if(i!=j)
				soma += r.x[i][j][v];
			}
			model.add(soma <= 1);

		}
	}
	for (int v = 0; v < r.nVeiculos; v++)
	{
		for (int i = 1; i < r.nClientes; i++)
		{
			for (int j = 1; j < r.nClientes; j++)
			{
				model.add(r.x[i][j][v]+r.x[j][i][v] <= r.y[v]);
			}
			

		}
	}




	
}
void printValues(IloCplex cplex, Milk &r, long double tempo)
{
	ofstream io("C:/Users/lucao/Downloads/RoteamentoDeVeiculos/MilkRun/VRP" + std::to_string(r.nClientes) + "_"+to_string(r.nVeiculos)+"_result.txt");
	io << tempo << endl;
	io << "Função Objetivo: " << cplex.getObjValue() << endl;
	io << "GAP: " << cplex.getMIPRelativeGap() << endl;
	for (int i = 0; i < r.nClientes; i++)
	{
		Point p = r.produtores[i];
		io << p.x << "\t" << p.y << endl;
	}
	io << endl;
	int v;
	foreach(v, r.nVeiculos) {
		float qtdTotal = 0.0;
		for (int i = 0; i < r.nClientes; i++) {
			for (int j = 0; j < r.nClientes; j++)
			{
				if (cplex.getValue(r.x[i][j][v]) == 1)
				{
					qtdTotal += r.q[j];
					io << i << "\t" << j << "\t" << r.q[j] << endl;
				}
			}
		}
		io << v << "-" << qtdTotal << endl;
		io << endl;
	}
	



}
int runMilk()
{
	string files[] = {
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/MilkRun/vrp3_1_norm.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/MilkRun/vrp25_6_norm.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/MIlkRun/vrp30_3_norm.txt",
		"C:/Users/lucao/Downloads/RoteamentoDeVeiculos/MIlkRun/vrp30_6_norm.txt"
	};
	for (int f = 0; f < 4 ; f++) {
		Milk m = readData(files[f]);
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
