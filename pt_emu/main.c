/* A multi-level page table system emulator */

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define FRAME_SIZE 	4096
#define PAGE_SIZE 	FRAME_SIZE
#define RAM_SIZE 	(1024 * PAGE_SIZE)

typedef uint64_t	pte_t;
typedef pte_t		*pt_t;

#define PTE_V (1L << 0)	// Valid
#define PTE_R (1L << 1) // Read
#define PTE_W (1L << 2) // Write
#define PTE_X (1L << 3) // Execute

#define PX(level, va) ((((uint64_t)va) >> (12 + 9*(level))) & 0x1FF)

#define PTE2PA(pte) (((pte) >> 10) << 12)
#define PA2PTE(pa) ((((uint64_t)pa) >> 12) << 10)

struct page {
	struct page *next;
};

struct {
	struct page *fl;
} rt;

void   kfree(void *pa);
void   kinit(char *ram, size_t size);
void  *kalloc(void);
pte_t *walk(pt_t pt, uint64_t va, int alloc);
int    mappages(pt_t pt, uint64_t va, uint64_t pa, uint64_t perm);

int main(void)
{
	char ram[RAM_SIZE] __attribute__((aligned(PAGE_SIZE)));
	kinit(ram, RAM_SIZE);
	
	pt_t root = (pt_t)kalloc();
	printf("root page table at: %p\n", root);

	uint64_t va = 0x800000;
	uint64_t pa = 0x1000;

	mappages(root, va, pa, PTE_R | PTE_W);

	pte_t *pte = walk(root, va, 0);

	if (pte && (*pte & PTE_V)) {
		uint64_t r_pa = PTE2PA(*pte);
		printf("verification: va 0x%lx translates to pa 0x%lx\n",
				va, r_pa);

	} else {
		printf("verification failed\n");
	}

	return 0;
}

void kinit(char *ram, size_t size)
{
	rt.fl = NULL;

	char *p = (char *)ram;

	for (size_t i = 0; i + PAGE_SIZE <= size; i += PAGE_SIZE)
		kfree(p + i);		
}

void kfree(void *pa)
{
	struct page *p = (struct page *)pa;

	p->next = rt.fl;
	rt.fl = p;
}

void *kalloc(void)
{
	struct page *p = rt.fl;

	if (p) {
		rt.fl = p->next;
		memset(p, 0, PAGE_SIZE);
	}

	return (void *)p;
}

pte_t *walk(pt_t pt, uint64_t va, int alloc)
{
	for (int l = 2; l > 0; l--) {
		int idx = PX(l, va);
		pte_t *pte = &pt[idx];
		
		if (*pte & PTE_V) {
			pt = (pt_t)PTE2PA(*pte);
		} else {
			if (!alloc || (pt = (pt_t)kalloc()) == NULL)
				return NULL;

			*pte = PA2PTE(pt) | PTE_V;
		}
	}

	return &pt[PX(0, va)];
}


int mappages(pt_t pt, uint64_t va, uint64_t pa, uint64_t perm)
{
	pte_t *pte = walk(pt, va, 1);
	
	if (pte == NULL)
		return -1;

	if (*pte & PTE_V)
		return -1;

	*pte = PA2PTE(pa) | perm | PTE_V;

	return 0;
}
