#include "task.h"
#include <memory.h>
#include <printk.h>
#include <string.h>
#include <x86.h>

#define PHYSICAL_POOL_PAGES 64
#define PHYSICAL_POOL_BYTES (PHYSICAL_POOL_PAGES << 12)
#define BITSET_SIZE (PHYSICAL_POOL_PAGES >> 6)



#define PTE_FLAG_VALID      	0x1
#define PTE_FLAG_RW           	0x2
#define PTE_FLAG_USER      	0x4
#define PTE_FLAG_CACHED 	0x8
#define PTE_FLAG_RAM    	0x10
#define PTE_FLAG_ACCESSED      	0x20
#define PTE_FLAG_DIRTY  	0x40
#define PTE_FLAG_HUGE   	0x80
#define PTE_FLAG_GLOBAL 	0x100 // pas sur de ca
#define PTE_FLAG_NO_EXECUTE    	0x200

// pte flags masks
#define PTE_IS_VALID(p)    	((p) & PTE_FLAG_VALID)
#define PTE_IS_RW(p)      	((p) & PTE_FLAG_RW)
#define PTE_IS_USER(p)   	((p) & PTE_FLAG_USER)
#define PTE_IS_CACHED(p)	((p) & PTE_FLAG_CACHED)
#define PTE_IS_RAM(p)		((p) & PTE_FLAG_RAM)
#define PTE_IS_ACCESSED(p)	((p) & PTE_FLAG_ACCESSED)
#define PTE_IS_DIRTY(p)		((p) & PTE_FLAG_DIRTY)
#define PTE_IS_HUGE(p)		((p) & PTE_FLAG_HUGE)
#define PTE_IS_GLOBAL(p)	((p) & PTE_FLAG_GLOBAL) // pas sur de ca
#define PTE_IS_NO_EXECUTE(p)	((p) & PTE_FLAG_NO_EXECUTE)

// pte addr makss
#define _PTE_ADDR_MASK		0xffffffff000
#define PTE_NEXT_ADDR(p)	((p) & _PTE_ADDR_MASK)

#define _PTE_MASK_PML1 0x1ff000
#define _PTE_MASK_PML2 0x3fe00000
#define _PTE_MASK_PML3 0x7fc0000000
#define _PTE_MASK_PML4 0xff8000000000

#define PTE_GET_INDEX_PML1(v) (((v) & _PTE_MASK_PML1) >> 12)
#define PTE_GET_INDEX_PML2(v) (((v) & _PTE_MASK_PML2) >> 21)
#define PTE_GET_INDEX_PML3(v) (((v) & _PTE_MASK_PML3) >> 30)
#define PTE_GET_INDEX_PML4(v) (((v) & _PTE_MASK_PML4) >> 39)
#define PTE_GET_INDEX_FOR_LVL(v, lvl) ((v) >> (12 + (9 * ((lvl) - 1))) & 0x1ff)

extern __attribute__((noreturn)) void die(void);

static uint64_t bitset[BITSET_SIZE];

static uint8_t pool[PHYSICAL_POOL_BYTES] __attribute__((aligned(0x1000)));

paddr_t alloc_page(void) {
  size_t i, j;
  uint64_t v;

  for (i = 0; i < BITSET_SIZE; i++) {
    if (bitset[i] == 0xffffffffffffffff)
      continue;

    for (j = 0; j < 64; j++) {
      v = 1ul << j;
      if (bitset[i] & v)
        continue;

      bitset[i] |= v;
      return (((64 * i) + j) << 12) + ((paddr_t)&pool);
    }
  }

  printk("[error] Not enough identity free page\n");
  asm volatile("hlt");
  return 0;
}

void free_page(paddr_t addr) {
  paddr_t tmp = addr;
  size_t i, j;
  uint64_t v;

  tmp = tmp - ((paddr_t)&pool);
  tmp = tmp >> 12;

  i = tmp / 64;
  j = tmp % 64;
  v = 1ul << j;

  if ((bitset[i] & v) == 0) {
    printk("[error] Invalid page free %p\n", addr);
    die();
  }

  bitset[i] &= ~v;
}

void print_pgt(paddr_t pml, uint8_t lvl) {
  if (lvl == 0) {
    return;
  }
  if (lvl == 4) {
    printk("> print_pgt: cr3: %p\n", pml);
  }

  paddr_t *p = (paddr_t *)pml;
  for (int i = 0; i < PAGE_NB_ENTRIES; i++) {
    paddr_t entry = p[i];
    paddr_t page_addr = PAGE_ADDRESS(entry);
    uint8_t is_last = i == PAGE_NB_ENTRIES - 1 || !PAGE_IS_VALID(p[i + 1]);

    if (PAGE_IS_VALID(entry)) {
      for (int i = 4; i > lvl; i--) {
        if (!is_last && i == lvl + 1) {
          printk("  | ");
        } else {
          printk("    ");
        }
      }
      printk("  level %d, index %d, entry: %p\n", lvl, i, entry);
      if (!PAGE_IS_HUGE(entry)) {
        print_pgt(page_addr, lvl - 1);
      }
    }
  }
}

