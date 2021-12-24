# TP4: Sistema de archivos e intérprete de comando

## Parte 1: caché de bloques

Preguntas: ¿Qué es super->s_nblocks? ¿Dónde y cómo se configura este bloque especial?

Super es una variable, la misma representa el superbloque del filesystem. Este superbloque contiene tres campos: el campo s_magic el cual almacena el numero magico FS_MAGIC; el campo File s_root que es un struct que representa el nodo del directorio raiz, y el campo s_nblocks que almacena la cantidad de bloques del filesystem.

Se le da el formato al filesystem en el archivo fsformat.c, la estructura del superbloque se configura con la funcion opendisk(). Y la función fs_init() del archivo fs.c apunta a la estructura del superbloque.

