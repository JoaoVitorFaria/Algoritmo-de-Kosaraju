//Nome: João Vitor de Faria
//Matricula: 2019006030
//Trabalho 04
//Codigo baseado no codigo disponibilizado pelo professor
#include<stdio.h>
#include<stdlib.h>
#include<utility>  
#include<iterator> 
#include<map>

//estrutura que define o arco
typedef struct arco{
	int verticeInicio;
	int verticeFim;
	struct arco * prox;
}Arco;
//estrutura que define o vertice
typedef struct vertice{
	Arco * arcosDeEntrada;
	Arco * arcosDeSaida;
	int visitado;
	int componenteConexo;//variavel usada para salvar o  componente conexo a qual o vertice pertence
}Vertice;

//estrutura que define o grafo
typedef struct digrafo{
	int vertices;//n
	int arestas;//m
	Vertice * vetorVertices;
}Digrafo;

//funcao para criacao do grafo
Digrafo * criaGravo(int vertices){
	Digrafo * grafo = (Digrafo *)malloc(sizeof(Digrafo));
	grafo->vetorVertices=(Vertice*)malloc(vertices*sizeof(Vertice));
	grafo->vertices=vertices;
	grafo->arestas=0;
	for(int i=0;i<vertices;i++){
		grafo->vetorVertices[i].visitado=0;
		grafo->vetorVertices[i].arcosDeEntrada=NULL;
		grafo->vetorVertices[i].arcosDeSaida=NULL;
	}
	return grafo;
}

//funcao para criar o arco
Arco * criaArco(int inicio, int fim){
	Arco * A=(Arco*)malloc(sizeof(Arco));
	A->verticeInicio=inicio;
	A->verticeFim=fim;
	A->prox=NULL;
	return A;
}
//funcao para adicionar o arco ao grafo
void adicionaArco(Digrafo * grafo, int inicio, int fim){
	grafo->arestas++;
	Arco * entrada= criaArco(inicio, fim);
	entrada->prox=grafo->vetorVertices[fim].arcosDeEntrada;
	grafo->vetorVertices[fim].arcosDeEntrada=entrada;
	
	Arco * saida= criaArco(inicio, fim);
	saida->prox=grafo->vetorVertices[inicio].arcosDeSaida;
	grafo->vetorVertices[inicio].arcosDeSaida=saida;
	
	return ;
}
//funcao para zerar o grafo, deixando ele inteiro como não visitado
void marcarVerticeNaoVisitado(Digrafo * grafo){
	for(int i=0;i<grafo->vertices;i++){
		grafo->vetorVertices[i].visitado=0;
	}
}
//funcao que faz o primeiro dfs do algoritmo de kosaraju
void primeiroDFS(Digrafo * grafo, int verticeAtual, int * vetorTempoTermino, int * tempo){
	grafo->vetorVertices[verticeAtual].visitado=1;
	
	for (Arco * a= grafo->vetorVertices[verticeAtual].arcosDeEntrada; a!=NULL; a=a->prox){
		int vizinho=a->verticeInicio;
		if(grafo->vetorVertices[vizinho].visitado==0){
			primeiroDFS(grafo, vizinho, vetorTempoTermino, tempo );
		}
	}
	(*tempo)++;
	vetorTempoTermino[verticeAtual]=(*tempo);
}
//funcao que faz o segundo dfs do algoritmo de kosaraju
void segundoDFS(Digrafo * grafo, int vertice,  int * pertence, int * lider){
	grafo->vetorVertices[vertice].visitado=1;
	//esse for percorre no sentido contrario do primeiro dfs pois na primeira execucao estavamos buscando 
	//o grafo invertido
	for(Arco * a = grafo->vetorVertices[vertice].arcosDeSaida; a!=NULL; a=a->prox){
		int vizinho=a->verticeFim;
		if(grafo->vetorVertices[vizinho].visitado==0){
			segundoDFS(grafo, vizinho,pertence, lider);
		}
	}
	//a cada novo vertice eu verifico se ele é menor que o lider anterior. Desse modo eu vou atualizando o lider
	if((*lider)>vertice)(*lider)=vertice;

	
	//aqui eu salvo no vertice qual componente conexo ele pertence
	grafo->vetorVertices[vertice].componenteConexo=(*pertence);
	
			
}

