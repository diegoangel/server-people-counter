# README #

### INSTALACION ###

El instalador actualmente esta configurado para ser instalado en sistemas operativos linux con arquitecturas de 64 bits utilizando el compilador g++ y el estandar C++11.  
El instalador creara los directorios bin/ y build/.   
En el directorio bin/ se creara el binario ejecutable.  
En el directorio build/ estaran el codigo fuente compilado antes de enlazar a librerias externas y crear el ejecutable que se dejara en el mencionado directorio bin/.  

Para instalar ejecutar:  

```bash
make
```

Para reinstalar:

```bash
make clean
make
```

### ESTRUCTURA DEL PROYECTO ###

* src/  

Codigo fuente del programa.  

* include/  

Cabeceras incluidas en el codigo fuente.  

* doc/  

Documentacion de API generada con Doxygen en formato HTML.  

* conf/  

Archivos de configuracion del programa y de la libreria de logging.  

* Doxyfile  

Archivo de configuración para generar la documentación

* makefile  

Archivo para la compilación del programa

### DEPURACION / DEBUGGING ###

El ejecutable se compila con la opcion _-gdwarf-2 -g3_ para que sea posible la utilizacion de [GDB](http://www.gnu.org/software/gdb/) para depurar o debuggear el programa.  

```bash
cd bin/
gdb ./MovilgateServerApp
```

[Mas info sobre debugging con GDB](http://www.unknownroad.com/rtfm/gdbtut/gdbuse.html#RUN)

#### Optimizacion ####

Asi mismo el programa se compilo con la opcion de optimizacion _-O2_ que reduce tiempo de compilacion y mejora performance.  
Si se necesita una mejor experiencia de debugging se puede cambiar esta opcion a _-Og_.  

[Más info sobre optimizacion](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
