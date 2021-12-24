# TP4: Sistema de archivos e intérprete de comando

## Parte 1: caché de bloques

Preguntas: ¿Qué es super->s_nblocks? ¿Dónde y cómo se configura este bloque especial?

Super es una variable, la misma representa el superbloque del filesystem. Este superbloque contiene tres campos: el campo s_magic el cual almacena el numero magico FS_MAGIC; el campo File s_root que es un struct que representa el nodo del directorio raiz, y el campo s_nblocks que almacena la cantidad de bloques del filesystem.
En la función diskaddr() se valida que el numero de bloques que se pasan por parametros no supere a la cantidad de bloques que tiene el disco que estamos usando.


