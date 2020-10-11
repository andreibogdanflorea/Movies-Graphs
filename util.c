#include "util.h"
#include <stdlib.h>
#include <string.h>

#define mod 1000003

void addEdge(TGraph *graph, int node1, int node2, char *name1, char *name2) {
	// adauga nodul 2 in lista de vecini al nodului 1
	TNode *vecin = malloc(sizeof(TNode));
	vecin->idx = node2; vecin->name = strdup(name2);
	vecin->next = graph->adl[node1];
	graph->adl[node1] = vecin;
	
	vecin = malloc(sizeof(TNode));
	vecin->idx = node1; vecin->name = strdup(name1);
	vecin->next = graph->adl[node2];
	graph->adl[node2] = vecin;
	
	// daca am ajuns cu numerotarea nodurilor la un numar ce depaseste nr de noduri ale grafului,
	// actualizez
	if (node1 + 1 > graph->number_of_nodes) {
		graph->number_of_nodes = node1 + 1;
	}
	if (node2 + 1 > graph->number_of_nodes) {
		graph->number_of_nodes = node2 + 1;
	}

	// node_names este un vector ce contine legatura dintre numerotarea nodului si numele actorului
	if (graph->node_names[node1] == NULL) {
		graph->node_names[node1] = strdup(name1);
	}
	if (graph->node_names[node2] == NULL) {
		graph->node_names[node2] = strdup(name2);
	}
}

// functie pentru dezalocarea grafului
void destroyGraph(TGraph *graph) {
	for (int i = 0; i < graph->number_of_nodes; i++) {
		TNode *p = graph->adl[i], *prev;
		while (p != NULL) {
			prev = p;
			p = p->next;
			free(prev->name);
			free(prev);
		}
	}
	free(graph->adl);
	for (int i = 0; i < graph->number_of_nodes; i++) {
		free(graph->node_names[i]);
	}
	free(graph->node_names);

	free(graph);
}

// functie pentru crearea hashmap-ului
THash *createHashTable(int capacity) {
	THash *hash_table = malloc(sizeof(THash));
	hash_table->table = calloc(capacity, sizeof(Entry*));
	hash_table->capacity = capacity;
	hash_table->number_of_entries = 0;
	return hash_table;
}

int hashFunction(char *actor) {
	unsigned long hash = 5381;
	char ch;
	while (*actor) {
		ch = *actor++;
		hash = ((hash << 5) + hash) + ch;
	}
	return hash % mod;
}

// functie pentru adaugarea in hashtable a unui actor nou, sau pentru gasirea indicelui sau corespunzator
int insertInHashTable(THash *hash_table, char *actor) {
	int index = hashFunction(actor);
	Entry *p = hash_table->table[index];
	while (p != NULL) {
		if (strcmp(actor, p->name) == 0) {
			return p->number;
		}
		p = p->next;
	}

	Entry *new_entry = malloc(sizeof(Entry));
	new_entry->name = strdup(actor);
	new_entry->number = hash_table->number_of_entries++;
	new_entry->next = hash_table->table[index];
	hash_table->table[index] = new_entry;
	return new_entry->number;
}

// functie pentru dezalocarea unui hashmap
void destroyHashTable(THash *hash_table) {
	for (int i = 0; i < hash_table->capacity; i++) {
		Entry *p = hash_table->table[i], *tmp;
		while (p != NULL) {
			tmp = p;
			p = p->next;
			free(tmp->name);
			free(tmp);
		}
	}
	free(hash_table->table);
	free(hash_table);
}


// implementarea uzuala a cozii, folosita pentru BFS
Queue *createQueue() {
	Queue *queue = (Queue*)malloc(sizeof(Queue));
	queue->size = 0;
	queue->front = NULL;
	queue->rear = NULL;
	return queue;
}

int isQueueEmpty(Queue *q) {
	return q->front == NULL;
}

QueueElem front(Queue *q) {
	return q->front->elem;
}

void enqueue(Queue *q, QueueElem elem) {
	QueueNode *newNode = (QueueNode*)malloc(sizeof(QueueNode));
	newNode->elem = elem;
	newNode->next = NULL;

	if (isQueueEmpty(q)) {
		q->front = newNode;
		q->rear = newNode;
		q->size++;
		return;
	}

	q->rear->next = newNode;
	q->rear = newNode;
	q->size++;
}

void dequeue(Queue *q) {
	if (isQueueEmpty(q))
		return;

	QueueNode *temp = q->front;
	q->front = q->front->next;
	if (q->front == NULL) {
		q->rear = NULL;
	}
	free(temp);
	q->size--;
}


void destroyQueue(Queue *q) {
	while (!isQueueEmpty(q)) {
		dequeue(q);
	}
	free(q);
}

// functie pentru crearea unui vector cu rezultatele de la cerinta 3
TarjanResult *createResult(int capacity) {
	TarjanResult *result = malloc(sizeof(TarjanResult));
	result->elements = malloc(capacity * sizeof(TwoActors*));
	result->size = 0;
	return result;
}

int compareActors_Tarjan(const void *a, const void *b) {
	const TwoActors *res1 = *(const TwoActors**)a;
   	const TwoActors *res2 = *(const TwoActors**)b;
   	int result = strcmp(res1->actor1, res2->actor1);
   	if (result == 0)
   		return strcmp(res1->actor2, res2->actor2);
    return strcmp(res1->actor1, res2->actor1);
}

// functie pentru sortarea alfabetica indicata
void sortResult(TarjanResult *result) {
	for (int i = 0; i < result->size; i++) {
		if (strcmp(result->elements[i]->actor1, result->elements[i]->actor2) > 0) {
			char *temp = result->elements[i]->actor1;
			result->elements[i]->actor1 = result->elements[i]->actor2;
			result->elements[i]->actor2 = temp;
		}
	}

	qsort(result->elements, result->size, sizeof(TwoActors*), compareActors_Tarjan);
}

// functie pentru dezalocarea vectorului cu rezultatele de la cerinta 3
void destroyResult(TarjanResult *result) {
	for (int i = 0; i < result->size; i++) {
		free(result->elements[i]->actor1);
		free(result->elements[i]->actor2);
		free(result->elements[i]);
	}
	free(result->elements);
	free(result);
}