void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr) {
  paddr_t *page_table = (paddr_t *)ctx->pgt;
  paddr_t i_pml[4] = {
      PAGE_INDEX_PML1(vaddr),
      PAGE_INDEX_PML2(vaddr),
      PAGE_INDEX_PML3(vaddr),
      PAGE_INDEX_PML4(vaddr),
  };

  // printk("indexes of vaddr %p: %d, %d, %d, %d\n", vaddr, i_pml[3], i_pml[2],
        //  i_pml[1], i_pml[0]);
  paddr_t entry_index;

  // printk("> map_page: cr3: %p, vaddr: %p, paddr: %p,\n", page_table, vaddr,
  //        paddr);
  // printk("  i_pml4: %d, i_pml3: %d, i_pml2: %d, i_pml1: %d\n", i_pml[3],
  //        i_pml[2], i_pml[1], i_pml[0]);

  for (int lvl = 3; lvl > 0; lvl--) {
    entry_index = i_pml[lvl];
    // printk("level=%d, entry_index=%d\n", lvl, entry_index);
    if (!PAGE_IS_VALID(page_table[entry_index])) {
      // printk("new page allocation for pml%d\n", lvl);
      paddr_t page = alloc_page();
      memset((void *)page, 0, PAGE_SIZE);
      page_table[entry_index] =
          page | PAGE_FLAG_USER | PAGE_FLAG_WRITE | PAGE_FLAG_VALID;
      // printk("  alloc of pml%d, in index %d of pml%d with address %p\n", lvl,
      //        entry_index, lvl + 1, page);
    }
    page_table = (paddr_t *)PAGE_ADDRESS(page_table[entry_index]);
  }

  entry_index = i_pml[0];
  if (!PAGE_IS_VALID(page_table[entry_index])) {
    page_table[entry_index] =
        paddr | PAGE_FLAG_USER | PAGE_FLAG_WRITE | PAGE_FLAG_VALID;
  } else {
    printk("  [error] virtual address already mapped\n");
    asm volatile("hlt");
  }
}

void load_task(struct task *ctx)
{
	/* On se trouve dans une nouvelle tache, il faut allouer pgt */
	paddr_t new_pml4 = alloc_page();
	memset((void *)new_pml4, 0, PAGE_SIZE);
	ctx->pgt = new_pml4;

	/* Pour mapper noyau, on a besoin de creer pml3
	 * car on a juste de copier pml3[0] du parent
	 * TODO check user
	*/
	paddr_t pml3 = alloc_page();
	memset((void *)pml3, 0, PAGE_SIZE);
	((paddr_t *)new_pml4)[0] = (paddr_t)pml3 | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;

	/* A partir de la, on a new_pml4[0] -> new_pml3[0], 
	 * on peut copier la pml2 du kernel.
	*/
	/* On recupere la pgt du processus courrant */
	paddr_t *kernel_pml4 = (paddr_t *)store_cr3();
	paddr_t *kernel_pml3 = (paddr_t *)PTE_NEXT_ADDR(kernel_pml4[0]);
	((paddr_t *)pml3)[0] = kernel_pml3[0];

	/* La partie setup pgt est terminee, il faut maintenant
	 * allouer.
	*/

	/* On itere sur toutes la partie load_end_paddr - load_paddr */
	vaddr_t vaddr = ctx->load_vaddr;
	paddr_t paddr = ctx->load_paddr;

	for(; paddr < ctx->load_end_paddr; paddr+=PAGE_SIZE) {
		map_page(ctx, vaddr, paddr);
		vaddr+=PAGE_SIZE;
	}


	printk("before bss alloc\n");
	/* A ce moment, vaddr = bss_start */
	for(; vaddr < ctx->bss_end_vaddr; vaddr+=PAGE_SIZE) {
		paddr_t new_page = alloc_page();
		memset((void *)new_page, 0, PAGE_SIZE);
		map_page(ctx, vaddr, new_page);
	}
	

	printk("!!Task loaded: load_vaddr=%p, load_end_vaddr=%p, bss_end_vaddr=%p\n",
		ctx->load_vaddr, vaddr,  ctx->bss_end_vaddr);
}

