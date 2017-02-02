#include "gurobi_c++.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <vector>
#include <list>
#include <string>
#include <iterator>
#include <math.h>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;

//estrutura que armazena todos os par�metros de entrada e sa�da associados a um job.
struct job{
	
	//input data
	//int instance;
	int index;
	double releaseTime;
	double processingTime;
	double dueDate;
	double penaltyWeight;
	double tardiWeight;
	double earlyWeight;

	//output data
	double cj; //completion time
	double sj; //start time
	double lj; //lateness
	double uj; //unit penalty (=1 if job is completed after its due date)
	double tj; //tardiness
	double ej; //earliness
	
	//data for static programming
	//double latenessEqWeight;		//diferen�a entre o peso de adiantamento e o de atraso (WE-WT)
	//double weightedProcessingTime;  //atributo que ser� utilziado para ordenar os jobs (WEj-WTj)/pj

	//data for dynamic programming
	double weightedSlackTime;
	//double latenessEqWeight;
	//double weightedSlack;
};

//struct objetivo: code, formula e value
struct objectiveFunction{
	int code;
	string formula;
	double value;
};

//estrutura que armazena o movimento, do tipo 'node 1 substitui seu antecessor node2, passando a ser processado imediatamente antes' do mesmo
struct movement{
	int node1; //elemento originalmente processado depois do node2;
	int node2; //elemento que foi substitu�do pelo seu subsequente;
};

//estrutura que armazena uma solu��o vi�vel do problema, incluindo a sequencia de jobs, o valor da fun��o objetivo selecionada e o movimento que gerou tal solu��o
struct solution{
	vector<int> sequence;
	string objFormula;
	double objValue;
	movement moveTrack;
};


//Fun��o que l� o n�mero da inst�ncia e vetor com jobs dessa inst�ncia. Cada elemento j do vetor representa um job, j� com os atributos preenchidos
vector <job> readInstanceData(int instance){
	
	job aux;
	vector<job> orderData;
		
	if (instance == 1){
		aux.index=	1	; aux.releaseTime=	2	; aux.processingTime=	10.8	; aux.dueDate=	95	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.9	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	2	; aux.releaseTime=	1	; aux.processingTime=	11.7	; aux.dueDate=	93	; aux.penaltyWeight=	0.1	; aux.tardiWeight=	0.6	; aux.earlyWeight=	0.7	; orderData.push_back(aux);
		aux.index=	3	; aux.releaseTime=	2	; aux.processingTime=	19,0	; aux.dueDate=	91	; aux.penaltyWeight=	0,0	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.9	; orderData.push_back(aux);
		aux.index=	4	; aux.releaseTime=	1	; aux.processingTime=	16.2	; aux.dueDate=	98	; aux.penaltyWeight=	0.5	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	5	; aux.releaseTime=	1	; aux.processingTime=	11.3	; aux.dueDate=	100	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.1	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	6	; aux.releaseTime=	2	; aux.processingTime=	5.1	; aux.dueDate=	98	; aux.penaltyWeight=	0.4	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	7	; aux.releaseTime=	0	; aux.processingTime=	12.5	; aux.dueDate=	102	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.6	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	8	; aux.releaseTime=	0	; aux.processingTime=	20.9	; aux.dueDate=	92	; aux.penaltyWeight=	0.7	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	9	; aux.releaseTime=	0	; aux.processingTime=	18.1	; aux.dueDate=	93	; aux.penaltyWeight=	0.5	; aux.tardiWeight=	0.8	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	10	; aux.releaseTime=	2	; aux.processingTime=	4.3	; aux.dueDate=	100	; aux.penaltyWeight=	0.2	; aux.tardiWeight=	0.1	; aux.earlyWeight=	0.1	; orderData.push_back(aux);
	}

	if (instance == 2){
		aux.index=	1	; aux.releaseTime=	1	; aux.processingTime=	10.4	; aux.dueDate=	98	; aux.penaltyWeight=	0.1	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.9	; orderData.push_back(aux);
		aux.index=	2	; aux.releaseTime=	1	; aux.processingTime=	5.6	; aux.dueDate=	96	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	3	; aux.releaseTime=	1	; aux.processingTime=	7.7	; aux.dueDate=	102	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.1	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	4	; aux.releaseTime=	0	; aux.processingTime=	20.7	; aux.dueDate=	101	; aux.penaltyWeight=	0.1	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.3	; orderData.push_back(aux);
		aux.index=	5	; aux.releaseTime=	2	; aux.processingTime=	17.5	; aux.dueDate=	99	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.3	; orderData.push_back(aux);
		aux.index=	6	; aux.releaseTime=	0	; aux.processingTime=	4.6	; aux.dueDate=	100	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.8	; aux.earlyWeight=	0.3	; orderData.push_back(aux);
		aux.index=	7	; aux.releaseTime=	1	; aux.processingTime=	12.6	; aux.dueDate=	103	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.3	; aux.earlyWeight=	0,0	; orderData.push_back(aux);
		aux.index=	8	; aux.releaseTime=	1	; aux.processingTime=	7.7	; aux.dueDate=	95	; aux.penaltyWeight=	0.9	; aux.tardiWeight=	0.6	; aux.earlyWeight=	0.8	; orderData.push_back(aux);
		aux.index=	9	; aux.releaseTime=	2	; aux.processingTime=	10,0	; aux.dueDate=	97	; aux.penaltyWeight=	0.5	; aux.tardiWeight=	0.1	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	10	; aux.releaseTime=	1	; aux.processingTime=	21.3	; aux.dueDate=	96	; aux.penaltyWeight=	0.1	; aux.tardiWeight=	0.9	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	11	; aux.releaseTime=	1	; aux.processingTime=	6.7	; aux.dueDate=	100	; aux.penaltyWeight=	0.5	; aux.tardiWeight=	0.2	; aux.earlyWeight=	1,0	; orderData.push_back(aux);
		aux.index=	12	; aux.releaseTime=	0	; aux.processingTime=	4.5	; aux.dueDate=	93	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.9	; orderData.push_back(aux);
	}

	if (instance == 3){
		aux.index=	1	; aux.releaseTime=	1	; aux.processingTime=	13.8	; aux.dueDate=	109	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	2	; aux.releaseTime=	1	; aux.processingTime=	8.1	; aux.dueDate=	115	; aux.penaltyWeight=	0.6	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.7	; orderData.push_back(aux);
		aux.index=	3	; aux.releaseTime=	2	; aux.processingTime=	10.7	; aux.dueDate=	123	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.3	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	4	; aux.releaseTime=	1	; aux.processingTime=	8,0	; aux.dueDate=	117	; aux.penaltyWeight=	0.6	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.9	; orderData.push_back(aux);
		aux.index=	5	; aux.releaseTime=	1	; aux.processingTime=	7.4	; aux.dueDate=	116	; aux.penaltyWeight=	0.7	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	6	; aux.releaseTime=	2	; aux.processingTime=	6.9	; aux.dueDate=	123	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.2	; aux.earlyWeight=	1,0	; orderData.push_back(aux);
		aux.index=	7	; aux.releaseTime=	0	; aux.processingTime=	5.1	; aux.dueDate=	112	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	8	; aux.releaseTime=	0	; aux.processingTime=	6.1	; aux.dueDate=	114	; aux.penaltyWeight=	0.2	; aux.tardiWeight=	0,0	; aux.earlyWeight=	0.7	; orderData.push_back(aux);
		aux.index=	9	; aux.releaseTime=	1	; aux.processingTime=	14.1	; aux.dueDate=	114	; aux.penaltyWeight=	0.2	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.7	; orderData.push_back(aux);
		aux.index=	10	; aux.releaseTime=	2	; aux.processingTime=	11.2	; aux.dueDate=	113	; aux.penaltyWeight=	0.9	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.1	; orderData.push_back(aux);
		aux.index=	11	; aux.releaseTime=	1	; aux.processingTime=	16.8	; aux.dueDate=	116	; aux.penaltyWeight=	0,0	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	12	; aux.releaseTime=	1	; aux.processingTime=	21.7	; aux.dueDate=	110	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	13	; aux.releaseTime=	2	; aux.processingTime=	9,0	; aux.dueDate=	116	; aux.penaltyWeight=	0.4	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.9	; orderData.push_back(aux);
		aux.index=	14	; aux.releaseTime=	0	; aux.processingTime=	15.6	; aux.dueDate=	123	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.1	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
	}

	if (instance == 4){
		aux.index=	1	; aux.releaseTime=	1	; aux.processingTime=	20.4	; aux.dueDate=	210	; aux.penaltyWeight=	0.7	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.7	; orderData.push_back(aux);
		aux.index=	2	; aux.releaseTime=	0	; aux.processingTime=	17.6	; aux.dueDate=	189	; aux.penaltyWeight=	0.7	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	3	; aux.releaseTime=	0	; aux.processingTime=	9.9	; aux.dueDate=	204	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.1	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	4	; aux.releaseTime=	2	; aux.processingTime=	18.4	; aux.dueDate=	210	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	1,0	; aux.earlyWeight=	0.8	; orderData.push_back(aux);
		aux.index=	5	; aux.releaseTime=	1	; aux.processingTime=	20.3	; aux.dueDate=	192	; aux.penaltyWeight=	0.9	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.1	; orderData.push_back(aux);
		aux.index=	6	; aux.releaseTime=	2	; aux.processingTime=	18.4	; aux.dueDate=	212	; aux.penaltyWeight=	0.7	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0,0	; orderData.push_back(aux);
		aux.index=	7	; aux.releaseTime=	0	; aux.processingTime=	7,0	; aux.dueDate=	209	; aux.penaltyWeight=	0.5	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.1	; orderData.push_back(aux);
		aux.index=	8	; aux.releaseTime=	1	; aux.processingTime=	15.6	; aux.dueDate=	198	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.5	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	9	; aux.releaseTime=	2	; aux.processingTime=	10.7	; aux.dueDate=	199	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.8	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	10	; aux.releaseTime=	1	; aux.processingTime=	8.2	; aux.dueDate=	207	; aux.penaltyWeight=	0.9	; aux.tardiWeight=	0.6	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	11	; aux.releaseTime=	1	; aux.processingTime=	19.5	; aux.dueDate=	208	; aux.penaltyWeight=	0.6	; aux.tardiWeight=	0.9	; aux.earlyWeight=	0.5	; orderData.push_back(aux);
		aux.index=	12	; aux.releaseTime=	1	; aux.processingTime=	19.6	; aux.dueDate=	196	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.3	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	13	; aux.releaseTime=	1	; aux.processingTime=	4,0	; aux.dueDate=	191	; aux.penaltyWeight=	0.7	; aux.tardiWeight=	0.8	; aux.earlyWeight=	0.3	; orderData.push_back(aux);
		aux.index=	14	; aux.releaseTime=	1	; aux.processingTime=	11.3	; aux.dueDate=	195	; aux.penaltyWeight=	0.8	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	15	; aux.releaseTime=	0	; aux.processingTime=	11.4	; aux.dueDate=	197	; aux.penaltyWeight=	0.3	; aux.tardiWeight=	0.7	; aux.earlyWeight=	0.3	; orderData.push_back(aux);
		aux.index=	16	; aux.releaseTime=	2	; aux.processingTime=	6.7	; aux.dueDate=	190	; aux.penaltyWeight=	0.6	; aux.tardiWeight=	1,0	; aux.earlyWeight=	0.2	; orderData.push_back(aux);
		aux.index=	17	; aux.releaseTime=	2	; aux.processingTime=	6.1	; aux.dueDate=	203	; aux.penaltyWeight=	1,0	; aux.tardiWeight=	0.6	; aux.earlyWeight=	0.6	; orderData.push_back(aux);
		aux.index=	18	; aux.releaseTime=	2	; aux.processingTime=	14.3	; aux.dueDate=	189	; aux.penaltyWeight=	0.2	; aux.tardiWeight=	0.9	; aux.earlyWeight=	0.1	; orderData.push_back(aux);
		aux.index=	19	; aux.releaseTime=	1	; aux.processingTime=	17.7	; aux.dueDate=	192	; aux.penaltyWeight=	0.2	; aux.tardiWeight=	0.4	; aux.earlyWeight=	0.4	; orderData.push_back(aux);
		aux.index=	20	; aux.releaseTime=	1	; aux.processingTime=	8.9	; aux.dueDate=	205	; aux.penaltyWeight=	0.6	; aux.tardiWeight=	0.2	; aux.earlyWeight=	0.1	; orderData.push_back(aux);
	}

	return orderData;

}

