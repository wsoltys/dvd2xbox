#include <malloc.h>
#include <string.h>
#include "handler.h"

struct memory_stats MS={0,0,0,0,0,0};
int first = 1;

char *malloc2( int size )
{
	struct memory_header *mh;
	struct memory_block *mb;

	if( first )
	{
		first = 0;

		memset( &MS, 0, sizeof( struct memory_stats ));
		MS.last.prev = &MS.first;
		MS.first.next = &MS.last;
		MS.first.prev = 0;
		MS.last.next = 0;
		
	}
	mh = (struct memory_header *)malloc( sizeof( struct memory_header ) );
	mb = (struct memory_block *)malloc( sizeof( struct memory_block ) );
	mb->memory = (char *)malloc( size );
	mh->memory = mb->memory;
	mh->mb = mb;
	mb->mh = mh;

	mh->size = size;
	MS.currently_allocated += size;
	MS.total_alloced += size;

	mh->next = &MS.last;
	mh->prev = MS.last.prev;

	MS.last.prev->next = mh;
	MS.last.prev = mh;

	mb->crc1 = 0x1234ABCD;
	mb->crc2 = 0x1234ABCD;
	return mb->memory;
}

static struct memory_header *MEM_LocateHeaderFromMemory( char *loc )
{
	struct memory_header *mh;

	mh = MS.first.next;

	while( mh )
	{
		if( mh->memory == loc )
			return mh;
		mh = mh->next;
	}
	return 0;
}

char free2( char *loc )
{
	struct memory_block *mb;
	struct memory_header *mh;

	if( !loc )
		return 1;
	mh = MEM_LocateHeaderFromMemory( loc );
	if( !mh )
		return 0;
	mb = mh->mb;
	if( (mb->crc1 != 0x1234ABCD) && (mb->crc2 != 0x1234ABCD) )
	{
		MS.destroyed_blocks ++;
		MS.size_of_destroyed_memory += mh->size;
		return 1;
		//  THIS MEANS MEMORY WAS DESTROYED.
	}
	mh = mb->mh;
	MS.currently_allocated -= mh->size;
	MS.total_freed += mh->size;
	free( mb->memory );


	// fix lists
	mh->prev->next = mh->next;
	mh->next->prev = mh->prev;

	free( (char*)mh );
	free( (char*)mb );
	return 0;
}

int MEM_Allocated( void )
{
	return MS.currently_allocated;
}

int MEM_Freed( void )
{
	return MS.total_freed;
}

int MEM_Destroyed( void )
{
	return MS.destroyed_blocks;
}

int MEM_DestroyedSize( void )
{
	return MS.size_of_destroyed_memory;
}

int MEM_Verify( void )
{
	struct memory_block *mb;	
	struct memory_header *mh;	

	mh = MS.first.next;

	MS.size_of_destroyed_memory = 0;
	MS.destroyed_blocks = 0;
	while( mh->next )
	{
		mb = mh->mb;
		if( (mb->crc1 != 0x1234ABCD) && (mb->crc2 != 0x1234ABCD) )
		{
			MS.destroyed_blocks ++;
			MS.size_of_destroyed_memory += mh->size;
		}
		mh = mh->next;
	}
	return MS.size_of_destroyed_memory;
}

int MEM_FreeAllMemory( void )
{
	struct memory_header *mh;
	struct memory_header *mh2;


	MS.size_of_destroyed_memory = 0;
	MS.destroyed_blocks = 0;
	mh = MS.first.next;
	while( mh->next )
	{
		mh2 = mh->next;
		free2( mh->memory );
		mh = mh2;
	}
	return 0;
}