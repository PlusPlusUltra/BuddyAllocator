#include <stdio.h>
#include <math.h>
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);
//functions to work with integers as if they were bits
//i will store the bitmap as an array of integers and then work with the single bits of each integer
void setBit(int array[], int bitIndex) //sets the bit at index bitIndex
{
	int intIndex = bitIndex / (sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	array[intIndex] = array[intIndex] | flag;
}

void clearBit(int array[], int bitIndex) //clears the bit at index bitIndex
{
	int intIndex = bitIndex / (sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	flag = ~flag;
	array[intIndex] = array[intIndex] & flag;
}
int getBit(int array[], int bitIndex) //returns the value of the bit at index bitIndex. Can probably use char to return
{
	int intIndex = bitIndex /(sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	if (array[intIndex] & flag) return 1;
	else return 0;
}
//let's create the struct
typedef struct {
	int tree[];
	int num_levels;
	char* memory;
	int min_bucket_size;
} BuddyAllocator;
//tree functions
int levelIdx(size_t idx)
{
	return (int)floor(log2(idx));
}
int buddyIdx(int idx){
	if (idx&0x1){ 
		return idx -1;
	}
	return idx+1;
}
int parentIdx(int idx){
	return idx/2;
}
int startIdx(int idx){
	return (idx-(1<<levelIdx(idx)));
}
int leftSon(int idx){
	return idx*2;
}
int rightSon(int idx){
	return idx*2 + 1;
}

//allocator functions
void* chunkGivenIndex(BuddyAllocator* alloc, int idx){
	int level = levelIdx(idx);
	int firstOfLevel = pow(2,level);
	int toAdd = alloc->min_bucket_size * (alloc->num_levels - level) * (idx - firstOfLevel);
	return alloc->memory + toAdd;
}
void* BuddyAllocator_malloc(BuddyAllocator alloc, int size){
	//calculate max mem
	int mem_size= (1<<alloc->num_levels)*min_bucket_size;
	//calculate level for page
	int level = floor(log2(mem_size/(size+8)));
	
	//if the allocator does not have enough levels we just use the max level
	if (level > alloc->num_levels) level = num_levels;
	printf("requested: %d bytes, level %d \n",
	size, level);
	// get a buddy of the right size
	int buddy = BuddyAllocator_getBuddy(alloc,size);
	//write in the memory
	void* ret = chunkGivenIndex(alloc, buddy);
	int* forLater = (int*)ret;
	ret += 8;
	*forLater = buddy;
	return ret;
	////////////
}
int minBuddy(BuddyAllocator* alloc, int level) //find the index of the smallest available buddy
{
	int i;
	for (i = pow(2,level+1) - 1; i > 0; i--)
	{
		if (getBit(alloc->tree, i) return i;
	}
	return 0;
	
}
char* chunkGivenIndex(char* memory, int idx{
}
//clearSons(BuddyAllocator alloc){//makes sure all sons of all allocated blocks are not free
	//int i;
	//for (i = 1; i < pow(2,alloc->num_levels; i++)
	//{
	//	if (!getBit(alloc->tree, parentIdx(i))) clearBit(alloc->tree, i)
	//}
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
	if (level<0) return 0;
	int idx = minBuddy(alloc, level);
	clearBit(alloc->tree,idx);
	//all parents and all sons are not free anymore
	int i;
	for (i = idx; i > 0; i=parentIdx(i))
	{
		clearBit(alloc->tree,idx);
	}
	//i don't know how to clear sons. i can only do this using recursion and like this
	//clearSons(alloc);
	//nevermind i found it
	int nSons = pow(2,alloc->num_levels-level);
	int toFree[nSons];
	int currentNode;
	toFree[0]=idx;
	int writeHere = 1;
	for (currentNode = 0; currentNode < nSons; currentNode++)
	{
		clearBit(alloc->tree,toFree[currentNode]);
		if(leftSon(toFree[currentNode]) < pow(2,alloc->num_levels)
		{
			toFree[writeHere] = leftSon(toFree[currentNode]);
			writeHere++;
			toFree[writeHere] = rightSon(toFree[currentNode]);
			writeHere++;
		}
	}
	return idx;
}
int indexGivenChunk(char*memory)
{
}
void releaseBuddy (BuddyAllocator* alloc, int idx)
{
	nParents = levelIdx(idx);
	int toCheck[nParents];
	int i;
	int writeHere = 1;
	for (i = 0; i < nParents; i++) //probably not the best way to do it, no reason to use an array
	{
		setBit(alloc->tree,toCheck[i]);
		if(getBit(alloc->tree,buddyIdx(toCheck[i])){
			toCheck[writeHere] = parentIdx((alloc->tree[toCheck[i]]));
			writeHere++;
		}
		else break;
	}
}
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
	printf("freeing %p", mem);
	int* retr = (int*) mem;
	retr = retr - 8;
	int idx = *retr;
	//
	releaseBuddy(alloc, idx);
}
//trovato il modo di trovare tutti i figli di un nodo
//si fa un array di dimensione pari al numero di figli di un nodo
//si calcola facendo 2 alla livello nono - livello massimo
//poi si fa uno while
//finché non ho superato l'index massimo
//tengo traccia dell'ultimo index su cui ho scritto, sempre facendo ++
//chiamiamo i l'index su cui ho scritto , praticamente sto facendo una arraylist
//aggiungo il nodo da cui parte tutto a array[0]
//solo dopo parte il while
//all'inizio del giro prendo array[i]
//lo libero e aggiungo i suoi figli all'array in posizione i+1 e i+2
//ok ho sbagliato
//l'errore è che mi devo segnare separatamente l'indice su cui ho scritto e l'indice dove si trova
//l'ultimo nodo su cui ho lavorato
//quindi si all'inizio dell'iterazione, si scrivono i due figli nell'array se sono nell'albero
//semplicemente ogni volta che scrivo incremento il numero di dove devo scrivere
//vado avanti nello scorrere l'array finché non finisco
//notare che modifico l'array mentre ci sto lavorando
