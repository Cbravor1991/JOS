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

...


env_pop_tf
----------

...


gdb_hello
---------

...
