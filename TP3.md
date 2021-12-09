# Tarea: envid2env

envid2env devuelve el currenv env si envid es igual a cero. De esta manera en sys_env_destroy se va a imprimir "...exiting gracefully" y destruye el current environment.
</br>

# Tarea: dumbfork
</br>
1. No se preserva el flag de solo lectura. Internamente duppage llama a sys_page_alloc con los flags PTE_P|PTE_U|PTE_W
2. </br>

```

extern volatile pte_t uvpt[];     // VA of "virtual page table"
extern volatile pde_t uvpd[];     // VA of current page directory

```

</br>

```

// A linear address 'la' has a three-part structure as follows:
//
// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(la) --/ \--- PTX(la) --/ \---- PGOFF(la) ----/
//  \---------- PGNUM(la) ----------/
//
// The PDX, PTX, PGOFF, and PGNUM macros decompose linear addresses as shown.
// To construct a linear address la from PDX(la), PTX(la), and PGOFF(la),
// use PGADDR(PDX(la), PTX(la), PGOFF(la)).

// page number field of address
#define PGNUM(la)	(((uintptr_t) (la)) >> PTXSHIFT)

```

</br>

```

envid_t dumbfork(void) {
    // ...
    for (addr = UTEXT; addr < end; addr += PGSIZE) {
        bool readonly;
        //
        // TAREA: dar valor a la variable readonly
        //
        pte_t pte = uvpt[PGNUM(addr)];
        //si da == 0 entonces solo lectura
        int res = pte & PTE_W
        read_only = !res;
        duppage(envid, addr, readonly);
    }

```

</br>

3. 

original
</br>
```

void duppage(envid_t dstenv, void *addr, bool readonly) {
    // Código original (simplificado): tres llamadas al sistema.
    sys_page_alloc(dstenv, addr, PTE_P | PTE_U | PTE_W);
    sys_page_map(dstenv, addr, 0, UTEMP, PTE_P | PTE_U | PTE_W);

    memmove(UTEMP, addr, PGSIZE);
    sys_page_unmap(0, UTEMP);

    // Código nuevo: una llamada al sistema adicional para solo-lectura.
    if (readonly) {
        sys_page_map(dstenv, addr, dstenv, addr, PTE_P | PTE_U);
    }
}

```

</br>

Modificado 

</br>

```

void duppage(envid_t dstenv, void *addr, bool readonly) {
    // Código original (simplificado): tres llamadas al sistema.
    sys_page_alloc(dstenv, addr, PTE_P | PTE_U | PTE_W);
    sys_page_map(dstenv, addr, 0, UTEMP, read_only ? PTE_P | PTE_U : PTE_P | PTE_U | PTE_W);

    memmove(UTEMP, addr, PGSIZE);
    sys_page_unmap(0, UTEMP);

    
}

```

</br>

# Tarea multicore_init

</br>

1. copia el codigo en mpentry.S. La direccion de destino es code, que es el resultado de `KADDR(MPENTRY_PADDR)` que es lo mismo que `KADDR(0x7000)` o `0x7000 + KERNBASE` que resulta en la irección virtual `0xf0007000`. 0x7000 era física (KADDR toma una fisica).

2. Se usa para indicarle a mpentry.S qué stack debe usar (cada CPU tendrá uno). En boot_aps cada CPU/core tendrá un stack pointer distinto, mientras que en bootstack todos tendrian el mismo

3. En el registro eip estará el puntero al tope de la pila 0x7000.


