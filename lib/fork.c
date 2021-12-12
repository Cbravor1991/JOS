// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW 0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//

// El error ocurrió por una lectura si el bit FEC_WR
// está a 0 en utf->utf_err; la dirección está mapeada si y solo sí el bit
// FEC_PR está a 1. Para verificar PTE_COW se debe usar uvpt.

static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.

	// panic("pgfault not implemented");

	pte_t pte = uvpt[(uintptr_t) addr >> PGSHIFT];
	if ((FEC_WR & err) == 0) {
		panic("Error por una lectura");
	}
	if ((FEC_PR & err) == 0) {
		panic("error FEC_PR");
	}
	// PTE_COW, con valor 0x800, toma uno de estos bits como mecanismo de
	// discriminación entre errores por acceso incorrecto a memoria, y
	// errores relacionados con copy-on-write.
	if ((PTE_COW & pte) == 0) {
		panic("error en pgfault");
	}
	// PFTEMP: Used for temporary page mappings for the user page-fault
	// handler (should not conflict with other temporary page mappings)
	envid_t envid = sys_getenvid();
	if ((r = sys_page_alloc(envid, PFTEMP, PTE_P | PTE_U | PTE_W)) < 0)
		panic("sys_page_alloc: %e", r);
	void *rounded_addr = ROUNDDOWN(addr, PGSIZE);
	memmove(PFTEMP, rounded_addr, PGSIZE);
	if ((r = sys_page_map(
	             envid, PFTEMP, envid, rounded_addr, PTE_P | PTE_U | PTE_W)) < 0)
		panic("sys_page_map: %e", r);
	if ((r = sys_page_unmap(0, PFTEMP)) < 0)
		panic("sys_page_unmap: %e", r);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	// panic("duppage not implemented");

	pte_t pte = uvpt[pn];
	void *va = (void *) (pn * PGSIZE);
	envid_t envid_get = sys_getenvid();
	// permisos original
	bool PTE_W_removed = false;
	bool PTE_COW_in_child = false;
	int perm = pte & (PTE_COW | PTE_SYSCALL);
	int child_perm = perm;
	if ((perm & PTE_W) != 0) {
		child_perm = child_perm & ~PTE_W;
		PTE_W_removed = true;
	}
	if (PTE_W_removed) {
		child_perm = child_perm | PTE_COW;
		PTE_COW_in_child = true;
	}
	if (PTE_COW_in_child) {
		if ((r = sys_page_map(envid_get, va, envid, va, child_perm)) < 0)
			panic("sys_page_map: %e", r);
	} else {
		if ((r = sys_page_map(envid_get, va, envid, va, perm)) < 0)
			panic("sys_page_map: %e", r);
	}

	if ((r = sys_page_map(envid_get, va, envid_get, va, child_perm)) < 0)
		panic("sys_page_map: %e", r);


	return 0;
}


