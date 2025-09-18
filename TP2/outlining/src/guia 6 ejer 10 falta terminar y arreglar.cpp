#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
	/*10) Una fábrica cuenta con los datos de sus 25 clientes, que se leen al inicio: código de cliente 
	(no correlativo), Nro de documento y Estado (A: Paga a término, B: Moroso, C: Incobrable).
		Desea actualizar el Estado, según lo ocurrido en el mes. Para lo cual se rige por las siguientes reglas: Si
		el cliente paga en fecha el estado, es A. Si el pago se realiza luego de la fecha requerida de cobro, es B.
		Si no paga es C.
		Se ingresan los pagos del mes: código de cliente, fecha de pago, fecha en que debía pagar, monto. El
		ingreso no sigue ningún orden, y los clientes realizan pagos mensuales. (Sólo uno por cliente, o ninguno
		si no paga). Las fechas se leen como aaaa, mm, dd (3 variables diferentes)
		Se desea obtener:
		a) La cantidad total de clientes de cada tipo.
		b) Realizar un listado ordenado en forma ascendente por Nro. de Documento de los clientes Incobrables.*/
	
	string estado[26];
	int doc[26];
	int codigo;
	int cont=0;
	int contestadoA=0;
	int contestadoB=0;
	int contestadoC=0;
	int cont2;
	int fecha[26][4];
	int fechadestado;
	int auxdoc[26];
	int i;
	cout<<"ingrese codigo de cliente"<<endl;
	cin>>codigo;
	
	while (codigo!=0){
		cont++;
		cout<<"ingrese numero de documento del cliente"<<endl;
		cin>>doc[codigo];
		cout<<"ingrese Estado del cliente (A: Paga a termino, B: Moroso, C: Incobrable). "<<endl;
		getline(cin,estado[codigo]);
		cin.get();
		cout<<"ingrese codigo de cliente"<<endl;
		cin>>codigo;
		cin.get();
		 cont2=cont;
	}
	for(int i=1; i<=cont2; i++){
		cout<<"ingrese codigo de cliente"<<endl;
		cin>>codigo;
		cout<<"fecha de pago como aaaa mm dd, si no pago ponga 0"<<endl;
		cin>>fecha[codigo][1];
		cout<<"fecha que debia pagar como aaaa mm dd"<<endl;
		cin>>fecha[codigo][2];
		cout<<"monto que debe pagar o que pago "<<endl;
		cin>>fecha[codigo][3];
		
		fechadestado=fecha[codigo][2]-fecha[codigo][1];
		if (fechadestado==fecha[codigo][2]){
			estado[codigo]='C';
			contestadoC++;
			auxdoc[codigo]=doc[codigo];
			
		} else if (fechadestado>=0){
			estado[codigo]= 'A';
			contestadoA++;
		}
			if (fechadestado<0){
				estado[codigo]='B';
				contestadoB++;
		}
		
		}
	    ///a) La cantidad total de clientes de cada tipo.
		cout<<"la cantidad total de clientes del tipo A son: "<<contestadoA<<endl;
		cout<<"la cantidad total de clientes del tipo B son: "<<contestadoB<<endl;
		cout<<"la cantidad total de clientes del tipo C son: "<<contestadoC<<endl;
		///b) Realizar un listado ordenado en forma ascendente por Nro. de Documento de los clientes Incobrables.
		
		int j;
		int aux;
		/*for (int i=0; i<=cont2-1;i++){
			for(int j=i+1; j<=cont2; j++){
				if (doc[i]>doc[j]){
				aux=doc[j];
				doc[j]= doc[i];
				doc[i]=aux;
				}
			}
				
		}
*/int cont3;
cont3=cont2+1;

		for(i=1; i<cont3-1; i++)
		{
			for(j=i+1; j<cont3; j++)
			{
				if(doc[i]>doc[j])
				{
					aux=doc[i];
					doc[i]=doc[j];
					doc[j]=aux;
				}
			}
		}
		
		for(int i=0; i<cont2; i++){
			
			
			cout<<auxdoc[i]<<endl;
			
		}
			
		/*4
		45310319
		B
		2
		44316300
		C
		3
		43555333
		A
		1
		47310310
		c
		0
		4
		20230117
		20230130
		5000
		2
		0
		20230130
		1000000
		3
		20230131
		20230130
		50000
		1
		0
		20230130
		5000
		*/
		
		
	return 0;
}
