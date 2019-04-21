#include <ilcplex\ilocplex.h>
#include <vector>
ILOSTLBEGIN
struct Mochila {
	int nObj;
	float capacity;
	vector<float> p;
	vector<float> c;
	IloNumVarArray x;
};
Mochila readData(string filename)
{
	Mochila m;
	ifstream io(filename);
	io >> m.nObj;
	io >> m.capacity;
	for (int i = 0; i < m.nObj; i++) 
	{
		float val;
		io >> val;
		m.p.push_back(val);
	}
	for (int i = 0; i < m.nObj; i++)
	{
		float val;
		io >> val;
		m.c.push_back(val);
	}
	return m;



}
IloExpr fo(IloEnv env,Mochila &m) 
{
	IloExpr obj(env);

	IloNumVarArray x(env, m.nObj, 0, 1, ILOINT);
	m.x = x;
	for (int i = 0; i < m.nObj; i++) 
	{
		obj += m.p[i] * m.x[i];
	}
	
	return obj;

}
IloExpr restricao(IloEnv env,Mochila &m)
{
	IloExpr r(env);
	for (int i = 0; i < m.nObj; i++)
	{
		r += m.c[i] * m.x[i];
	}
	return r;
}
int main3() 
{
	IloEnv env;
	try {
		IloModel model(env);
		IloExpr obj(env);
		//IloNumVarArray x(env,);
		Mochila m = readData("mochila.csv");
		model.add(IloMaximize(env,fo(env, m)));
		//constraints
		model.add(restricao(env, m) <= m.capacity);
		IloCplex cplex(env);
		cplex.setOut(env.getNullStream());
		cplex.extract(model);
		cplex.solve();
		for (int i = 0; i < m.nObj; i++)
		{
			if(cplex.getValue(m.x[i])==1)
			{
				cout << i << '\t';
			}
		}
		cout << endl;
		env.end();
	}
	catch (...) {

	}
	system("pause");
	
	
	return 0;
}