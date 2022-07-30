#ifndef _SM_MEM_H_
#define _SM_MEM_H_

#include<stdio.h>
#include<stdint.h>
#include<malloc.h>

#define MY_ASSERT(_para_)	do{	if (!_para_) {printf("Mem init fail!\n");while(1);}}while(0);

typedef struct mem_m{
	uint16_t magic;
	uint16_t used;
	size_t mem_size;
	struct mem_m* next;
	struct mem_m* prev;
}mem_manage;

extern void sm_mempool_init(size_t memsize);
extern void* sm_malloc(size_t size);
extern void sm_free(void* _Block);
extern void* sm_calloc(size_t count, size_t size);
extern void sm_printf(void);

#endif //_SM_MEM_H_
