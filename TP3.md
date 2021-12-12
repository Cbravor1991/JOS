

# Tarea env_return

</br>

Preguntas:

1. Al terminar un proceso su función umain() ¿dónde retoma la ejecución el kernel? Describir la secuencia de llamadas desde que termina umain() hasta que el kernel dispone del proceso.

2. ¿en qué cambia la función env_destroy() en este TP, respecto al TP anterior?

Respuestas:

1. En la función libmain() se llama a la función umain(), una vez que umain() finaliza se vuelve nuevamente a libmain(), esta sera la que llame al exit(), que hace uso de sys_env_destroy(), que finaliza invocando a env_destroy() que a su vez llama a env_free, la cual imprime que se esta liberando el envioment y libera los recursos del mismo y luego se invoca a shed_yield, quien llama a sched_alt, que procede a buscar otros environments y al no encontrar otro environments finalmente llama al monitor del kernel.

2. En el TP anterior la función env_destroy liberaba el unico proceso. En cambio en este TP sera necesario realizar una serie de validaciones ya que un mismo proceso va a poder estar corriendo en otra CPU y si no es el proceso actual de la CPU que estoy utilizando. Entonces estado del proceso cambiara a ENV_DYING, el proceso pasa a ser "ZOMBIE" y pasa a ser liberado cuando se le ceda el control.
Si no sucede lo descripto anteriormente se lo libera pero sera necesario antes chequear a traves de otra validación si este no estaba corriendo en ese momento, si sucede esto sera necesario actualizar el curenv y a través de sched_yield cambiar a otro proceso.


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


# Tarea: ipc_recv
</br>
1. Un proceso podría intentar enviar el valor númerico -E_INVAL vía ipc_send(). ¿Cómo es posible distinguir si es un error, o no? </br>

![](./capturacodigoipc_recv().png)

Respuesta: 
Para poder distinguir un error de un valor númerico podemos realizar el siguiente código:

```
envid_t src = -1;
int r = ipc_recv(&src, 0, NULL);

if (r < 0)
  if (!src)
    puts("Hubo error.");
  else
    puts("Valor negativo correcto.")

```

</br>

# Tarea sys_ipc_try_send

</br>

Para hacer bloqueante la llamada se podria usar un mecanismo similar al utilizado en recv cuando se envia se coloca al env como NOT_RUNNABLE, y en una lista se pone a los envs esperando enviarse. Y después al hacer un recvs se chequea la lista para ver si hay algun envs esperanso ser enviado. Para esto se va a poder hacer uso del flag env_ipc_recving.
El orden en que despertaría dependería tanto del orden en que el proceso se destrabe como en qué momento se recorra la lista para hacer los chequeos. El proceso que hace la llamada a recv también va a tener que poder hacer que el otro proceso retome el ENV_RUNNABLE.
El deadlock podría ocurrir si por alguna razón el proceso no hace recv sobre el proceso que envía el mensaje. 

Otra implementación un poco más simple seria utilizar un variable que me indique el estado del proceso B, si esta recibiendo o no. 