void libera_lista_arcos(Arco * lista){
    if(lista == NULL) return;
    libera_lista_arcos(lista->prox);
    free(lista);
}

void liberaDigrafo(Digrafo * G){
    for(int i = 0; i < G->vertices; i++){
        libera_lista_arcos(G->vetorVertices[i].arcosDeEntrada);
        libera_lista_arcos(G->vetorVertices[i].arcosDeSaida);
    }
    free(G->vetorVertices);
    free(G);
}

//funcao do algoritmo de kosaraju
void kosaraju(Digrafo * grafo){
	int * vetorTempoTermino=(int*)malloc(grafo->vertices*sizeof(int));
	int * vetorOrdemSegundaPassada=(int *)malloc(grafo->vertices*sizeof(int));
	int tempo=0;
	int pertence=0;
	int lider=100000000;

	
	marcarVerticeNaoVisitado(grafo);
	for(int i=0;i<grafo->vertices;i++){
		if(grafo->vetorVertices[i].visitado==0){
			primeiroDFS(grafo, i, vetorTempoTermino, &tempo);
		}
	}
	
	for(int i=0;i<grafo->vertices;i++){
		vetorOrdemSegundaPassada[grafo->vertices - vetorTempoTermino[i]]=i;
	}
	
	std::map<int, int> pares; 
	//aqui começa a parte a ser feita
	//o grafo ja foi percorrido por um dfs, entao é necessario marcar ele como n percorrido 
	marcarVerticeNaoVisitado(grafo);
	for(int i=0;i<grafo->vertices;i++){
		//eu preciso percorrer ele na ordem que esta salva no vetorOrdemSegundaPassada
		//esse for acaba percorrendo esse vetor e passando os valores que estao salvos nele como indice para o vetorVertices
		//para i=0 temos vetorOrdemSegundaPassada[i]=3. sendo assim estamo fazendo grafo->vetorVertices[3]
		if(grafo->vetorVertices[vetorOrdemSegundaPassada[i]].visitado==0){
			//a variavel pertence faz a contagem do componente formente conexo
			pertence++;
			//a variavel lider vai ser alterada dentro do dfs a fim de salvar o menor valor de cada SCC
			segundoDFS(grafo, vetorOrdemSegundaPassada[i], &pertence, &lider);
			//criei um map para associar um lider a cada SCC
			pares.insert(std::pair <int,int>(pertence, lider));
		}
		//associo um valor gigante ao lider para n correr o risco de salvar nele um valor menor que o lider do SCC
		lider=100000000;
	}
	//Crio o iterator
	std::map<int, int>::iterator itr;
	for(int i=0;i<grafo->vertices;i++){
		//para cada vertice eu percorro o map para achar o pair que tenha um key igual ao atributo componenteConexo do vertice
		for (itr = pares.begin(); itr != pares.end(); ++itr){
			//se eu encontrar uma key igual ao compenente conexo eu sei que o valor salvo no pair é o lider daquele SCC
			if(grafo->vetorVertices[i].componenteConexo==itr->first){
				//dai eu printo o lider  que esta salvo no second
				printf("%d ", itr->second);
			}
		}	
	}
	printf("\n");
	free(vetorTempoTermino);
	free(vetorOrdemSegundaPassada);
}
int main(){
	int vertices, arestas;
	scanf("%d %d",&vertices, &arestas);
	//criacao do grafo
	Digrafo * grafo = criaGravo(vertices);
	//leitura dos arcos
	for(int i=0;i<arestas;i++){
		int inicio, fim;
		scanf("%d %d", &inicio, &fim);
		adicionaArco(grafo, inicio, fim);
	}
	
	kosaraju(grafo);
	liberaDigrafo(grafo);
	return 0;
}