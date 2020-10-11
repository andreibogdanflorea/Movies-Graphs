#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX_NAME_LENGTH 100
#define MAX_ACTORS_PER_FILM 1000
#define INF 2000000000
#define MAX_ACTORS 1000003

// functie pentru citirea informatiilor din fisier si construirea grafului aferent
void constructGraph(TGraph *graph, FILE *fin) {
	int films_number, actors_number;
	char film_name[1000];
	fscanf(fin, "%d\n", &films_number);
	// am creat un hashtable si un graf cu capacitate destul de mare de la inceput
	THash *hash_table = createHashTable(MAX_ACTORS);
	graph->capacity = MAX_ACTORS_PER_FILM * films_number;
	graph->number_of_nodes = 0;
	graph->adl = malloc(graph->capacity * sizeof(TNode*));
	for (int i = 0; i < graph->capacity; i++) {
		graph->adl[i] = NULL;
	}
	//node_names este corespondenta intre indice si numele autorului
	graph->node_names = calloc(graph->capacity, sizeof(char*));

	for (int i = 0; i < films_number; i++) {
		fgets(film_name, sizeof(film_name), fin);
		film_name[strlen(film_name) - 1] = '\0';
		fscanf(fin, "%d\n", &actors_number);
		char **actors = malloc(actors_number * sizeof(char*));
		for (int j = 0; j < actors_number; j++) {
			actors[j] = malloc(MAX_NAME_LENGTH * sizeof(char));
			fgets(actors[j], MAX_NAME_LENGTH * sizeof(char), fin);
			actors[j][strlen(actors[j]) -1] = '\0';
		}
		// parcurg fiecare pereche de autori de la filmul curent pentru a crea o muchie intre ei
		for (int j = 0; j < actors_number - 1; j++) {
			for (int k = j + 1; k < actors_number; k++) {
				// adaug/caut indicele corespunzator actorilor
				int first_node = insertInHashTable(hash_table, actors[j]);
				int second_node = insertInHashTable(hash_table, actors[k]);
				// creez muchia intre acesti indici, si atribui si numele actorilor 
				// in campurile nodului din graf
				addEdge(graph, first_node, second_node, actors[j], actors[k]);
			}
		}
		for (int j = 0; j < actors_number; j++) {
			free(actors[j]);
		}
		free(actors);
	}

	destroyHashTable(hash_table);
}

// implementarea algoritmului DFS, care, in steps, numara cate noduri a vizitat
void dfs(TGraph *graph, int node, int *visited, int *steps) {
	visited[node] = 1;
	(*steps)++;
	for (TNode *it = graph->adl[node]; it != NULL; it = it->next) {
		if (!visited[it->idx]) {
			dfs(graph, it->idx, visited, steps);
		}
	}
}

void dfsActorNames(TGraph *graph, int node, int *visited, char **actors, int *n) {
	visited[node] = 1;
	actors[(*n)++] = graph->node_names[node];
	for (TNode *it = graph->adl[node]; it != NULL; it = it->next) {
		if (!visited[it->idx]) {
			dfsActorNames(graph, it->idx, visited, actors, n);
		}
	}
}
// functie pentru cerinta 1
// practic se cere componenta conexa cu cele mai multe noduri
int maximumProduction(TGraph *graph, char **actors) {
	int maximum = 0, start_node = 0;
	int *visited = calloc(graph->number_of_nodes, sizeof(int));
	for (int i = 0; i < graph->number_of_nodes; i++) {
		if (!visited[i]) {
			int steps = 0;
			dfs(graph, i, visited, &steps);
			if (maximum < steps) {
				start_node = i;
				maximum = steps;
			}
		}
	}
	free(visited);

	// pentru a reface actorii din componeneta conexa cu nr maxim de elemente
	visited = calloc(graph->number_of_nodes, sizeof(int));
	int n = 0;
	dfsActorNames(graph, start_node, visited, actors, &n);
	free(visited);
	return maximum;
}

// functie care returneaza indexul corespunzator unui actor
// avand ca referinta acel vector node_names al grafului, in care este retinuta
// conexiunea intre index si actor
int getIndex(TGraph *graph, char *actor) {
	for (int i = 0; i < graph->number_of_nodes; i++) {
		if (strcmp(graph->node_names[i], actor) == 0)
			return i;
	}
	return -1;
}

// implementarea BFS pentru cerinta 2
// se gaseste distanta minima dintre doua noduri
int bfs(TGraph *graph, char *actor1, char *actor2) {
	Queue *q = createQueue();
	// nodurile sunt de fapt acesti indecsi, de aceea am nevoie de transformarea ei
	// si trebuie sa gasesc indexul corespunzator unui autor
	int idx1 = getIndex(graph, actor1);
	int idx2 = getIndex(graph, actor2);
	QueueElem elem;
	elem.distance = 0; elem.idx = idx1;
	enqueue(q, elem);
	
	int *visited = calloc(graph->number_of_nodes, sizeof(int));
	visited[idx1] = 1;
	while (!isQueueEmpty(q)) {
		QueueElem elem = front(q);
		dequeue(q);
		// daca am gasit nodul cautat, garantat este la cea mai mica distanta, si returnez
		// distanta curenta
		if (elem.idx == idx2) {
			free(visited);
			destroyQueue(q);
			return elem.distance;
		}
		for (TNode *it = graph->adl[elem.idx]; it != NULL; it = it->next) {
			if (!visited[it->idx]) {
				QueueElem new_elem;
				visited[it->idx] = 1;
				new_elem.idx = it->idx; new_elem.distance = elem.distance + 1;
				enqueue(q, new_elem);
			}
		}
	}
	free(visited);
	destroyQueue(q);
	return -1;
}

