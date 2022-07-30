#ifndef _SM_MEM_H_
#define _SM_MEM_H_

#include<stdio.h>
#include<stdint.h>
#include<malloc.h>

typedef void* (*malloc_hook)(size_t );

typedef struct mem_m{
	uint16_t magic;
	uint16_t used;
	size_t mem_size;
	struct mem_m* next;
	struct mem_m* prev;
}mem_manage;

typedef struct {
	mem_manage *head;
	size_t free_mem;
	//TODO
}mempool_control;

void sm_mempool_init(size_t memsize);
void* sm_malloc(size_t size);
void sm_free(void* _Block);
void* sm_calloc(size_t count, size_t size);

#endif //_SM_MEM_H_
