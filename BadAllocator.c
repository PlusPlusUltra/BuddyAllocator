int levelIdx(size_t idx){
return (int)floor(log2(idx));
};
int buddyIdx(int idx){
if (idx&0x1){
return idx-1;
}
return idx+1;
}
int parentIdx(int idx){
return idx/2;
}
int startIdx(int idx){
return (idx-(1<<levelIdx(idx)));
}
typedef struct BuddyListItem {
ListItem list;
int idx;
// tree index
int level; // level for the buddy
char* start; // start of memory
int size;
struct BuddyListItem* buddy_ptr;
struct BuddyListItem* parent_ptr;
} BuddyListItem;
typedef struct {
ListHead free[MAX_LEVELS];
ListHead occupied[MAX_LEVELS];
int num_levels;
PoolAllocator list_allocator;
the memory area to be managed
char* memory; //
// the minimum page of RAM that can be returned
int min_bucket_size;
} BuddyAllocator;
//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc,
int size) {
// calculate max mem
int mem_size=
(1<<alloc->num_levels)*alloc->min_bucket_size;
//calculate level for page
int level=floor(log2(mem_size/(size+8)));
// if the level is too small, we pad it to max
if (level>alloc->num_levels)
level=alloc->num_levels;
printf("requested: %d bytes, level %d \n",
size, level);
// we get a buddy of that size;
BuddyListItem* buddy=
BuddyAllocator_getBuddy(alloc, level);
if (! buddy)
return 0;
// we write in the memory
region managed the buddy address
BuddyListItem** target=
(BuddyListItem**)(buddy->start);
*target=buddy;
return buddy->start+8;
}
BuddyListItem* BuddyAllocator_getBuddy(
BuddyAllocator* alloc, int level){
if (level<0)
return 0;
if (! alloc->free[level].size ) {
// no buddies on this level
BuddyListItem* parent_ptr=
BuddyAllocator_getBuddy(alloc, level-1);
if (! parent_ptr)
return 0;
// parent already detached from free list
int left_idx=parent_ptr->idx<<1;
int right_idx=left_idx+1;
BuddyListItem* left_ptr=
BuddyAllocator_createListItem(alloc,
left_idx,
parent_ptr);
BuddyListItem* right_ptr=
BuddyAllocator_createListItem(alloc,
right_idx,
parent_ptr);
// we need to update the buddy ptrs
left_ptr->buddy_ptr=right_ptr;
right_ptr->buddy_ptr=left_ptr;
}
// we detach the first
if(alloc->free[level].size) {
BuddyListItem* item=
(BuddyListItem*)
List_popFront(alloc->free+level);
return item;
}
return 0;
}
void BuddyAllocator_releaseBuddy(
BuddyAllocator* alloc,
BuddyListItem* item){
BuddyListItem* parent_ptr=item->parent_ptr;
BuddyListItem *buddy_ptr=item->buddy_ptr;
// buddy back in the free list of its level
List_pushFront(&alloc->free[item->level],
(ListItem*)item);
// if on top of the chain, do nothing
if (! parent_ptr)
return;
// if the buddy of this item is not free,
// we do nothing
if (buddy_ptr->list.prev==0 &&
buddy_ptr->list.next==0)
return;
//join
//1. we destroy the two buddies in the free list;
printf("merge %d\n", item->level);
BuddyAllocator_destroyListItem(alloc, item);
BuddyAllocator_destroyListItem(alloc, buddy_ptr);
//2. we release the parent
BuddyAllocator_releaseBuddy(alloc, parent_ptr);
//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc,
void* mem) {
printf("freeing %p", mem);
// we retrieve the buddy from the system
char* p=(char*) mem;
p=p-8;
BuddyListItem** buddy_ptr=(BuddyListItem**)p;
BuddyListItem* buddy=*buddy_ptr;
//printf("level %d", buddy->level);
// sanity check;
assert(buddy->start==p);
BuddyAllocator_releaseBuddy(alloc, buddy);
// initializes the buddy allocator,
and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
int num_levels,
char* buffer,
int buffer_size,
char* memory,
int min_bucket_size);
//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc,
int size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
