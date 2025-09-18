#include <iostream>

#include <string>
using namespace std;

int main() {
	string patente[10][3]; 
	int nrocarnetdeconducir; 
	int cantmultas; int cont;
		int cantacarreos;
	int tipodemulta[6];
	int mes;
	int dia;
	
	//para correr el algoritmo la cantidad de motos registradas sera 10.
	  for (int i=0; i=10; i++)
	{
		cout<<"ingrese su numero de patente";
		getline (cin,patente[i][0]);
		cout<<"ingrese su numero de carnet de conducir";
		cin>>nrocarnetdeconducir;
		cout<<"ingrese cantidad de multas";
		cin>>cantmultas;
		cout<<"ingrese cantidad de acarreos";
		cin>>cantacarreos;
		// patente[patente][];
}
	  cout<<"ingrese numero de patente";
	  getline(cin,patente[0][0]);
	while (patente!="x");
	cout<<"ingrese cantidad de multas"; cout<<endl;
	cin>>cantmultas;
	for (int i=0; i=cantmultas; i++)
		cont=0;
	{
		cout<<"ingrese tipo de multa de forma numerica (1: EXCESO DE VELOCIDAD, 2: DOCUMENTACIÓNVENCIDA, 3: CONDUCIR SIN LICENCIA, 4: CONDUCIR SIN CASCO, 5: CONDUCIR ALCOHOLIZADO)";
	cin>>tipodemulta[i];
	
	if (tipodemulta[tipodemulta]=3 or tipodemulta[tipodemulta]=4 or tipodemulta[tipodemulta]=5)
		 {
	tipodemulta[tipodemulta]= cont+1;
		cantacarreos=cont;
	else 
		cout<<"esta multa no requiere retencion de moto";
	}

	}

/*

	int main() {
		int numero[5];
		string alumnos[5];
		
		for (int i=0; i<=4; i++)
		{	cout<<"ingrese  nombre del alumno"<<endl;
		getline(cin,alumnos[i]);
		cout<<"ingrese edad del alumno"<<endl;
		cin>>numero[i];
		cin.get();
		
		}
		for (int i=0; i<=4; i++)
			
			cout<<"la edad del "<<alumnos[i]<<" es "<<numero[i]<<endl;
		
		
		return 0;
	}
	
	




