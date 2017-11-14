# README #

### Introducción ###

La aplicación consta de un servidor desarrollado en C++ y un cliente UDP desarrollado en Perl.  
Elegí [Memcached](http://memcached.org/) para persistencia de datos en memoria y [JSON](http://json.org/) para serialización de datos.

Repositorio git de la aplicación: https://bitbucket.org/Movilgate/servicio-conteo-personas

Dependencias de librerías externas en la aplicación de C++:

* Blackfin API provista por Irisys
* Cliente Memcached: 
URL: http://libmemcached.org/libMemcached.html
Archivo. libmemcached/memcached.h
* JSON for Modern C++:
URL: https://github.com/nlohmann/json
Archivo: json.hpp
* Easylogging++: URL: https://github.com/easylogging/easyloggingpp  

Dependencias del cliente UDP en Perl:

* IO::Socket::INET
* POSIX
* DateTime
* JSON::Parse
* Cache::Memcached::Fast
* Data::Dumper
* Pod::Usage
* Log::Log4perl
* Log::Dispatch::FileRotate

### Instalación y Configuración ###


