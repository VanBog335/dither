/*
	vanbog_memory_manager -- v0.7.4 -- https://github.com/VanBog335
	to use, do this:
	```c
	#define VB_MM_IMPLEMENTATION
	#include "vb_mm.h"
	```
	yes, like in stb libs

	example:
	```c
	#define VB_MM_IMPLEMENTATION
	#include "vb_mm.h"

	int main()
	{
		vb_da_ptr_init();

		char *a = vb_alloc(16);
		a = vb_realloc(a, 8);
		vb_free(a);

		vb_da_ptr_destroy();
		return 0;
	}
	```
*/
#ifndef VB_MM_H_
#define VB_MM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef VB_MEMALLOC
#include <stdlib.h>
#define VB_MEMALLOC malloc
#endif /* VB_MEMALLOC */

#ifndef VB_MEMREALLOC
#include <stdlib.h>
#define VB_MEMREALLOC realloc
#endif /* VB_MEMREALLOC */

#ifndef VB_MEMFREE
#include <stdlib.h>
#define VB_MEMFREE free
#endif /* VB_MEMFREE */


#define VB_SIZE_MULTIPLIER 2
#define VB_INITIAL_ARRAY_SIZE 8

void *vb_alloc(size_t size);
void *vb_realloc(void *p, size_t newsize);
void vb_free(void *p);
void vb_freeall();
void vb_da_ptr_init();
void vb_da_ptr_destroy();
void vb_printPoints();

#endif /* VB_MM_H_ */


#ifdef VB_MM_IMPLEMENTATION

void **vb_PointArray;
size_t vb_array_size = VB_INITIAL_ARRAY_SIZE;
size_t vb_current_size = 0;

void *vb_alloc(size_t size)
{
	void *p = VB_MEMALLOC(size);
	if (vb_current_size >= vb_array_size) {
		vb_array_size *= VB_SIZE_MULTIPLIER;
		vb_PointArray = VB_MEMREALLOC(vb_PointArray, vb_array_size * sizeof(void *));
		for (size_t i = (vb_array_size/VB_SIZE_MULTIPLIER); i < vb_array_size; i++) {
			vb_PointArray[i] = NULL;
		}
	}
	vb_PointArray[vb_current_size++] = p;
	return p;
}

void *vb_realloc(void *p, size_t newsize)
{
	void *oldp = p;
	void *newp = VB_MEMREALLOC(p, newsize);

	for (size_t i = 0; i < vb_array_size; i++){
		if (vb_PointArray[i] == oldp){
			vb_PointArray[i] = newp;
			break;
		}
	}

	return newp;
}

void vb_free(void *p)
{
	for (size_t i = 0; i < vb_current_size; i++) {
		if (vb_PointArray[i] == p) {
			vb_current_size--;
			vb_PointArray[i] = vb_PointArray[vb_current_size]; // Replace with last element
			vb_PointArray[vb_current_size] = NULL;
			break;
		}
	}
	VB_MEMFREE(p);
}

void vb_freeall()
{
	for (size_t i = 0; i < vb_array_size; i++){
		if (vb_PointArray[i] != 0){
			VB_MEMFREE(vb_PointArray[i]);
			vb_PointArray[i] = NULL;
			vb_current_size--;
		}
	}
}

void vb_da_ptr_init() {
	vb_PointArray = VB_MEMALLOC(vb_array_size * sizeof(void *));
	for (size_t i = 0; i < vb_array_size; i++) {
		vb_PointArray[i] = NULL;
	}
}

void vb_da_ptr_destroy() {
	vb_freeall();
	VB_MEMFREE(vb_PointArray);
}

void vb_printPoints()
{
	printf("----------------\n");
	for (size_t i = 0; i < vb_array_size; i++){
		printf("%zX. %p\n", i, vb_PointArray[i]);
	}
	printf("----------------\n");
}

#endif /* VB_MM_IMPLEMENTATION */
