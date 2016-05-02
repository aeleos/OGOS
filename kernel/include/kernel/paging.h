#ifndef PAGING_H
#define PAGING_H


#include <kernel/system.h>

extern page_directory_t *current_directory;


/**
   Sets up the environment, page directories etc and
   enables paging.
**/
void initialise_paging(uint32_t memorySize);

/**
   Causes the specified page directory to be loaded into the
   CR3 register.
**/
void switch_page_directory(page_directory_t *new);

/**
   Retrieves a pointer to the page required.
   If make == 1, if the page-table in which this page should
   reside isn't created, create it!
**/
page_t *get_page(uint32_t address, uint32_t make, page_directory_t *dir);

/**
   Handler for page faults.
**/
void page_fault(registers_t *regs);

/**
   Makes a copy of a page directory.
**/
page_directory_t *clone_directory(page_directory_t *src);


/**
    Pages memory at a given location
**/
void pageMem(uint32_t location);

/**
    Maps virtual pages
**/
void virtual_map_pages(long addr, long size, uint32_t rw, uint32_t user);

#endif //PAGING_H
