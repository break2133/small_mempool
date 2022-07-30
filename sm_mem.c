#include "sm_mem.h"

static mempool_control mem_con;

void sm_mempool_init(size_t memsize)
{
    mem_manage* lfree = NULL;
	size_t maxsize = (size_t) - 1;
	if (memsize > maxsize - 2 * sizeof(mem_manage))
		return;
	size_t pollsize = memsize + 2 * sizeof(mem_manage);
	void * pool = malloc(pollsize);
	if (!pool)
        return;

	lfree = (mem_manage*)pool;
	lfree->magic = 0xdea0;
	lfree->mem_size = pollsize - 2 * sizeof(mem_manage);
	lfree->next = (mem_manage*)((size_t)lfree + lfree->mem_size + sizeof(mem_manage));
	lfree->next->prev = lfree;
	lfree->next->mem_size = 0;
	lfree->next->next = NULL;
	lfree->next->magic = 0xE00D;	//"E00D" mean end
	lfree->next->used = 1;
	lfree->prev = NULL;
	lfree->used = 0;
	mem_con.head = lfree;
}

void* sm_malloc(size_t size)
{
	if (!size)
		return NULL;

    mem_manage* lfree = NULL;

	for (lfree = mem_con.head; lfree; lfree=lfree->next) {
		if (size + sizeof(mem_manage) <= lfree->mem_size && !lfree->used) {
			mem_manage* new_mem;
			mem_manage* new_mem2;
			mem_manage* lfree_next;
			
			new_mem = lfree; 
			lfree_next = lfree->next;
			new_mem->magic = 0xdea0;
			new_mem->mem_size = size;
			new_mem->next = (mem_manage*)((size_t)new_mem + size + sizeof(mem_manage));	
			new_mem->prev = lfree->prev;
			new_mem->used = 1;

			new_mem2 = new_mem->next;
			new_mem2->magic = 0xdea0;
			new_mem2->mem_size = ((size_t)lfree_next - (size_t)new_mem2 - sizeof(mem_manage));
			new_mem2->next = lfree_next;
			new_mem2->next->prev = new_mem2;
			new_mem2->prev = new_mem;
			new_mem2->used = 0;

			return (mem_manage*)((size_t)new_mem + sizeof(mem_manage));
		}
	}
	return NULL;
}

void* sm_calloc(size_t count, size_t size)
{
	void* p;
	p = sm_malloc(count * size);
	if (p)
		sm_memset(p, 0, count * size);
	else
		p = NULL;

	return p;
}

void sm_free(void* _Block)
{
	if (!_Block)
		return;

	mem_manage* free_p = (mem_manage*)((size_t)_Block - sizeof(mem_manage));

	free_p->used = 0;

	if (free_p->prev !=NULL) {
		if (free_p->prev->used == 0) {
			free_p->prev->mem_size += free_p->mem_size + sizeof(mem_manage);
			free_p->prev->next = free_p->next;
			free_p->next->prev = free_p->prev;
			if (free_p->next != NULL) {
				if (free_p->next->used == 0) {
					free_p->prev->mem_size += free_p->next->mem_size + sizeof(mem_manage);
					free_p->prev->next = free_p->next->next;
					free_p->next->next->prev = free_p->prev;
				}
			}
		}
		else
		{
			if (free_p->next != NULL) {
				if (free_p->next->used == 0) {
					free_p->mem_size += free_p->next->mem_size + sizeof(mem_manage);
					free_p->next->next->prev = free_p;
					free_p->next = free_p->next->next;
				}
			}
		}
	}
	else {
		if (free_p->next != NULL) {
			if (free_p->next->used == 0) {
				free_p->mem_size += free_p->next->mem_size + sizeof(mem_manage);
				free_p->next = free_p->next->next;
				free_p->next->prev = free_p;
			}
		}
	}
}
