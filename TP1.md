TP1: Memoria virtual en JOS
===========================

boot_alloc_pos
--------------

...


page_alloc
----------

1. Responder: ¿en qué se diferencia `page2pa()` de `page2kva()`? </br> </br>
Tanto `page2pa()` como `page2kva()` reciben una página física, se diferencian en que `page2pa()` devuelve la dirección física con tipo `physaddr_t` y `page2kva()` retorna la dirección virtual con tipo `void*`. `page2kva()` internamente llama a `page2pa()` y luego pasa la dirección física a `KADDR()` que devuelve la dirección virtual.


map_region_large
----------------

1. ¿cuánta memoria se ahorró de este modo? (en KiB)  </br> </br>

 </br> </br>
2. ¿es una cantidad fija, o depende de la memoria física de la computadora? </br> </br>

 


