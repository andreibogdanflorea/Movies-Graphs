#ifndef UTIL_H
#define UTIL_H

// nod din graf
typedef struct TNode {
	int idx;
	char *name;
	struct TNode *next;
} TNode;

// graful
typedef struct {
	int capacity;
	int number_of_nodes;
	char **node_names;
	TNode **adl;
} TGraph;

typedef struct TEntry {
	char *name;
	int number;
	struct TEntry *next;
} Entry;

// hashmap
typedef struct {
	int capacity;
	int number_of_entries;
	Entry **table;
} THash;


// tipul elementului din coada, alcatuit din indexul unui nod si distanta de la nodul start la acest nod 
typedef struct QueueElem {
	int distance, idx;
} QueueElem;

// nod al cozii
typedef struct QueueNode {
	QueueElem elem;
	struct QueueNode *next;
} QueueNode;

// coada
typedef struct Queue {
	QueueNode *front;
	QueueNode *rear;
	long size;
} Queue;

// folosit pentru afisare la cerinta 3: numele a doi actori care compun o punte
typedef struct {
	char *actor1, *actor2;
} TwoActors;


// vectorul de punti afisat la cerinta 3
typedef struct TarjanResult {
	TwoActors **elements;
	int size;
} TarjanResult;


// functii pentru graf
void addEdge(TGraph *graph, int node1, int node2, char *name1, char *name2);
void destroyGraph(TGraph *graph);

// functii pentru hashmap
THash *createHashTable(int capacity);
int hashFunction(char *actor);
int insertInHashTable(THash *hash_table, char *actor);
void destroyHashTable(THash *hash_table);


// functiile pentru coada
Queue* createQueue(void);
void destroyQueue(Queue* queue);
void enqueue(Queue* queue, QueueElem elem);
void dequeue(Queue* queue);
int isQueueEmpty(Queue *queue);
QueueElem front(Queue* queue);


// functiile folosite pentru cerinta 3
TarjanResult *createResult(int capacity);
void sortResult(TarjanResult *result);
void destroyResult(TarjanResult *result);

#endif