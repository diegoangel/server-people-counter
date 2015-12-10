#!/usr/bin/env bash

### BEGIN INFO
#
# Description:	Bash script para monitorear el estado del servicio server-conteo-personas.
#			En el caso de que el servicio no se este ejecunatdo este script tratara de reiniciar el servicio.
#			Ejecutar este script mediante un cron como root.
#			Implementacion mediante un cron para que se ejecute cada 1 minuto:
#			root > crontab -e
#			Agregar al final del archivo:
#			> */1 * * * * /home/movilgate/binn/servicio-conteo-personas/monitor.sh
# Copyright:    2015 Movilgate
#
### END INFO

SERVICE="server-servicio-conteo-personas"

service $SERVICE status

if [ $? -ne 0  ]; then
    service $SERVICE restart
fi