//Fun��o que l� o n�mero de jobs de cada inst�ncia
int readNumberOfJobs(int instance){
	int n;
	vector<int> amountOfJobs(5);
	amountOfJobs[1] = 10; amountOfJobs[2] = 12; amountOfJobs[3] = 14; amountOfJobs[4] = 20;
	n = amountOfJobs.at(instance);
	return n;
}

//Fun��o que calcula os par�metros necess�rios � heur�stica construtiva est�tica, retornando vetor com jobs 
vector<job> setParametersForModel3 (vector<job> availableJobs, int t){

	vector<job> modifiedList;	//vetor de jobs que ser�o manipulados pela heur�stica construtiva
	double slackTime=0;

	for (int j=0; j<availableJobs.size(); j++){
		
		//calcula a folga e pondera pelo peso de atraso ou adiantamento, conforme valor da folga
		slackTime = availableJobs.at(j).dueDate - t - availableJobs.at(j).processingTime;
		if (slackTime >=0) availableJobs.at(j).weightedSlackTime = slackTime*availableJobs.at(j).earlyWeight;
		else availableJobs.at(j).weightedSlackTime = slackTime*availableJobs.at(j).tardiWeight;
		
		//popula a lista de jobs
		modifiedList.push_back(availableJobs.at(j));

	}
	
	return modifiedList;

}

//Fun��o que compara a penalidade de atraso ou adiantamento, j� ponderados pelo peso certo, e ordena crescentemente
bool decreasingWeightedSlack(job lhs, job rhs){ 
	return lhs.weightedSlackTime > rhs.weightedSlackTime;
}

//Fun��o que encontra o m�nimo processingTime dos jobs de umas lista de jobs
double minDj (vector<job> jobs){				
	double minDj=10000;
	for(int j=0; j<jobs.size(); j++){
		minDj = jobs.at(j).dueDate < minDj ? jobs.at(j).dueDate : minDj;
	}
	return minDj;
}

//Fun��o que encontra o m�ximo processingTime dos jobs de uma lista de jobs
double maxDj (vector<job> jobs){				
	double maxDj=0;
	for(int j=0; j<jobs.size(); j++){
		maxDj = jobs.at(j).dueDate > maxDj ? jobs.at(j).dueDate : maxDj;
	}
	return maxDj;
}

//Fun��o que encontra a soma dos tempos de processamento dos jobs de uma lista de jobs
double sumOfPj (vector<job> jobs){				
	double sumPj=0;
	for(int j=0; j<jobs.size(); j++){
		sumPj += jobs.at(j).processingTime;
	}
	return sumPj;
}


//Fun��o que calcula e preenche as vari�veis de sa�da de uma lista de jobs (vetor de jobs), dada uma sequ�ncia de processamento (vetor com index dos jobs na ordem de processamento)
vector<job> calculateJobOutputVariables (vector<job> jobs, vector<int> sequence){

	double instant = 0;															//vari�vel auxiliar para c�lculo do tempo total para processar os jobs na ordem dada	
	double rj, pj;																//vari�veis auxiliares que armazenam o release time e o processing time de cada job j

	for (int position=0; position<sequence.size(); position++){					//percorre a sequencia de jobs calculando o tempo percorrido at� o fim do processamento do job de cada posi��o

		rj = jobs.at(sequence.at(position)-1).releaseTime;						//busca o releaseTime do job e joga em rj
		pj = jobs.at(sequence.at(position)-1).processingTime;					//busca o processingTime do job e joga em pj
						
		instant = instant < rj ? rj : instant;											//atualiza o instante para s� iniciar o processamento quando o instante for maior ou igual a rj
					
		jobs.at(sequence.at(position)-1).sj = instant;							//grava o instante de in�cio de processamento de j na posi��o (j-1) do vetor startTime
		jobs.at(sequence.at(position)-1).cj = instant + pj;						//grava o instante de fim de processamento de j na posi��o (j-1) do vetor endTime
					
		instant += pj;															//atualiza o instante para pr�xima itera��o, contando o tempo de processamento do job

	}					

	//Calcula earliness, lateness, tardiness e unit penalty, jogando no vetor de jobs
	for (int j=0; j<jobs.size(); j++){
					
		//lateness � a diferen�a entre o fim do processamento e a data de entrega do job
		jobs.at(j).lj = jobs.at(j).cj - jobs.at(j).dueDate;

		//se lateness for positivo, o job teve atraso, portanto Uj=1, Tj=Lj e Ej=0 
		if (jobs.at(j).lj>0){
				jobs.at(j).uj = 1;
				jobs.at(j).tj = jobs.at(j).lj;
				jobs.at(j).ej = 0;
		}

		//caso contr�rio, Uj=0, Tj=0 e Ej= - Lj 
		else{
				jobs.at(j).uj = 0;
				jobs.at(j).tj = 0;
				jobs.at(j).ej = -1*(jobs.at(j).lj);
		}
	
	}

	return jobs;

}

//Fun��o que retorna um vetor de objetivos, j� com c�digo e nome (f�rmula)
vector<objectiveFunction> setObjectives(){
	
	vector<objectiveFunction> objectiveFunction(11);
	
	for(int n=1; n<=11; n++){
		objectiveFunction.at(n-1).code=n;
	}
	
	objectiveFunction.at(0).formula = "Cmax";
	objectiveFunction.at(1).formula="Sum Cj";
	objectiveFunction.at(2).formula="sum WjCj";
	objectiveFunction.at(3).formula="sum Wj(Cj-Rj)";
	objectiveFunction.at(4).formula="sum Uj";
	objectiveFunction.at(5).formula="sum WjUj";
	objectiveFunction.at(6).formula="sum Tj";
	objectiveFunction.at(7).formula="sum WTjTj";
	objectiveFunction.at(8).formula="Tmax";
	objectiveFunction.at(9).formula="Lmax";
	objectiveFunction.at(10).formula="sum WTjTj+WEjEj";
	
	for(int n=1; n<=11; n++){
		objectiveFunction.at(n-1).value=0;
	}

	return objectiveFunction;

}

