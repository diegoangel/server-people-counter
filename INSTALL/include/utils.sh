#!/usr/bin/env bash

# Provides      utils.sh
# Description   Funciones varias de soporte a la instalacion
# Copyright     2015 Movilgate

# Funcion que se ejecuta en caso de cancelar la instalacion
# 
# return void
function bye {
    clear
    rm -f $INTERFACES
    cp $INTERFACES.orig $INTERFACES
    rm -f $TEMP
    exit 0
}

# Retorna un listado de interfaces de red separadas por un salto de linea, excepto "lo"
#
# return string
function getNetworkInterfaces {
    ifconfig -a | sed 's/[ \t].*//;/^\(lo\|\)$/d'
}

# Funcion que muestra una barra de progreso utilizando la tool whiptail
#
# param $1 Titulo de la barra de progreso
# return void
function displayProgressBar {
    {
    for ((i = 0 ; i <= 100 ; i+=20)); do
        sleep 1
        echo $i
    done
    } | whiptail --gauge "$1" 6 60 0
}