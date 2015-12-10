#!/usr/bin/env bash

#Provides       INSTALL.sh
# Description   Script de instalacion del Servicio Conteo de Personas
#               Utilizacion:
#               > ./INSTALL.sh
# Copyright     2015 Movilgate

# VARS ################################

WORKING_DIR=$PWD
source "$WORKING_DIR/include/vars.sh"

# UTILITIES FUNCTIONS #################

source "$WORKING_DIR/include/json.sh"
source "$WORKING_DIR/include/validation.sh"
source "$WORKING_DIR/include/utils.sh"

# SIGNAL HANDLER ######################

trap bye SIGHUP SIGINT SIGTERM

# MENU FUNCTIONS ######################

function enterStaticIP {

    whiptail --inputbox "\n\nIngrese la IP estática que va a tener el equipo" --title "Configuracion de IP estática" 10 60 2>"$TEMP"
    
    if [ $? -eq 0 ]; then
        validIP "$(<$TEMP)"
        if [ $? -eq 0 ]; then
            "$(cat $TEMP)" | tee -a "$INTERFACES"
            RETMSG="User selected Ok and entered $TEMP $(<$INTERFACES)"
        fi
    else
        RETMSG="User selected Cancel $(<$TEMP)."
    fi
}

function enterGateway {
    echo ""
}

function enterSubmask {
    echo ""
}

function configStaticIP {
    cp "$INTERFACES" "$INTERFACES.orig"

    if [ $? -ne 0 ]; then
        STATUS=1
        RETMSG="No se pudo copiar el archivo "
    else 
        sed -i -e '6,$d' "$INTERFACES"
        if [ $? -eq 0 ]; then
            enterStaticIP    
        fi
        if [ $? -eq 0 ]; then
            enterSubmask
        fi      
        if [ $? -eq 0 ]; then
            enterGateway
        fi           
    fi
     
    whiptail --title "Resultado de la comprobación" --msgbox "$RETMSG.\n\nSeleccione Ok para continuar." 30 80
}

function installServer {
    whiptail --title "Example Dialog" --textbox /etc/passwd 20 60 --scrolltext
}

function installClient {
    rm -f /root/test.tmp
}

function checkInstall {
    echo ""
}

# ENTRY POINT FOR THE SCRIPT ##########

function main {
    while [ $STATUS -eq 0 ]
    do
        CHOICE=$(whiptail --title "Instalacion" \
         --backtitle "© 2015 Movilgate- http://www.movilgate.com" --menu "\nSeleccione una opcion" 16 78 5 \
        "1." "Configurar IP estatica" \
        "2." "Instalar servidor." \
        "3." "Instalar cliente." \
        "4." "Chequeo de la instalacion" 3>&1 1>&2 2>&3) 

        case "$CHOICE" in
            1.)
                configStaticIP
                ;;
            2.) 
                installServer
                ;;
            3.)
                installClient
                ;;
            4.) 
                checkInstall
                ;;
            *) 
                bye
                ;;
        esac

        # SHOW ERROR MESSAGE AND RESET STATUS 

        if [  $STATUS -ne 0 ]; then
            whiptail --title "ERROR" --msgbox "$RETMSG.\n\nSeleccione Ok para continuar." 10 60

            if [ $? == 0 ]; then
                main
            fi
        fi
    done
}

# ENTRY POINT CALLING

main

# END OF INSTALL SCRIPT ###############