//Fun��o que calcula os objetivos usando um vetor de jobs j� com instantes de in�cio e fim calculados e joga em vetor de objectiveFunction
vector<objectiveFunction> calculateObjectives(vector<job> jobVector){
	
	//inicializa vetor com fun��es objetivo
	vector<objectiveFunction> objectivesVector(11);
	
	//grava o c�digo de cada fun��o objetivo armazenada no vetor
	for(int n=1; n<=11; n++){
		objectivesVector.at(n-1).code=n;
	}
	
	//grava descri��o de cada fun��o objetivo armazenada no vetor
	objectivesVector.at(0).formula ="Cmax";
	objectivesVector.at(1).formula="Sum Cj";
	objectivesVector.at(2).formula="sum WjCj";
	objectivesVector.at(3).formula="sum Wj(Cj-Rj)";
	objectivesVector.at(4).formula="sum Uj";
	objectivesVector.at(5).formula="sum WjUj";
	objectivesVector.at(6).formula="sum Tj";
	objectivesVector.at(7).formula="sum WTjTj";
	objectivesVector.at(8).formula="Tmax";
	objectivesVector.at(9).formula="Lmax";
	objectivesVector.at(10).formula="sum WTjTj+WEjEj";
	
	//zera valores iniciais
	for(int n=1; n<=11; n++){
		objectivesVector.at(n-1).value=0;
	}

	//popula o vetor com os valores de cada fun��o objetivo inicializada, para o jobVector inputado
	for (int j=0; j<jobVector.size(); j++){
		objectivesVector.at(0).value = objectivesVector.at(0).value > jobVector.at(j).cj ? objectivesVector.at(0).value : jobVector.at(j).cj; 
		objectivesVector.at(1).value += jobVector.at(j).cj;
		objectivesVector.at(2).value += jobVector.at(j).cj*jobVector.at(j).penaltyWeight;
		objectivesVector.at(3).value += (jobVector.at(j).cj-jobVector.at(j).releaseTime)*jobVector.at(j).penaltyWeight;
		objectivesVector.at(4).value += jobVector.at(j).uj;
		objectivesVector.at(5).value += jobVector.at(j).uj*jobVector.at(j).penaltyWeight;
		objectivesVector.at(6).value += jobVector.at(j).tj;
		objectivesVector.at(7).value += jobVector.at(j).tj*jobVector.at(j).tardiWeight;
		if (jobVector.at(j).tj>objectivesVector.at(8).value) objectivesVector.at(8).value = jobVector.at(j).tj;
		if (jobVector.at(j).lj> objectivesVector.at(9).value)  objectivesVector.at(9).value = jobVector.at(j).lj;
		if (-jobVector.at(j).lj> objectivesVector.at(9).value) objectivesVector.at(9).value = -jobVector.at(j).lj;
		objectivesVector.at(10).value += jobVector.at(j).tj*jobVector.at(j).tardiWeight + jobVector.at(j).ej*jobVector.at(j).earlyWeight;
	}
	
	return objectivesVector;

}

//Fun��o que compara valor da fun��o objetivo de duas solu��es pertecentes a um vetor de solu��es e retorna o menor deles (p/ ordenar crescentemente as solu��es em termos de valor na FO)
bool increasingOFValue(solution lhs, solution rhs){ 
	return lhs.objValue < rhs.objValue; 
}

//Fun��o que calcula vizinhan�a de uma solu��o utilizando o movimento de troca de pares adjacentes. Armazena todas as vizinhas j� com o movimento associado � sua constru��o a partir da solu��o dada.
vector<solution> getNeighborhood(solution currentSolution){
	
	vector<solution> neighborhood;	//vetor de solu��es encontradas a partir da currentSoution
	
	for(int position = 1; position<currentSolution.sequence.size(); position++){
		
		//inicializa candidata encontrada a partir de modifica��o realizada na solu��o original
		solution candidate = currentSolution;	

		//realiza um movimento e guarda nova sequ�ncia na solu��o candidata;
		candidate.sequence.at(position-1) = currentSolution.sequence.at(position);
		candidate.sequence.at(position) = currentSolution.sequence.at(position-1);

		//armazena movimento utilizado na modifica��o da solu��o corrente para a solu��o candidata;
		candidate.moveTrack.node1 = candidate.sequence.at(position-1); 
		candidate.moveTrack.node2 = candidate.sequence.at(position); //**************removi indez dessas duas linhas

		//joga solu��o vizinha no vetor de vizinhan�a
		neighborhood.push_back(candidate);

	}

	return neighborhood;


}

//Fun��o que recebe um movimento e inverte seu atributo, retornando o movimento assim obtido
movement invertMoveAttribute( movement original){

	movement modified;

	modified.node1 = original.node2;
	modified.node2 = original.node1;

	return modified;

}

//Fun��o que compara movimento com lista de movimentos (armazenados em um vetor) e retorna: 1 se o movimento for encontrado na lista, 0 caso contr�rio
int checkTabuList( vector<movement> list, movement move){
	
	int bin = 0;						//resultado da compara��o (bin�rio)

	for(int m=0; m<list.size(); m++){
		
		if(list.at(m).node1 == move.node1 && list.at(m).node2 == move.node2){
			
			bin = 1;
			break;

		}

	}

	return bin;

}

//Struct que liga c�digo a descri��o
struct codeDescr{
	string description;
	int code;
};

//Fun��o que retorna o c�digo e a descri��o de cada modelo e um vetor
vector<codeDescr> modelTypes(){
	vector<codeDescr> calcModel(5);
	for(int n=1; n<=5; n++){
		calcModel.at(n-1).code=n;
	}
	calcModel.at(0).description="Seq Imposta";
	calcModel.at(1).description="Aleatorio";
	calcModel.at(2).description="Regra liberacao";
	calcModel.at(3).description="Tabu Search";
	calcModel.at(4).description="Otimizacao - Gurobi";

	return calcModel;
}





