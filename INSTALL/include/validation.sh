#!/usr/bin/env bash

# Provides      validation.sh
# Description   Este archivo contiene funciones para realizar validaciones
# Copyright     2015 Movilgate

# Test an IP address for validity:
#
# Usage:
#      validIP IP_ADDRESS
#      if [[ $? -eq 0 ]]; then echo good; else echo bad; fi
#   OR
#      if validIP IP_ADDRESS; then echo good; else echo bad; fi
#
# See also: http://www.linuxjournal.com/content/validating-ip-address-bash-script
function validIP {
    local  IP=$1
    local  RESULT=1

    if [[ $IP =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        OIFS=$IFS
        IFS='.'
        IP=($IP)
        IFS=$OIFS
        [[ ${IP[0]} -le 255 && ${IP[1]} -le 255 \
            && ${IP[2]} -le 255 && ${IP[3]} -le 255 ]]
        RESULT=$?
    fi
    return $RESULT
}

validIP 10.0.0.3