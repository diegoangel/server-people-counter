# README #

### Configuracion ###



### Instalación de dependencias ###

Las dependencias estan declaradas en el archivo cpanfile

Ejecutar en consola:

```bash
cpanm --cpanfile cpanfile --installdeps .
```

### Generación de documentación ###

Instalar Pod::Tree

```bash
cpanm Pod::Tree
```

Ejecutar en consola:

```bash
pods2html --empty --module --index "UDPClient Documentación" . ./doc ./doc/template/template.html
```