//Rotina principal
int main(int   argc,     char *argv[]){


		/*********************************************Data structure**************************************************************/

		//Vari�veis de c�digo do modelo e inst�ncia do problema
		int modelCodeInput = 0;						//C�digo do modelo - 1 Seq Imposta; 2 Aleat�rio; 3 Regra libera��o; 4 Tabu Search; 5 Otimiza��o
		int firstInstance=0;
		int lastInstance=0;

		//Vari�vel que determina se o c�digo deve ser executado de novo, de acordo com a vontade do usu�rio
		int doItAgain = 1;

		//Estrutura de dados para armazenar inputs do usu�rio
		vector < vector<int> > tabuSearchMainInputsOfInstance;						//vetor que armazena CriterioParGlobal;CriterioParLocal;TamanhoListaTabu
		vector <int> randomSearchInputOfInstance;									//vetor que armazena o n�mero de itera��es solicitadas pelo usu�rio na busca aleat�ria
		vector<string> objectiveOfChoiceOfInstance;									//vetor com a descri��o do objetivo escolhido pelo usu�rio				
		

		//estruturas de dados que ser�o carregadas com os resultados de cada inst�ncia
		vector< vector<int> > sequenceOfInstance;									//vetor com ordem de processamento dos jobs, para cada intancia rodada
		vector< vector<objectiveFunction> > objectivesOfInstance;					//valores dos objetivos de cada instancia e tipo
		vector<int> timeForFindingSolutionOfInstance;								//vetor com tempo decorrido entre in�cio e fim da solu��o de cada int�ncia
		vector< vector< job > > jobOutputOfInstance;								//vetor com o conjunto de jobs e seus dados de sa�da associados, para cada int�ncia (acesso primeiro int�ncia, depois job j e depois o elemento que quero visualizar).
		vector< vector< int > > initialSequenceOfInstance;


		do{
			//Introdu��o do programa e carregamento dos nomes dos modelos e dos objetivos
			cout << "\nSOFTWARE DE PROGRAMACAO DE MAQUINA UNICA\nTrabalho com 4 instancias de 10, 12, 14 e 20 jobs\n\n";
				cout<< "De acordo com os codigos de modelo a seguir:\n\n   1-Seq Imposta\n   2-Busca Aleatoria\n   3-Heuristica Construtiva\n"
				   << "   4-Heuristica Busca - Tabu Search\n   5-Otimizacao - Gurobi\n";

			//Prompt de comando: pede modelo de c�lculo
			cout << "\nDigite o codigo do modelo que deseja rodar: ";
			cin >> modelCodeInput;
			cout << "\nDigite o intervalo de instancias que deseja rodar (por ex. '1 3' roda instancia 1 ate a 3): ";
			cin >> firstInstance;
			cin >> lastInstance;


			/************************************************************Calcs****************************************************************/

			//Para cada inst�ncia e modelo selecionado, faz os c�lculos 
			for (int instanceNumber = firstInstance; instanceNumber<=lastInstance; instanceNumber++){
			
				cout << "\n\nINSTANCIA " << instanceNumber << endl << endl;
			
				//L� os dados da inst�ncia
				int numberOfJobs = 0;								//vari�vel que armazena o n�mero de jobs da inst�ncia
				vector< job > jobList;								//lista de jobs (objetos tipo struct) com atributos associados a cada job j
				jobList = readInstanceData(instanceNumber);			//chama fun��o que l� os dados da inst�ncia e retorna vetor de jobs
				numberOfJobs = readNumberOfJobs(instanceNumber);	//chama fun��o que retorna o n�mero de dados da inst�ncia

				//Cria vari�veis auxiliares para c�lculo das vari�veis de sa�da
				int final, inicio;		//auxiliares para c�lculo de tempo de processamento do modelo, em cada inst�ncia				
			

				//estruturas de dados que ser�o utilizadas a cada execu��o do modelo selecionado, ou seja, em cada inst�ncia calculada
				vector<int> jobOrderOutput;							//vetor com a ordem de processamento dos jobs (index de cada job)
				vector<objectiveFunction> objectiveValue;			//vetor com o valor da fun��o objetivo, para cada objetivo 1 a 11 (N�O pula posi��o 0)
			
			
						
								

				//Sequ�ncia imposta
				if (modelCodeInput == 1){	
				
					//Prompt para entrada de sequ�ncia imposta
					cout << "Digite a sequencia de processamento dos jobs de 1 a " << numberOfJobs << ": ";
				
					//L� a sequ�ncia digitada e armazena no vetor "jobOrderOutput"
					int jobIndex=0;
					for (int j=0; j<numberOfJobs; j++){
						cin >> jobIndex;
						if (jobIndex>0 && jobIndex<=numberOfJobs) jobOrderOutput.push_back(jobIndex);
						else { 
							jobOrderOutput.clear();
							cout << "\nSequencia invalida! Por favor digite novamente a sequencia: ";
							j=-1;
						}
					}

					//Pega instante de in�cio da solu��o
					inicio = GetTickCount();

					//Chama a fun��o que calcula o in�cio e o fim de processamento de cada job, dado um vetor jobOrderOutput, e joga no vetor de jobs (jobList)
					jobList = calculateJobOutputVariables(jobList, jobOrderOutput);
					
					//Calcula e armazena os objetivos no vetor de fun��es objetivo
					objectiveValue = calculateObjectives(jobList);
					
					//Pega instante de fim da solu��o
					final = GetTickCount();
					
					//imprime objetivos
					for(int obj=0; obj<objectiveValue.size(); obj++){
						cout << "\nCodObj " << setw(2) << objectiveValue.at(obj).code << ": "<< objectiveValue.at(obj).formula << "=" << objectiveValue.at(obj).value;
					}
					cout << "\n\n";
					

					//armazena os outputs nos vetores de resultados
					objectivesOfInstance.push_back(objectiveValue);
					sequenceOfInstance.push_back(jobOrderOutput);
					timeForFindingSolutionOfInstance.push_back(final - inicio);
					jobOutputOfInstance.push_back(jobList);
					objectiveOfChoiceOfInstance.push_back("NA");

					//Zera os vetores de entrada e sa�da, para pr�xima inst�ncia
					jobOrderOutput.clear();
					objectiveValue.clear();
			
				
				}//fim do modelCode 1	
				


				//Busca aleat�ria
				if(modelCodeInput==2){
				
					cout << "\nModelo de calculo " << modelCodeInput << endl << endl;

					//vari�veis de input do usu�rio (n�mero de itera��es e fun��o objetivo)
					int finalIterationInput;		//n�mero de itera��es (buscas) que o usu�rio pede para realizar
					int objFuncInput;				//c�digo do objetivo que o usu�rio quer 'otimizar'
				
					//estrutura de dados de sa�da
					vector<objectiveFunction> tempObjectives = setObjectives();		//vetor com a descri��o dos objetivos, apenas para impressao na tela
					vector< vector<job> > jobOutputOfIteration;						//vetor com a lista de jobs e seus par�metros associados, de cada itera��o (acessa primeiro itera��o, depois job, depois propriedade)
					vector< vector<objectiveFunction> > objectivesOfIteration;		//vetor com os objetivos calculados para cada sequencia sorteada (acessa primeiro it, depois objetivo)
					vector< vector< int > > jobSequenceOfIteration;					//vetor com a sequ�ncia de processamento dos jobs de cada itera��o (acessa primeiro it depois position na sequencia)

					//prompt de comando para usu�rio inserir o n�mero de itera��es e a fun��o objetivo que deseja utilizar
					cout << "Digite o numero de iteracoes que deseja utilizar na busca: ";
					cin >> finalIterationInput;
					cout << "\nDados as funcoes objetivo listadas abaixo, ao lado de seu respectivo codigo:\n\n";
						for(int fo=0; fo<tempObjectives.size(); fo++){
							cout << "	" << tempObjectives.at(fo).code << " - ";
							cout << tempObjectives.at(fo).formula << endl;
						}
					cout << "\n\nDigite o codigo da FO que deseja considerar: ";
					cin >> objFuncInput;

					//Pega instante de in�cio da solu��o
					inicio = GetTickCount();			
					

					//executa o procedimento de busca tantas vezes quanto o usu�rio solicitou
					for(int it=0; it<finalIterationInput; it++){
					
						//Vetor que armazena jobs a ordenar, a cada itera��o
						vector<int> jobsNotPicked;
						for(int j=0; j<numberOfJobs; j++){
							jobsNotPicked.push_back(jobList.at(j).index);
						}
					
						//Rotina que sorteia um a um o pr�ximo job da lista, colocando na lista temporaria e removendo o escolhido do vetor jobsNotPicked
						vector<int> tempSeq;
						for(int n=numberOfJobs; n>0; n--){
												
							int randPosition;
							srand(GetTickCount());		//inicializa semente do gerador de n�mero pseudoaleat�rio
							randPosition = rand() % n;	//randPosition � um inteiro entre 0 e (n-1)
						
							//pega job que est� na posi��o sorteada e coloca na pr�xima posi��o vazia da sequ�ncia tempor�ria (seq da itera��o corrente)
							tempSeq.push_back(jobsNotPicked.at(randPosition));

							//atualiza vetor de jobs nao selecionados ainda
							for(int position=randPosition; position<n-1; position++){
								jobsNotPicked.at(position) = jobsNotPicked.at(position+1);
							}
							jobsNotPicked.pop_back();

						}
						//tempSeq.push_back(jobsNotPicked.at(0));
						jobsNotPicked.clear();

						//Imprime a sequ�ncia sorteada
						cout << "A sequencia de jobs sorteada na iteracao " << it+1 << " foi: ";
						for (int j=0; j<numberOfJobs; j++){
							cout << tempSeq.at(j) << " ";
						}cout << endl << endl;


						//Chama a fun��o que calcula o in�cio e o fim de processamento de cada job, dada a sequ�ncia de processamento tempSeq
						jobList = calculateJobOutputVariables(jobList, tempSeq);
												

						//Armazena resultados da itera��o nos vetores apropriados
						jobOutputOfIteration.push_back(jobList);						//Joga o jobList no vetor que armazena o jobList de cada itera��o
						objectivesOfIteration.push_back(calculateObjectives(jobList));	//Calcula e armazena os objetivos no vetor que armazena os objetivos de cada itera��o
						jobSequenceOfIteration.push_back(tempSeq);


					}//Fim das execu��es do procedimento de busca aleat�ria


					//Rotina que salva em um arquivo todas as sequ�ncias sorteadas pra essa inst�ncia
					ofstream myfile;
				
					ostringstream outputname;
					outputname << "resultados_por_iteracao-modelo_2-instancia_" << instanceNumber << ".csv";
				
					//myfile.open("resultados por iteracao - modelo 2.csv");
					myfile.open(outputname.str());
				
					myfile << "Inst; It; Seq; ObjCode; ObjVal; ModDescr\n";
					
					for(int it=0; it<finalIterationInput; it++){
						for(int obj=0; obj<11; obj++){
							myfile << instanceNumber << ";" << it+1 << ";";
								for(int j=0; j<numberOfJobs-1; j++){
									myfile << jobSequenceOfIteration.at(it).at(j) << "-";
								} myfile << jobSequenceOfIteration.at(it).at(numberOfJobs-1)<< ";";
							myfile << obj+1 << ";" << objectivesOfIteration.at(it).at(obj).value << ";";
							myfile << objFuncInput << ";\n" ;
						}		
					}
					myfile.close();

				

					//Rotina para encontrar o melhor resultado gerado e armazenar a solu��o
					double minObj = 100000;
					int bestIteration = 0;
					
					//Acha melhor itera��o, de acordo com fun��o objectivo escolhido pelo usu�rio
					for(int i=0; i<finalIterationInput; i++){

						if (objectivesOfIteration.at(i).at(objFuncInput-1).value < minObj){

							bestIteration = i+1;									 //joga o n�mero da itera��o em bestIteration
							minObj= objectivesOfIteration.at(i).at(objFuncInput-1).value;  //joga o valor da f.o. escolhida no m�nimo atual
							
						}

					}

					//Armazena valores nos vetores de objetivos, sequ�ncia de jobs e lista de jobs
					objectiveValue = objectivesOfIteration.at(bestIteration-1);			//guarda valores das fos da melhor solu��o encontrada na busca
					jobOrderOutput = jobSequenceOfIteration.at(bestIteration-1);		//guarda sequ�ncia de jobs da melhor solu��o encontrada
					jobList = jobOutputOfIteration.at(bestIteration-1);					//guarda a jobList da melhor solu��o encontrada


					//Pega instante de fim da solu��o
					final = GetTickCount();


					//Imprime melhor solu��o encontrada
					cout << "\n\nA melhor sequencia de jobs sorteada foi: ";
					for (int j=0; j<numberOfJobs; j++){
						cout << jobSequenceOfIteration.at(bestIteration-1).at(j) << " ";
					}cout << "\n -obtida na iteracao "<< bestIteration << endl << endl;

					
					//imprime objetivos
					for(int obj=0; obj<11; obj++){
						cout << "CodObj " << setw(2) << objectiveValue.at(obj).code << objectiveValue.at(obj).formula << "=" << setw(3) << objectiveValue.at(obj).value << endl;
					} cout <<endl;


					//armazena resultados nas estruturas de dados das inst�ncias
					randomSearchInputOfInstance.push_back(finalIterationInput);
					objectivesOfInstance.push_back(objectiveValue);
					sequenceOfInstance.push_back(jobOrderOutput);
					timeForFindingSolutionOfInstance.push_back(final - inicio);
					jobOutputOfInstance.push_back(jobList);
					objectiveOfChoiceOfInstance.push_back(tempObjectives.at(objFuncInput-1).formula);


					//Zera os vetores de entrada e sa�da, para uso na pr�xima inst�ncia
					jobOrderOutput.clear();
					objectiveValue.clear();


				}//fim do modelCode 2




				//Heur�stica construtiva - est�tica
				if(modelCodeInput == 3){

					cout << "Modelo de calculo " << modelCodeInput << endl << endl;
					int visualizationBin;


					cout << "\n\nDeseja visualizar solucoes a cada etapa? Digite 1 caso positivo, qualquer outro numero caso negativo: ";
					cin >> visualizationBin;

					//Pega instante de in�cio da solu��o
					inicio = GetTickCount();
					
					//Inicializa vari�veis e listas de jobs
					int t=0;										//Inicializa o contador do tempo
					vector<job> allocatedJobs;						//Ja - conjunto de jobs j� alocados
					vector<job> orderedAvailableJobs = jobList;		//Jn - cria vetor para armazenar a lista de jobs ainda n�o selecionados, ordenados pelo desvio ponderado
					
					
					do{
						
						//calcula e ordena crescentemente as folgas ponderadas dos jobs no vetor orderedAvailableJobs
						orderedAvailableJobs = setParametersForModel3(orderedAvailableJobs,t);							//joga uma c�pia da jobList nesse vetor, com os par�metros necess�rios p/ heur�stica
						std::sort(orderedAvailableJobs.begin(), orderedAvailableJobs.end(), decreasingWeightedSlack);	//ordena o vetor orderedJobList decrescentemente em penalidade associado � sua folga
						
						//pega primeiro job de Jn e p�e em Ja
						allocatedJobs.push_back(orderedAvailableJobs.back());
						orderedAvailableJobs.pop_back();
						
						//calcula a soma dos tempos de processamento dos jobs j� alocados
						t = sumOfPj(allocatedJobs);	


						if (visualizationBin==1){
							//Imprime sequ�ncia encontrada pelo algoritmo, para conferir na tela
							cout << "\n\nA sequencia de jobs construida ate esta etapa eh: ";
							for (int j=0; j<allocatedJobs.size(); j++){
								cout << allocatedJobs.at(j).index << " ";
							}cout << "\n\n";	
							system("pause");
						}


					}while(orderedAvailableJobs.size()>1);

					allocatedJobs.push_back(orderedAvailableJobs.back());

					
						


					//Popula o vetor de jobOrderOutput, colocando o �ndice dos jobs na mesma ordem em que eles aparecem no vetor startedEarly
					for (int j=0; j<numberOfJobs; j++){
						jobOrderOutput.push_back(allocatedJobs.at(j).index);
					}
					


					//Imprime sequ�ncia encontrada pelo algoritmo, para conferir na tela
					cout << "\n\nA sequencia de jobs construida pelo algoritmo foi: ";
					for (int j=0; j<numberOfJobs; j++){
						cout << jobOrderOutput.at(j) << " ";
					}cout << "\n\n";
					

					//Calcula e armazena os resultados na pr�rpia jobList, usando sequ�ncia de processamento armazenada em jobOrderOutput
					jobList = calculateJobOutputVariables(jobList, jobOrderOutput);
					

					//Calcula e armazena os objetivos
					objectiveValue = calculateObjectives(jobList);


					//Pega instante de fim da solu��o
					final = GetTickCount();


					//imprime objetivos
					for(int obj=0; obj<11; obj++){
						cout << "CodObj " << setw(2) << objectiveValue.at(obj).code << ":" << objectiveValue.at(obj).formula << "=" << setw(3) << objectiveValue.at(obj).value << endl;
					} cout <<endl;


					//armazena o vetor de objetivos no vetor objectiveOfInstance
					objectivesOfInstance.push_back(objectiveValue);
					sequenceOfInstance.push_back(jobOrderOutput);
					timeForFindingSolutionOfInstance.push_back(final - inicio);
					jobOutputOfInstance.push_back(jobList);
					objectiveOfChoiceOfInstance.push_back(objectiveValue.at(10).formula);


					//Zera os vetores de entrada e sa�da, para uso do pr�ximo m�todo
					jobOrderOutput.clear();
					objectiveValue.clear();


				}//fim do modelCode 3
			
				


				
				//Heur�stica de Busca: Tabu Search
				if (modelCodeInput==4){


					
					//*****Interface com usu�rio, que define todos os par�metros adotados na Busca Tabu

					cout << "Modelo de calculo " << modelCodeInput << endl << endl;

					//inicializa e define o objetivo da busca tabu
					int objectiveCode;
					vector<objectiveFunction> tempObjectives = setObjectives();

					//inicializa crit�rios de parada e de aspira��o, necess�rios � Busca Tabu
					int localStopCriterium;			//n�mero m�ximo de itera��es da busca local
					int globalStopCriterium;			//n�mero m�ximo de itera��es da busca global
					int localTabuListSize;			//n�mero de movimentos armazenados na mem�ria curta, para busca local
					double aspirationCriterium = 1.0;	//crit�rio de aspira��o para aceitar movimento Tabu
					int visualizationBin;
					vector<int> initialSequence; //sequ�ncia inicial, inserida pelo usu�rio


					//prompt para definicao do objetivo usado na busca tabu
					cout << "Dados as funcoes objetivo listadas abaixo, ao lado de seu respectivo codigo:\n\n";
					for(int fo=0; fo<11; fo++){
						cout << "	" << tempObjectives.at(fo).code << " - ";
						cout << tempObjectives.at(fo).formula << endl;
					}
					cout << "\n\nDigite o codigo da FO que deseja considerar: ";
					cin >> objectiveCode;			

					//prompt para definicao dos parametros adotados na busca tabu:
					cout << "\nDigite o criterio de parada para a busca local (numero de iteracoes): ";
					cin >> localStopCriterium;
					cout << "\nDigite o criterio de parada para a busca global (numero de iteracoes): ";
					cin >> globalStopCriterium;
					cout << "\nDigite o tamanho da Lista Tabu: ";
					cin >> localTabuListSize;
					cout << "\nCriterio de aspiracao:   solucao tabu  <  " << aspirationCriterium << " * melhor solucao";
					cout << "\n\nDeseja visualizar solucoes a cada etapa? Digite 1 caso positivo, qualquer outro numero caso negativo: ";
					cin >> visualizationBin;
					
					//guarda inputs do usu�rio na ordem: CriterioParGlobal;CriterioParLocal;TamanhoListaTabu
					vector<int> tabuSearchMainInputs;
					tabuSearchMainInputs.push_back(globalStopCriterium); 
					tabuSearchMainInputs.push_back(localStopCriterium);
					tabuSearchMainInputs.push_back(localTabuListSize);

					//****Estrutura de dados para uso da estrat�gia global

					//vari�veis armazenadas na longa mem�ria, para uso do procedimento global
					solution bestSolution;						//melhor solu��o encontrada at� o momento
					vector<movement> globalTabuList;			//Lista tabu de longa mem�ria
						



					//****Inicializa procedimento da heur�stica:

					//Popula o bestSolution com uma solu��o inicial definida pelo usu�rio
					cout << "\n\nSolucao inicial-> Digite a sequencia de processamento dos jobs de 1 a " << numberOfJobs << ": ";

					//L� a sequ�ncia digitada e armazena no vetor "jobOrderOutput"
					int jobIndex=0;
					for (int j=0; j<numberOfJobs; j++){
						cin >> jobIndex;
						if (jobIndex>0 && jobIndex<=numberOfJobs) jobOrderOutput.push_back(jobIndex);
						else { 
							jobOrderOutput.clear();
							cout << "\nSequencia invalida! Por favor digite novamente a sequencia: ";
							j=-1;
						}
					}

					initialSequence=jobOrderOutput;

					//Pega instante de in�cio da solu��o
					inicio = GetTickCount();

					//salva a solu��o inicial como melhor solucao encontrada ate o momento e cria um movimento "dumb" para esta solu��o
					bestSolution.sequence = jobOrderOutput; 
					bestSolution.objValue = calculateObjectives(calculateJobOutputVariables(jobList,bestSolution.sequence)).at(objectiveCode-1).value;
					bestSolution.objFormula = tempObjectives.at(objectiveCode-1).formula;
					bestSolution.moveTrack.node1 = 0;
					bestSolution.moveTrack.node2 = 0;



					//****Procedimento de busca global:

					//executa a busca local tantas vezes quanto for especificado pelo usu�rio, no globalStopCriterium
					for(int globalIt=1; globalIt<=globalStopCriterium; globalIt++){

						//Inicializa armazenamento de mem�ria para busca local
						solution currentSolution = bestSolution;	//solu��o corrente, para busca local			
						vector<solution> solutionPath;				//lista de solu��es percorridas na busca local
						vector<movement> localTabuList;				//Inicializa lista Tabu utilizada na busca local, de mem�ria curta
						
						//Adiciona solu��o ao vetor solutionPath e redefine a posi��o da melhor encontrada como igual � posi��o 0
						solutionPath.push_back(currentSolution);	//coloca a solu��o corrente (melhor encontrada at� o momento) na primeira posi��o do vetor solutionPath
						int bestSolutionPosition = 0;				//vari�vel aux que armazena posi��o da melhor solu��o no vetor solutionPath
						
						//Joga os elementos da lista global (se houver algum atributo j� armazenado) na lista Tabu local
						for(int aux=0; aux<globalTabuList.size();aux++){
							localTabuList.push_back(globalTabuList.at(aux));
						}

						cout << "\n\n\nIteracao global numero " << globalIt <<
							"\n\n   Melhor solucao encontrada tem valor de " << tempObjectives.at(objectiveCode-1).formula << "=" << bestSolution.objValue <<
							"\n    e sequencia de processamento: ";
						for(int j=0; j<bestSolution.sequence.size(); j++){
							cout << bestSolution.sequence.at(j); 
							if(j+1 == bestSolution.sequence.size()) cout << "\n";
							else cout << "-";
						}





						//***Procedimento de busca local:

						//executa tantas mudan�as na solu��o corrente quanto especificado pelo crit�rio de parada da busca local
						for(int localIt=1; localIt<=localStopCriterium; localIt++){
						


							//**Monta vizinhan�a usando o movimento de troca de pares adjacentes:

							//calcula a vizinhan�a da solu��o corrente e armazena no vetor neighborhood
							vector<solution> neighborhood = getNeighborhood(currentSolution);		
							
							//calcula o valor da FO de cada solu��o vizinha � solu��o corrente
							for (int k=0; k<neighborhood.size(); k++){								
								neighborhood.at(k).objValue = calculateObjectives(calculateJobOutputVariables(jobList,neighborhood.at(k).sequence)).at(objectiveCode-1).value; 
							}

							//ordena a vizinhan�a crescentemente na FO
							sort(neighborhood.begin(),neighborhood.end(),increasingOFValue);		

							

							//**Seleciona pr�xima solu��o corrente a partir das candidatas vizinhas:
														
							//percorre uma a uma as solu��es vizinhas at� selecionar a pr�xima solu��o corrente, a partir da vizinha de menor valor na FO
							for(int k=0; k<neighborhood.size(); k++){
							
								//verifica se o movimento associado � solu��o candidata � Tabu...
								if (checkTabuList(localTabuList,neighborhood.at(k).moveTrack)==1){
									
									//...se movimento for tabu, verifica se o crit�rio de aspira��o � atendido
									if(neighborhood.at(k).objValue<aspirationCriterium*bestSolution.objValue){

										//se crit�rio de aspira��o � atendido, a candidata vira solu��o corrente e n�o � preciso examinar outra solu��o da vizinhan�a
										currentSolution = neighborhood.at(k);

										//avisa que o movimento tabu foi executado e sai do loop de sele��o da pr�xima solu��o corrente
										cout << "\n     Movimento Tabu! Criterio de aspiracao atendido, pois " << neighborhood.at(k).objValue << "<" << aspirationCriterium*bestSolution.objValue << endl;								
										break;

									}

									//se o movimento � tabu mas crit�rio de aspira��o n�o � atendido, imprime que o movimento tabu nao foi aceito e continua no loop de sele��o da pr�xima solu��o corrente
									cout << "\n     Candidata seguinte, de movimento " << 
										neighborhood.at(k).moveTrack.node1 << " troca com " << neighborhood.at(k).moveTrack.node2 << 
										" eh tabu, mas nao atende criterio de aspiracao\n";

								}

								//...se movimento da candidata n�o for Tabu, a candidata vira solu��o corrente e n�o � preciso examinar outra solu��o vizinha
								else{
									currentSolution = neighborhood.at(k);
									break;
								} 

								//Se nenhuma solu��o da vizinhan�a satisfez os crit�rios acima (ou seja, todas s�o tabu e n�o satisfazem o crit�rio de aspira��o), seleciona a candidata de menor valor na FO
								if(k+1==neighborhood.size()){
									currentSolution = neighborhood.at(0);
									cout << "\n     Todas as solucoes vizinhas sao Tabu e nenhuma satisfaz o criterio de aspiracao :(\n";
								}

														
							}//fim do procedimento de defini��o da solu��o corrente entre as vizinhas candidatas

							

							//**Adiciona solu��o corrente (selecionada a partir das vizinhas) ao vetor de solu��es percorridas:

							solutionPath.push_back(currentSolution);
						
														

							//**Adiciona na lista tabu (local) o movimento inverso da obten��o da solu��o corrente:

							//se o tamanho da lista Tabu for menor que o tamanho m�ximo adotado, simplesmente adiciona o movimento inverso � obten��o da solu��o corrente na lista Tabu
							if(localTabuList.size()<localTabuListSize){
								localTabuList.push_back(invertMoveAttribute(currentSolution.moveTrack));
							}

							//se a lista tabu j� estiver cheia, apaga o movimento mais antigo da lista e adiciona o novo movimento (invertido)
							else{
								for(int aux=0; aux<localTabuListSize-1; aux++){
									localTabuList.at(aux) = localTabuList.at(aux+1);
								}
								localTabuList.pop_back();
								localTabuList.push_back(invertMoveAttribute(currentSolution.moveTrack));
							}


							
							//**Verifica se a solu��o corrente � a melhor encontrada nessa busca local:
							
							if(currentSolution.objValue<bestSolution.objValue){
									
								//se a solu��o corrente for melhor, atualiza melhor solu��o e guarda sua posi��o no vetor de solu��es percorridas pela busca local (solutionPath)
								bestSolution = currentSolution;
								bestSolutionPosition = localIt;

							}



							//**Imprime na tela os resultados desta itera��o da busca local:

							cout << "\n\n      Busca local numero " << localIt << ":\n" <<
								"         Solucao corrente eh ";
							for(int j=0; j<numberOfJobs; j++){
								cout << currentSolution.sequence.at(j); 
								if(j+1<numberOfJobs) cout << "-";
								else cout << "\n";
							}
							cout <<	"         Solucao corrente tem " << currentSolution.objFormula << "=" << currentSolution.objValue <<
								"\n         Lista tabu: {\n";
							for(int m=0; m<localTabuList.size(); m++){
								cout << "                      " << localTabuList.at(m).node1 << " troca com " << localTabuList.at(m).node2 << ",\n";
							}
							cout <<"       }\n";

							if(visualizationBin==1) system("pause"); //caso queira visualizar andamento, pausa entre itera��es
							

						}//Fim desta execu��o da busca local

					



						//***Atualiza lista de mem�ria longa, para pr�xima execu��o da busca local:
												
						//Armazena o movimento inverso ao aplicado na obten��o da melhor solu��o encontrada e o movimento usado na modifica��o seguinte � melhor solu��o encontrada, caso tais movimentos tenham sido de fato realizados:
							
						//se a solu��o inicial for a melhor solu��o encontrada at� agora, basta acrescentar na lista de longa mem�ria o atributo do movimento que a modificou nessa �ltima busca
						if(bestSolutionPosition==0) globalTabuList.push_back(solutionPath.at(bestSolutionPosition+1).moveTrack);

						//caso a �ltima busca local tenha encontrado um novo m�nimo, limpa lista tabu de longa mem�ria e guarda o movimento inverso do aplicado na obten��o da melhor solu��o e verifica se algum movimento foi aplicado na modifica��o da melhor solu��o
						else {	
							globalTabuList.clear();
							globalTabuList.push_back(invertMoveAttribute(solutionPath.at(bestSolutionPosition).moveTrack));												
							if (solutionPath.size() > bestSolutionPosition+1) globalTabuList.push_back(solutionPath.at(bestSolutionPosition+1).moveTrack);	
						}
						



						//***Limpa a mem�ria de curto prazo, para pr�xima execu��o de busca local:

						localTabuList.clear();
						solutionPath.clear();
						


					}//Fim da busca global


					//Pega instante de fim da solu��o
					final = GetTickCount();
					



					//****Imprime na tela e armazena os resultados finais na estrutura de dados usada no output do programa

					cout << "\n\n\nMelhor sequencia encontrada pela Busca Tabu foi: ";
					for(int m=0; m<bestSolution.sequence.size();m++){
						cout << bestSolution.sequence.at(m) << "-";
					} cout <<"\n\n\n";

					jobList = calculateJobOutputVariables(jobList,bestSolution.sequence);

					//Calcula e armazena os objetivos
					objectiveValue = calculateObjectives(jobList);	

					//imprime objetivos
					for(int obj=0; obj<11; obj++){
						cout << "CodObj " << setw(2) << objectiveValue.at(obj).code << ": " << objectiveValue.at(obj).formula << "=" << setw(3) << objectiveValue.at(obj).value << endl;
					} cout <<endl;

					//armazena as vari�veis de sa�da da inst�ncia executada
					objectivesOfInstance.push_back(objectiveValue);
					sequenceOfInstance.push_back(bestSolution.sequence);
					timeForFindingSolutionOfInstance.push_back(final - inicio);
					jobOutputOfInstance.push_back(jobList);
					objectiveOfChoiceOfInstance.push_back(objectiveValue.at(objectiveCode-1).formula);
					tabuSearchMainInputsOfInstance.push_back(tabuSearchMainInputs);
					initialSequenceOfInstance.push_back(initialSequence);


					//****Zera os vetores de entrada e sa�da, para uso da pr�xima inst�ncia
					jobOrderOutput.clear();
					objectiveValue.clear();
					initialSequence.clear();
					

				}//fim do modelCode 4
			




				
				//Otimiza��o
				if(modelCodeInput == 5){
				
					cout << "Modelo de calculo " << modelCodeInput << endl << endl;

					//inicializa e define o objetivo da busca tabu
					int objectiveCodeInput;
					vector<objectiveFunction> objNames = setObjectives();

					//prompt para definicao do objetivo usado na busca tabu
					cout << "Dados as funcoes objetivo listadas abaixo, ao lado de seu respectivo codigo:\n\n";
					for(int fo=0; fo<11; fo++){
						cout << "	" << objNames.at(fo).code << " - ";
						cout << objNames.at(fo).formula << endl;
					}
					cout << "\n\nDigite o codigo da FO que deseja considerar: ";
					cin >> objectiveCodeInput;

					//Pega instante de in�cio da solu��o
					inicio = GetTickCount();


					try{
						
						//Inicializa Gurobi
						GRBEnv env = GRBEnv();
						GRBModel model = GRBModel(env);

						//DEFINE PAR�METROS UTILIZADOS NA MODELAGEM
						double planningHorizon=0;		//l � o horizonte m�ximo de processamento dos jobs
						double maxRj=0;


						//Adoto um upper bound de l igual ao m�ximo Rj + soma dos processamentos de todos os jobs
						for(int j=0; j<numberOfJobs; j++){
							maxRj = maxRj > jobList.at(j).releaseTime*10 ? maxRj : jobList.at(j).releaseTime*10;
							planningHorizon += jobList.at(j).processingTime*10;
						}
						planningHorizon += maxRj;

						
						//CRIA VARI�VEIS DE DECIS�O
							
						//x(j,t) que vale 1 se job j � iniciado em t e 0 caso contr�rio
						GRBVar** x = 0;							
						x = new GRBVar* [numberOfJobs];						
						for(int j=0; j<numberOfJobs; j++){
							x[j]=model.addVars((int)planningHorizon, GRB_BINARY);	
							model.update();
							for (int t= 0; t<(int)planningHorizon; t++){			
								ostringstream vname;
								vname << "x(" << j << "." << t << ")";
								x[j][t].set(GRB_StringAttr_VarName, vname.str());
							}
						}
						cout << "all xjt created" << endl;



						if(objectiveCodeInput==1){
							
							//Calcula Cj
							vector<GRBLinExpr> completionTime;
							for(int j=0; j<numberOfJobs; j++){
								completionTime.push_back(0);
								for(int t=0; t<planningHorizon; t++){
									completionTime.at(j) += x[j][t]*(t+10*jobList.at(j).processingTime);
								}
							}
						
							//Minimiza CMax
							GRBLinExpr CMax = 0;
							for(int j=0; j<numberOfJobs; j++){
								CMax >= completionTime.at(j);
							}
							model.setObjective(CMax, GRB_MINIMIZE);
							cout << "Funcao objetivo CMax definida com sucesso" << endl;
							

						


							//DEFINE RESTRI��ES

							//Toda posi��o da sequ�ncia de processamentos precisa ser ocupada por um job
							GRBLinExpr startTimeOfJ=0;
							for(int j=0; j<numberOfJobs; j++){
								for(int t=0; t<(int)planningHorizon; t++){
									startTimeOfJ += x[j][t];
								}
								ostringstream cname;
								cname << "AllocationOfSingleStartTimetoJob(" << j << ")";
								model.addConstr(startTimeOfJ==1, cname.str());
							}


							//Todo job precisa ser alocado a uma (e apenas uma) posicao da sequ�ncia de processamento
							GRBLinExpr sumOfxjs=0;
							for(int t=0; t<(int)planningHorizon; t++){
								for(int j =0; j<numberOfJobs; j++){
									int aux = max(t-jobList.at(j).processingTime*10, 0);
									for(int s = aux; s<= t-1; s++){
										sumOfxjs += x[j][s];
									}
								}
								ostringstream cname;
								cname << "noOverlappingOfJobAtInstant(" <<  t;
								model.addConstr(sumOfxjs<=1, cname.str());
							}


							//Job s� pode ser iniciado ap�s rj
							for(int j=0; j<numberOfJobs; j++){
								int aux = max((int)jobList.at(j).releaseTime*10,0);
								for(int t=0; t < aux; t++){
									ostringstream cname;
									cname << "dontStart(" <<  j << ")before(" << t << ")" ;
									model.addConstr(x[j][t]==0, cname.str());
								}
							}
							

					
							//FIM DA MODELAGEM MATEM�TICA
							model.update();
							cout << "\nModelo inicializado com sucesso" << endl;



							//GRAVA E OTIMIZA O PROBLEMA MODELADO
							model.write("modeloMatematicoDaOtimizacao1.lp"); //escreve modelo em arquivo para checagem
							model.optimize();



							//Grava resultados da sequ�ncia
							final = GetTickCount();
							system("pause");
							



						}//Fim da modelagem objetivo 1

						
						if(objectiveCodeInput==2){

							//Calcula Cj
							vector<GRBLinExpr> completionTime;
							for(int j=0; j<numberOfJobs; j++){
								completionTime.push_back(0);
								for(int t=0; t<planningHorizon; t++){
									completionTime.at(j) += x[j][t]*(t+10*jobList.at(j).processingTime);
								}
							}
						
							//Minimiza SomaCj
							GRBLinExpr SumOfCj = 0;
							for(int j=0; j<numberOfJobs; j++){
								SumOfCj += completionTime.at(j);
							}
							model.setObjective(SumOfCj, GRB_MINIMIZE);
							cout << "Funcao objetivo Sum Cj definida com sucesso" << endl;
							

						


							//DEFINE RESTRI��ES

							//Toda posi��o da sequ�ncia de processamentos precisa ser ocupada por um job
							GRBLinExpr startTimeOfJ=0;
							for(int j=0; j<numberOfJobs; j++){
								for(int t=0; t<(int)planningHorizon; t++){
									startTimeOfJ += x[j][t];
								}
								ostringstream cname;
								cname << "AllocationOfSingleStartTimetoJob(" << j << ")";
								model.addConstr(startTimeOfJ==1, cname.str());
							}


							//Todo job precisa ser alocado a uma (e apenas uma) posicao da sequ�ncia de processamento
							GRBLinExpr sumOfxjs=0;
							for(int t=0; t<(int)planningHorizon; t++){
								for(int j =0; j<numberOfJobs; j++){
									int aux = max(t-jobList.at(j).processingTime*10, 0);
									for(int s = aux; s<= t-1; s++){
										sumOfxjs += x[j][s];
									}
								}
								ostringstream cname;
								cname << "noOverlappingOfJobAtInstant(" <<  t;
								model.addConstr(sumOfxjs<=1, cname.str());
							}


							//Job s� pode ser iniciado ap�s rj
							for(int j=0; j<numberOfJobs; j++){
								int aux = max((int)jobList.at(j).releaseTime*10,0);
								for(int t=0; t < aux; t++){
									ostringstream cname;
									cname << "dontStart(" <<  j << ")before(" << t << ")" ;
									model.addConstr(x[j][t]==0, cname.str());
								}
							}
							

					
							//FIM DA MODELAGEM MATEM�TICA
							model.update();
							cout << "\nModelo inicializado com sucesso" << endl;



							//GRAVA E OTIMIZA O PROBLEMA MODELADO
							model.write("modeloMatematicoDaOtimizacao2.lp"); //escreve modelo em arquivo para checagem
							model.optimize();



							//Grava resultados da sequ�ncia
							final = GetTickCount();
							system("pause");
							


						}
	
						if(objectiveCodeInput==3){

							//Calcula Cj
							vector<GRBLinExpr> completionTime;
							for(int j=0; j<numberOfJobs; j++){
								completionTime.push_back(0);
								for(int t=0; t<planningHorizon; t++){
									completionTime.at(j) += x[j][t]*(t+10*jobList.at(j).processingTime);
								}
							}
						
							//Minimiza SomaWjCj
							GRBLinExpr SumOfWjCj = 0;
							for(int j=0; j<numberOfJobs; j++){
								SumOfWjCj += completionTime.at(j)*jobList.at(j).penaltyWeight;
							}
							model.setObjective(SumOfWjCj, GRB_MINIMIZE);
							cout << "Funcao objetivo Sum WjCj definida com sucesso" << endl;
							

						


							//DEFINE RESTRI��ES

							//Toda posi��o da sequ�ncia de processamentos precisa ser ocupada por um job
							GRBLinExpr startTimeOfJ=0;
							for(int j=0; j<numberOfJobs; j++){
								for(int t=0; t<(int)planningHorizon; t++){
									startTimeOfJ += x[j][t];
								}
								ostringstream cname;
								cname << "AllocationOfSingleStartTimetoJob(" << j << ")";
								model.addConstr(startTimeOfJ==1, cname.str());
							}


							//Todo job precisa ser alocado a uma (e apenas uma) posicao da sequ�ncia de processamento
							GRBLinExpr sumOfxjs=0;
							for(int t=0; t<(int)planningHorizon; t++){
								for(int j =0; j<numberOfJobs; j++){
									int aux = max(t-jobList.at(j).processingTime*10, 0);
									for(int s = aux; s<= t-1; s++){
										sumOfxjs += x[j][s];
									}
								}
								ostringstream cname;
								cname << "noOverlappingOfJobAtInstant(" <<  t;
								model.addConstr(sumOfxjs<=1, cname.str());
							}


							//Job s� pode ser iniciado ap�s rj
							for(int j=0; j<numberOfJobs; j++){
								int aux = max((int)jobList.at(j).releaseTime*10,0);
								for(int t=0; t < aux; t++){
									ostringstream cname;
									cname << "dontStart(" <<  j << ")before(" << t << ")" ;
									model.addConstr(x[j][t]==0, cname.str());
								}
							}
							

					
							//FIM DA MODELAGEM MATEM�TICA
							model.update();
							cout << "\nModelo inicializado com sucesso" << endl;



							//GRAVA E OTIMIZA O PROBLEMA MODELADO
							model.write("modeloMatematicoDaOtimizacao3.lp"); //escreve modelo em arquivo para checagem
							model.optimize();



							//Grava resultados da sequ�ncia
							final = GetTickCount();
							system("pause");
							


						}



						if(objectiveCodeInput==4){

							//Calcula Cj-Rj
							vector<GRBLinExpr> queuingTime;
							for(int j=0; j<numberOfJobs; j++){
								queuingTime.push_back(0);
								for(int t=0; t<planningHorizon; t++){
									queuingTime.at(j) += x[j][t]*(t+10*jobList.at(j).processingTime-10*jobList.at(j).releaseTime);
								}
							}
						
							//Minimiza SomaWj(Cj-Rj)
							GRBLinExpr SumOfWjQj = 0;
							for(int j=0; j<numberOfJobs; j++){
								SumOfWjQj += queuingTime.at(j)*jobList.at(j).penaltyWeight;
							}
							model.setObjective(SumOfWjQj, GRB_MINIMIZE);
							cout << "Funcao objetivo Sum Wj(Cj-Rj) definida com sucesso" << endl;
							

						


							//DEFINE RESTRI��ES

							//Toda posi��o da sequ�ncia de processamentos precisa ser ocupada por um job
							GRBLinExpr startTimeOfJ=0;
							for(int j=0; j<numberOfJobs; j++){
								for(int t=0; t<(int)planningHorizon; t++){
									startTimeOfJ += x[j][t];
								}
								ostringstream cname;
								cname << "AllocationOfSingleStartTimetoJob(" << j << ")";
								model.addConstr(startTimeOfJ==1, cname.str());
							}


							//Todo job precisa ser alocado a uma (e apenas uma) posicao da sequ�ncia de processamento
							GRBLinExpr sumOfxjs=0;
							for(int t=0; t<(int)planningHorizon; t++){
								for(int j =0; j<numberOfJobs; j++){
									int aux = max(t-jobList.at(j).processingTime*10, 0);
									for(int s = aux; s<= t-1; s++){
										sumOfxjs += x[j][s];
									}
								}
								ostringstream cname;
								cname << "noOverlappingOfJobAtInstant(" <<  t;
								model.addConstr(sumOfxjs<=1, cname.str());
							}


							//Job s� pode ser iniciado ap�s rj
							for(int j=0; j<numberOfJobs; j++){
								int aux = max((int)jobList.at(j).releaseTime*10,0);
								for(int t=0; t < aux; t++){
									ostringstream cname;
									cname << "dontStart(" <<  j << ")before(" << t << ")" ;
									model.addConstr(x[j][t]==0, cname.str());
								}
							}
							

					
							//FIM DA MODELAGEM MATEM�TICA
							model.update();
							cout << "\nModelo inicializado com sucesso" << endl;



							//GRAVA E OTIMIZA O PROBLEMA MODELADO
							model.write("modeloMatematicoDaOtimizacao4.lp"); //escreve modelo em arquivo para checagem
							model.optimize();



							//Grava resultados da sequ�ncia
							final = GetTickCount();
							system("pause");
							


						}

						

	

					
					}//fim da modelagem de otimiza��o

					catch(GRBException e) {
						cout << "\nCodigo erro do Gurobi = " << e.getErrorCode() << endl;
						cout << e.getMessage() << endl;
					} catch(...) {
						cout << "\nExcecao durante otimizacao no Gurobi" << endl;
					}




					//****Imprime na tela e armazena os resultados finais na estrutura de dados usada no output do programa

					cout << "\n\n\nMelhor sequencia encontrada pela Otimizacao foi: ";
					for(int k=0; k<jobOrderOutput.size();k++){
						cout << jobOrderOutput.at(k) << "-";
					} cout <<"\n\n\n";			

					//Calcula e armazena os objetivos
					jobList = calculateJobOutputVariables(jobList,jobOrderOutput);
					objectiveValue = calculateObjectives(jobList);	

					//imprime objetivos
					for(int obj=0; obj<11; obj++){
						cout << "CodObj " << setw(2) << objectiveValue.at(obj).code << ": " << objectiveValue.at(obj).formula << "=" << setw(3) << objectiveValue.at(obj).value << endl;
					} cout <<endl;

					//armazena as vari�veis de sa�da da inst�ncia executada
					objectivesOfInstance.push_back(objectiveValue);
					sequenceOfInstance.push_back(jobOrderOutput);
					timeForFindingSolutionOfInstance.push_back(final - inicio);
					jobOutputOfInstance.push_back(jobList);
					objectiveOfChoiceOfInstance.push_back(objectiveValue.at(objectiveCodeInput-1).formula);
					



					//****Zera os vetores de entrada e sa�da, para uso da pr�xima inst�ncia
					jobOrderOutput.clear();
					objectiveValue.clear();



					

				}//Fim do modelCode 5
			
					
			

			}//fim do c�lculo da inst�ncia usando o modelo selecionado
			
				


		

		
			/******************************************Rotina que salva resultados em dois arquivos csv*********************************************/

			//Codigos dos modelos e dos objetivos, para impressao de resultados finais
			vector<codeDescr> calcModel = modelTypes();
			
			//nome do arquivo com tabelas de resultados
			ostringstream outputFile;
			outputFile << "ResultadosLuisa - Modelo" << modelCodeInput << " - inst " << firstInstance << " a " << lastInstance <<".csv";
						
			//cria arquivo de resultados
			ofstream myfile;
			myfile.open (outputFile.str());

			//Se o modelo adotado for Busca Aleat�ria ou Tabu, imprime os inputs do usu�rio
			if(modelCodeInput==2){
				myfile <<"Inputs do Usuario:\n" <<
					"Inst;NumIteracoes;ObjAdotado\n";
				for(int inst=0; inst<=lastInstance-firstInstance; inst++){
					myfile 
						<< firstInstance+inst << ";" 
						<< randomSearchInputOfInstance.at(inst) << ";"
						<< objectiveOfChoiceOfInstance.at(inst) << "\n" ;
				}
				myfile << "\n\n";
			}
			else if(modelCodeInput==4){
				myfile <<"Inputs do Usuario:\n" <<
					"Inst;CriterioParGlobal;CriterioParLocal;TamanhoListaTabu;Sequencia Inicial;ObjAdotado\n";
				for(int inst=0; inst<=lastInstance-firstInstance; inst++){
					myfile << firstInstance+inst << ";"
					 << tabuSearchMainInputsOfInstance.at(inst).at(0) << ";"
					 << tabuSearchMainInputsOfInstance.at(inst).at(1) << ";"
					 << tabuSearchMainInputsOfInstance.at(inst).at(2) << ";";
					for(int j=0; j<initialSequenceOfInstance.at(inst).size(); j++){
						myfile << initialSequenceOfInstance.at(inst).at(j) << "-";
					}
					 myfile << ";" << objectiveOfChoiceOfInstance.at(inst) << "\n" ;
				}
				myfile << "\n\n";
			}


			//Sa�da 1 - cabe�alho da tabela de resultados 1
			myfile << "SchedSolu1\n";
			myfile << "Inst;OrdNum;ObjDescr;ObjVal;ModDescr;Tempo(ms);ObjAdotado\n";


			//valores da tabela 1
			for(int inst=0; inst<=lastInstance-firstInstance; inst++){

				for(int obj=0; obj<11; obj++){

					myfile << firstInstance+inst << ";" << readNumberOfJobs(firstInstance+inst) << ";";
					myfile << objectivesOfInstance.at(inst).at(obj).formula << ";" << objectivesOfInstance.at(inst).at(obj).value << ";";
					myfile << calcModel.at(modelCodeInput-1).description <<";";
					myfile << timeForFindingSolutionOfInstance.at(inst) << ";";
					myfile << objectiveOfChoiceOfInstance.at(inst) << "\n" ;
					
				}

			}

			//myfile.close();

			

			//Sa�da 2 - tabela SchedSolu2 que detalha in�cio e fim de processamento de cada job j		
			
			//cabe�alho da tabela 2
			myfile << "\n\n\nSchedSolu2\n";
			myfile << "Inst; ModDescr; Seq; Ord; IIni; IFin\n";
			
			//valores da tabela de sa�da 2
			for(int inst=0; inst<=lastInstance-firstInstance; inst++){

				for(int seq=0; seq<readNumberOfJobs(firstInstance+inst); seq++){

					myfile << firstInstance+inst << ";" << calcModel.at(modelCodeInput-1).description << ";";
					myfile << seq+1 << ";" << sequenceOfInstance.at(inst).at(seq) << ";";
					myfile << jobOutputOfInstance.at(inst).at(sequenceOfInstance.at(inst).at(seq)-1).sj << ";";
					myfile << jobOutputOfInstance.at(inst).at(sequenceOfInstance.at(inst).at(seq)-1).cj << ";";
					myfile << "\n";

				}
			
			}



			//Fecha arquivo com outputs do modelo executado
			myfile.close();



			/******************************************FIM da rotina que salva resultados em dois arquivos csv**************************************/



			//Verifica se o usu�rio quer rodar o programa de novo
			cout << "\n\nDeseja rodar o programa de novo?\n  Para continuar digite 1, para sair digite qq outro numero: ";
			cin >> doItAgain;

			//Zera as estruuras de dados de sa�da, para uso na pr�xima execu��o
			sequenceOfInstance.clear();					
			objectivesOfInstance.clear();				
			timeForFindingSolutionOfInstance.clear();	
			jobOutputOfInstance.clear();
			objectiveOfChoiceOfInstance.clear();



		} while (doItAgain==1); //fim da rotina de execu��o do programa

		

		system("pause");


		return 0;



}