int min(int a, int b) {
	return a < b? a: b;
}

// functie care adauga doi actori (o punte) ca un element din vectorul rezultat la cerinta 3
void addToResult(TarjanResult *result, char *actor1, char *actor2) {
	TwoActors *new_element = malloc(sizeof(TwoActors));
	new_element->actor1 = strdup(actor1);
	new_element->actor2 = strdup(actor2);
	result->elements[result->size] = new_element;
	result->size++;
}

// implementarea algoritmului lui Tarjan descris
void dfs_tarjan(TGraph *graph, FILE *fout, int node, int *time, int *indexes,
				int *low, int *parents, TarjanResult *result) {
	indexes[node] = *time;
	low[node] = *time;
	(*time)++;
	for (TNode *it = graph->adl[node]; it != NULL; it = it->next) {
		int new_node = it->idx;
		if (new_node != parents[node]) {
			if (indexes[new_node] == -1) {
				parents[new_node] = node;
				dfs_tarjan(graph, fout, new_node, time, indexes, low, parents, result);
				low[node] = min(low[node], low[new_node]);
				if (low[new_node] > indexes[node]) {
					addToResult(result, graph->node_names[node], graph->node_names[new_node]);
				}
			} else {
				low[node] = min(low[node], indexes[new_node]);
			}
		}	
	}
}

void tarjan(TGraph *graph, FILE *fout) {
	int time = 0;
	int *indexes = malloc(graph->number_of_nodes * sizeof(int));
	int *low = malloc(graph->number_of_nodes * sizeof(int));
	int *parents = malloc(graph->number_of_nodes * sizeof(int));
	for (int i = 0; i < graph->number_of_nodes; i++) {
		indexes[i] = -1;
		low[i] = INF;
		parents[i] = -1;
	}

	// aloc memorie pentru vectorul cu puntile cerute
	// intr-un graf cu N noduri pot fi maxim N-1 punti, deci e de ajuns dimensiunea N
	TarjanResult *result = createResult(graph->number_of_nodes);

	for (int i = 0; i < graph->number_of_nodes; i++) {
		if (indexes[i] == -1) {
			dfs_tarjan(graph, fout, i, &time, indexes, low, parents, result);
		}
	}
	free(indexes); free(low); free(parents);

	// sortez alfabetic in modul indicat
	sortResult(result);
	fprintf(fout, "%d\n", result->size);
	for (int i = 0; i < result->size; i++) {
		fprintf(fout, "%s %s\n", result->elements[i]->actor1, result->elements[i]->actor2);
	}

	destroyResult(result);
}

int compareActors(const void *a, const void *b) {
	const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}


void print_adl(TGraph *graph) {
	for (int i = 0; i < graph->number_of_nodes; i++) {
		TNode *p = graph->adl[i];
		printf("%s: ", graph->node_names[i]);
		while (p != NULL) {
			printf("%s ", p->name);
			p = p->next;
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	FILE *fin = fopen(argv[2], "r");
	FILE *fout = fopen(argv[3], "w");
	
	// se construieste graful cu ajutorul informatiilor din fisier
	TGraph *graph = malloc(sizeof(TGraph));
	constructGraph(graph, fin);
	// diferentierea un functie de cerinta
	if (strcmp(argv[1], "-c1") == 0) {
		char **actors_output = malloc(graph->number_of_nodes * sizeof(char*));
		int max_actors = maximumProduction(graph, actors_output);
		fprintf(fout, "%d\n", max_actors);
		qsort(actors_output, max_actors, sizeof(char*), compareActors);
		for (int i = 0; i < max_actors; i++) {
			fprintf(fout, "%s\n", actors_output[i]);
		}
		free(actors_output);
	} else if (strcmp(argv[1], "-c2") == 0) {
		char *actor1 = malloc(MAX_NAME_LENGTH * sizeof(char));
		char *actor2 = malloc(MAX_NAME_LENGTH * sizeof(char));
		fgets(actor1, MAX_NAME_LENGTH * sizeof(char), fin);
		fgets(actor2, MAX_NAME_LENGTH * sizeof(char), fin);
		actor1[strlen(actor1) - 1] = '\0';
		actor2[strlen(actor2) - 1] = '\0';
		int min_distance = bfs(graph, actor1, actor2);
		fprintf(fout, "%d\n", min_distance);
		free(actor1); free(actor2);
	} else {
		tarjan(graph, fout);
	}

	destroyGraph(graph);
	fclose(fin);
	fclose(fout);
	return 0;
}