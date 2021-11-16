TP2: Procesos de usuario
========================

env_alloc
---------

`ENVGENSHIFT = 12` entonces `1 << ENVGENSHIFT = 1 << 12 = 4096` y `NENV = 1 << LOG2NENV = 1 << 10 = 1024` entonces `~(NENV - 1) = -1023`. 
generation es el `env_id` que traía más `4096 & -1023` para despues hacer un *or* un offset entre la posicion que ocupa el env actual (`struct Env *e`) y el arreglo de envs (`struct Env *envs`).

Según la consigna, en base hexadecimal quedaría:

* 4096 = 1000
* 1023 = 03FF

</br>

Entonces el *and* da 4096 o 1000 en hexadecimal y al hacer el *or* le voy a sumar el offset. De esta manera los primeros 5 valores van a ser (en hexadecimal):

* 4096 = 1000
* 4096 + 1 = 1001 
* 4096 + 2 = 1002
* 4096 + 3 = 1003 
* 4096 + 4 = 1004

En *env_init()* cuando los inicializo en el ciclo *for* coloco en cero los *env_id*, entonces no los tengo en cuenta y para este caso va a ser siempre 1000 + offset.


El proceso que se va a lanzar será 4096 más el offset 630, en hexadecimal: `1000 + 276 = 1276`. El proximo va a ser igual pero ahora el `env_id` del proceso anterior será `1276`. De esta manera se tendrá `(1276 + 1000) or 276 = 2276`. El siguiente igual pero ahora el id_previo es `2276` entonces `(2276 + 1000) or 276 = 3276` y así sucesivamente. En hexadecimal para las primeras 5 iteraciones quedará de la siguiente manera:

* 1276
* 2276
* 3276
* 4276
* 5276


...


env_init_percpu
---------------

A `lgdt` le paso como parámetro `gdt_pd = { sizeof(gdt) - 1, (unsigned long) gdt }`

`sizeof(gdt) = 6 bytes`, se puede ver a continuación el tamaño del struct de acuerdo a las direcciones donde escribe

```
struct Segdesc gdt[] = {
	// 0x0 - unused (always faults -- for trapping NULL far pointers)
	SEG_NULL,

	// 0x8 - kernel code segment
	[GD_KT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 0),

	// 0x10 - kernel data segment
	[GD_KD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 0),

	// 0x18 - user code segment
	[GD_UT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 3),

	// 0x20 - user data segment
	[GD_UD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 3),

	// 0x28 - tss, initialized in trap_init_percpu()
	[GD_TSS0 >> 3] = SEG_NULL
}

```

Los bytes representan ...TODO

...


env_pop_tf
----------

```

struct Trapframe {
	struct PushRegs tf_regs;
	uint16_t tf_es;
	uint16_t tf_padding1;
	uint16_t tf_ds;
	uint16_t tf_padding2;
	uint32_t tf_trapno;
	/* below here defined by x86 hardware */
	uint32_t tf_err;
	uintptr_t tf_eip;
	uint16_t tf_cs;
	uint16_t tf_padding3;
	uint32_t tf_eflags;
	/* below here only when crossing rings, such as from user to kernel */
	uintptr_t tf_esp;
	uint16_t tf_ss;
	uint16_t tf_padding4;
} __attribute__((packed));

```


* `%0` hace referencia a un operando (el primero), en este caso es el que se pasa por parámetro: `tf`. Entonces el tope de la pila justo antes de `popal` tiene el puntero tf (direccion a la que apunta).
* `popal` restaura todos los registros. `popl` popea el stack, esto se hace 2 veces. Luego se le suma `0x8` (`/* skip tf_trapno and tf_errcode */`). Con este último comentario se puede asumir que el tope de la pila va a contener a `uintptr_t tf_eip`.
* Los siguientes tres elementos serán  `tf_cs`, `tf_padding3` y `tf_eflags` será el tercer elemento antes de ejecutarse `iret`.


</br>
</br>

De la función `env_alloc()`
```
	// Set up appropriate initial values for the segment registers.
	// GD_UD is the user data segment selector in the GDT, and
	// GD_UT is the user text segment selector (see inc/memlayout.h).
	// The low 2 bits of each segment register contains the
	// Requestor Privilege Level (RPL); 3 means user mode.  When
	// we switch privilege levels, the hardware does various
	// checks involving the RPL and the Descriptor Privilege Level
	// (DPL) stored in the descriptors themselves.
	e->env_tf.tf_ds = GD_UD | 3;
	e->env_tf.tf_es = GD_UD | 3;
	e->env_tf.tf_ss = GD_UD | 3;
	e->env_tf.tf_esp = USTACKTOP;
	e->env_tf.tf_cs = GD_UT | 3;
	// You will set e->env_tf.tf_eip later.

```

Para determinar el nivel se ven los 2 bits más bajos. X86 lo hace en los de `tf_cs`. FALTA COMO DETERMINA UN CAMBIO X86

...


gdb_hello
---------

...