// void load_task(struct task *ctx) {
//   // printk("> load_task: pgt: %p, load_vaddr: %p, bss_end_vaddr: %p\n",
//   // ctx->pgt,
//   //        ctx->load_vaddr, ctx->bss_end_vaddr);
//   // printk("  load_paddr: %p, load_end_paddr: %p\n", ctx->load_paddr,
//   //        ctx->load_end_paddr);

//   // allocate process' page table
//   paddr_t pml4_page = alloc_page();
//   memset((void *)pml4_page, 0, PAGE_SIZE);
//   ctx->pgt = pml4_page;
//   // printk("  alloc pml4, pgt: %p\n", ctx->pgt);

//   // allocate process' pml3
//   paddr_t pml3_page = alloc_page();
//   memset((void *)pml3_page, 0, PAGE_SIZE);
//   ((paddr_t *)pml4_page)[0] =
//       pml3_page | PAGE_FLAG_USER | PAGE_FLAG_WRITE | PAGE_FLAG_VALID;

//   // set pml3[0] with parent pml3[0], mapping kernel space
//   paddr_t *parent_pml4 = (paddr_t *)store_cr3();
//   paddr_t *parent_pml3 = (paddr_t *)PAGE_ADDRESS(parent_pml4[0]);
//   ((paddr_t *)pml3_page)[0] = parent_pml3[0];

//   // map task memory
//   // printk("  indices of vaddr %p: %d, %d, %d, %d\n", ctx->load_vaddr,
//   //        PAGE_INDEX_PML4(ctx->load_vaddr), PAGE_INDEX_PML3(ctx->load_vaddr),
//   //        PAGE_INDEX_PML2(ctx->load_vaddr), PAGE_INDEX_PML1(ctx->load_vaddr));
//   paddr_t vaddr = ctx->load_vaddr;
//   paddr_t paddr = ctx->load_paddr;

//   // map process payload (text, data) from the physical memory
//   for (; paddr < ctx->load_end_paddr; paddr += PAGE_SIZE) {
//     map_page(ctx, vaddr, paddr);
//     vaddr += PAGE_SIZE;
//   }

//   // map remaining unmapped virtual addresses till bss end
//   for (; vaddr < ctx->bss_end_vaddr; vaddr += PAGE_SIZE) {
//     mmap(ctx, vaddr);
//   }
// }

void set_task(struct task *ctx) { load_cr3(ctx->pgt); }

void mmap(struct task *ctx, vaddr_t vaddr) {
  // printk("> mmap: pgt: %p, vaddr: %p\n", ctx->pgt, vaddr);
  paddr_t page = alloc_page();
  memset((void *)page, 0, PAGE_SIZE);
  map_page(ctx, vaddr, page);
}

void munmap(struct task *ctx, vaddr_t vaddr) {
  // printk("> munmap: pgt: %p, vaddr: %p\n", ctx->pgt, vaddr);
  paddr_t *page_table = (paddr_t *)ctx->pgt;
  paddr_t i_pml[4] = {
      PAGE_INDEX_PML1(vaddr),
      PAGE_INDEX_PML2(vaddr),
      PAGE_INDEX_PML3(vaddr),
      PAGE_INDEX_PML4(vaddr),
  };

  for (int lvl = 4; lvl > 1; lvl--) {
    paddr_t entry = page_table[i_pml[lvl - 1]];
    if (!PAGE_IS_VALID(entry)) {
      printk("  [error] munmap walk, pml%d page not valid\n", lvl);
      return;
    }
    page_table = (paddr_t *)PAGE_ADDRESS(entry);
  }

  paddr_t entry = page_table[i_pml[0]];
  if (!PAGE_IS_VALID(entry)) {
    printk("  [error] munmap freeing invalid page %p in pml1 %p index %d\n",
           PAGE_ADDRESS(entry), page_table, i_pml[0]);
    return;
  }

  free_page(PAGE_ADDRESS(entry));
  page_table[i_pml[0]] = 0x0;
  // TLB invalidation
  invlpg(vaddr);
}

void pgfault(struct interrupt_context *ctx) {
  paddr_t page_fault_addr = store_cr2();
  // printk("> page fault: rip: %p, cr2: %p, cr3: %p\n", ctx->rip,
  // page_fault_addr,
  //        store_cr3());

  // address not in the stack segment: segmentation fault
  if (page_fault_addr < STACK_BEGIN || page_fault_addr > STACK_END) {
    printk("  [error] segmentation fault, addr outside stack bounds\n");
    exit_task(ctx);
    asm volatile("hlt");
  }

  // lazy allocation of one stack page
  mmap(current(), page_fault_addr);
}

void duplicate_task(struct task *ctx) {}