static void
dup_or_share(envid_t dstenv, void *va, int perm)
{
	int r;
	// Pagina solo ESCRITURA se crea se crea copia (DUPPAGE EN dumbfork)
	// pero hago el chequeo antes de llamar a dup_or_share para
	// que sea más eficiente
	if ((perm & PTE_W) == PTE_W) {
		if ((r = sys_page_alloc(dstenv, va, PTE_P | PTE_U | PTE_W)) < 0)
			panic("sys_page_alloc: %e", r);
		if ((r = sys_page_map(dstenv, va, 0, UTEMP, PTE_P | PTE_U | PTE_W)) <
		    0)
			panic("sys_page_map: %e", r);
		memmove(UTEMP, va, PGSIZE);
		if ((r = sys_page_unmap(0, UTEMP)) < 0)
			panic("sys_page_unmap: %e", r);

		// LECTURA  =>  page_map
	} else if ((perm & PTE_W) != PTE_W) {
		if ((r = sys_page_map(0, va, dstenv, va, perm)) < 0)
			panic("sys_page_map: %e", r);
	}
}
envid_t
fork_v0()
{
	envid_t envid;
	uint8_t *addr;
	int r;
	envid = sys_exofork();
	if (envid < 0)
		panic("sys_exofork: %e", envid);
	if (envid == 0) {
		// We're the child.
		// The copied value of the global variable 'thisenv'
		// is no longer valid (it refers to the parent!).
		// Fix it and return 0.
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	// We're the parent.
	// Eagerly copy our entire address space into the child.

	// si dire mapeada => dup_or_share
	// sólo se han de copiar el page directory y las page tables en uso.
	for (addr = 0; (int) addr < UTOP; addr += PGSIZE) {
		pde_t pde = uvpd[PDX(addr)];  // page dir
		int perm = PGOFF(pde);        // Da lo mismo sin el PGOFF...
		if (perm & PTE_P) {
			pte_t pte = uvpt[PGNUM(addr)];  // page table entry
			perm = PGOFF(pte);  // Da lo mismo sin el PGOFF...
			if (perm & PTE_P) {
				perm = PTE_SYSCALL & pte;
				dup_or_share(envid, addr, perm);
			}
		}
	}

	// marco hijo
	int error = sys_env_set_status(envid, ENV_RUNNABLE);
	if (error < 0) {
		panic("Couldnt set env status");
	}
	return envid;
}


//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// return fork_v0();
	// panic("fork not implemented");
	envid_t envid;
	uint8_t *addr;
	int r;
	int error;
	// Instalar, en el padre, la función pgfault como manejador de page
	// faults. Esto también reservará memoria para su pila de excepciones.
	set_pgfault_handler(pgfault);
	// Llamar a sys_exofork() y manejar el resultado. En el hijo, actualizar
	// como de costumbre la variable global thisenv.


	envid = sys_exofork();
	if (envid < 0)
		panic("sys_exofork: %e", envid);
	if (envid == 0) {
		// We're the child.
		// The copied value of the global variable 'thisenv'
		// is no longer valid (it refers to the parent!).
		// Fix it and return 0.
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}


	// We're the parent.
	// Eagerly copy our entire address space into the child.

	// si dire mapeada => dup_or_share
	// sólo se han de copiar el page directory y las page tables en uso.
	// Top of one-page user exception stack UXSTACKTOP
	// Iterar sobre el espacio de memoria del padre (desde 0 hasta UTOP) y,
	// para cada página presente, invocar a la función duppage() para mapearla en el hijo. Observaciones:
	for (addr = 0; addr < (uint8_t *) UTOP; addr += PGSIZE) {
		// no se debe mapear la región correspondiente a la pila de excepciones

		/* UTOP,UENVS ------>  +------------------------------+ 0xeec00000
		 * UXSTACKTOP -/       |     User Exception Stack     | RW/RW PGSIZE
		 *                     +------------------------------+ 0xeebff000
		 *                     |       Empty Memory (*)       | --/-- PGSIZE
		 */
		if (addr < (uint8_t *) (UXSTACKTOP - PGSIZE)) {
			pde_t pde = uvpd[PDX(addr)];  // page dir
			int perm = PGOFF(pde);
			if (perm &
			    PTE_P) {  // no verificar ningún PTE cuyo PDE ya indicó que no hay page table presente
				pte_t pte =
				        uvpt[PGNUM(addr)];  // page table entry
				perm = PGOFF(pte);
				if (perm & PTE_P) {
					perm = PTE_SYSCALL & pte;
					duppage(envid, PGNUM(addr));
				}
			}
		}
	}
	// Reservar memoria para la pila de excepciones del hijo, e instalar su manejador de excepciones.
	error = sys_page_alloc(envid, (void *) (UXSTACKTOP - PGSIZE), PTE_SYSCALL);
	if (error < 0) {
		panic("Couldn alloc in fork");
	}

	// seteo en hijo
	error = sys_env_set_pgfault_upcall(envid, thisenv->env_pgfault_upcall);
	if (error < 0) {
		panic("Couldnt set pgfault");
	}

	// marco hijo

	error = sys_env_set_status(envid, ENV_RUNNABLE);
	if (error < 0) {
		panic("Couldnt set env status");
	}
	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
