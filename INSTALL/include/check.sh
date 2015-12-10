#!/usr/bin/env bash

# Description:      Script para realizar comprobaciones basicas de entorno y funcionamiento del server-servicio-conteo-personas
#                   de conteo de personas.
# Copyright         2015 Movilgate


# DEFAULT VALUES ######################

TEMP=/tmp/INSTALL-server-servicio-conteo-personas.log
RETMSG="ERROR"

# UTILITIES FUNCTIONS #################

displayProgressBar() {
    {
    for ((i = 0 ; i <= 100 ; i+=20)); do
        sleep 1
        echo $i
    done
    } | whiptail --gauge "$1" 6 60 0
}

# MENU FUNCTIONS ######################

checkInternetConection() {
    displayProgressBar "Comprobando conexión a Internet"

    wget -q --spider 181.15.96.221
    
    WGET=$?

    if [ $WGET -eq 0 ]; then
        RETMSG="La conexión a Internet funciona correctamente"
    else
        RETMSG="Ups, hemos encontrado un problema y no pudimos conectarnos a Internet"
    fi

    whiptail --title "Resultado de la comprobación" --msgbox "$RETMSG.\n\nSeleccione Ok para continuar." 10 60
}

checkMovilgate() {
    displayProgressBar "Comprobando conexión a © Movilgate"

    nping -c1 --udp -p 20001 --once 64.76.120.14 &> "$TEMP"
    
    if [ "$(grep 'Lost: 0' $TEMP)" != "" ]; then
        RETMSG="La conexión a © Movilgate funciona correctamente"
    else
        RETMSG="Ups, hemos encontrado un problema y no pudimos conectarnos a © Movilgate"
    fi
    
    whiptail --title "Resultado de la comprobación" --msgbox "$RETMSG.\n\nSeleccione Ok para continuar." 10 60
}

checkMemcached() {
    displayProgressBar "Comprobando conexión a memcached"

    exit | telnet 127.0.0.1 11211 &> "$TEMP"

    if [ -s "$TEMP" ]; then
        RETMSG="La conexión al servidor local de memcached funciona correctamente"
    else
        RETMSG="Ups, hemos encontrado un problema y no pudimos conectarnos al servidor local de memcached"
    fi

    whiptail --title "Resultado de la comprobación" --msgbox "$RETMSG.\n\nSeleccione Ok para continuar." 10 60
}

checkServiceStatus() {
    displayProgressBar "Comprobando estado del servicio Conteo de Personas"

    service server-servicio-conteo-personas status > "$TEMP"

    if [ "$(grep 'PID' $TEMP)" != "" ]; then
        RETMSG="El servicio Conteo de Personas se encuentra funcionando"
    else
        RETMSG="Ups, hemos encontrado un problema, el servicio Conteo de Personas no se esta ejecutando"
    fi

    whiptail --title "Resultado de la comprobación" --msgbox "$RETMSG.\n\nSeleccione Ok para continuar." 10 60    
}

bye() {
  clear
  rm -f $TEMP
  exit
}

# ENTRY POINT FOR THE SCRIPT ##########

    STATUS="0"
    while [ "$STATUS" -eq 0 ]  
    do
        CHOICE=$(whiptail --title "Comprobaciones de conectividad, estado y entorno de la aplicación" \
         --backtitle "© 2015 Movilgate- http://www.movilgate.com" --menu "\nSeleccione una comprobación" 16 78 5 \
        "1." "Comprobar conexión a Internet" \
        "2." "Comprobar conexión al servidor de recepción de datos de Movilgate." \
        "3." "Comprobar conexión al servidor local de memcache." \
        "4." "Comprobar estado del servicio Conteo de Personas." 3>&1 1>&2 2>&3) 

        case "$CHOICE" in
            1.)
                checkInternetConection
                ;;
            2.) 
                checkMovilgate
                ;;
            3.)
                checkMemcached
                ;;
            4.)
                checkServiceStatus
                ;;
            *) 
                bye
                ;;
        esac
        EXITSTATUS=$STATUS
    done
