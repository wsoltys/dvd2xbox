#define MEMORY_HEADER ( sizeof(struct memory_block) + sizeof(int)  )


char *malloc2( int size );
char free2( char *loc );
int MEM_Allocated( void );
int MEM_Destroyed( void );
int MEM_DestroyedSize( void );
int MEM_Verify( void );
int MEM_Freed( void );
int MEM_FreeAllMemory( void );


struct memory_header
{
	struct memory_header	*next;
	struct memory_header	*prev;
	int						size;
	int						*called_from;
	struct memory_block		*mb;
	char					*memory;
};

struct memory_block
{
	struct memory_header	*mh;
	int						crc1;
	char					*memory;
	int						crc2;
};

struct memory_stats
{
	struct memory_header	first;
	struct memory_header	last;
	int						total_alloced;
	int						total_freed;
	int						currently_allocated;
	int						destroyed_blocks;
	int						size_of_destroyed_memory;
};
