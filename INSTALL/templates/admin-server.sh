#!/usr/bin/env bash

### BEGIN INIT INFO
#
# Provides:         servicio-conteo-personas
# Required-Start:   $local_fs $network $syslog $time memcached
# Required-Stop:    $syslog $time
# Default-Start:    2 3 4 5 
# Default-Stop:     0 1 6
# Description:      Init script del servicio de conteo de personas. Codigos de exit y este bloque de comentario 
#                   corresponden al estandar LSB.
#                   http://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/iniscrptact.html
#                   Instalacion Debian/Ubuntu: 
#                   > sudo update-rc.d -f server-servicio-conteo-personas remove
#                   > sudo cp admin-server.sh /etc/init.d/server-servicio-conteo-personas
#                   > sudo chmod 755 /etc/init.d/server-servicio-conteo-personas
#                   > sudo update-rc.d server-servicio-conteo-personas defaults
#                   > sudo service server-servicio-conteo-personas start
#                   Instalacion Redhat/Centos/Fedora
#                   TODO: completar instalacion servicio con chkconfig
# Copyright         2015 Movilgate
#
### END INIT INFO

# HEY Y'ALL, EDITING THIS FILE CAN BRING TERRIBLE CONSEQUENCES, I'M JUST SAYING #################

APPLICATION_USER=movilgate

if [ "$(whoami)" == "$APPLICATION_USER" ] || [ "$(whoami)" == "root" ]; then

    BIN_PATH=/home/movilgate/binn/servicio-conteo-personas/server/bin
    APPLICATION_NAME=server-servicio-conteo-personas
    EXIT_CODE=0
    PIDFILE=/tmp/$APPLICATION_NAME.pid
    LOGFILE=/tmp/$APPLICATION_NAME.log
    MG_DEBUG=0
    export MG_DEBUG

    start() {
        if [ -f $PIDFILE ] && kill -0 $(cat $PIDFILE); then
            echo "$APPLICATION_NAME already running"
            EXIT_CODE=1
        fi
        if [ ! -f "$BIN_PATH/$APPLICATION_NAME" ]; then
            echo "PATH/$APPLICATION_NAME wasn't founded";
            EXIT_CODE=5
        fi
        echo "Starting $APPLICATION_NAME..."
        $BIN_PATH/$APPLICATION_NAME $PIDFILE $LOGFILE
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 0 ] && touch $PIDFILE; then
            echo "$APPLICATION_NAME started"
        fi
        return $EXIT_CODE
    }

    stop() {
        if [ ! -f "$PIDFILE" ] || ! kill -0 $(cat "$PIDFILE"); then
            echo "Service not running"
            EXIT_CODE=7
        fi      
        echo "Shutting down $APPLICATION_NAME..."
        kill -15 $(cat "$PIDFILE")
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 0 ] && rm -f $PIDFILE; then
            echo "$APPLICATION_NAME stopped"
        fi
        return $EXIT_CODE
    }   

    status() {
        printf "%-50s" "Checking $APPLICATION_NAME..."
        if [ -f $PIDFILE ]; then
            PID=$(cat $PIDFILE)
            if [ -z "$(ps axf | grep ${PID} | grep -v grep)" ]; then
                printf "%s\n" "The process appears to be dead but pidfile still exists"
                EXIT_CODE=1
            else    
                echo "Running, the PID is $PID"
                EXIT_CODE=0
            fi
        else
            printf "%s\n" "$APPLICATION_NAME is not running"
            EXIT_CODE=3
        fi
        return $EXIT_CODE
    }

    case "$1" in
        start)
            start
            ;;
        stop)
            stop
            ;;
        status)
            status
            ;;
        restart)
            stop
            start
            ;;
        info)
            echo "BIN file: $BIN_PATH/$APPLICATION_NAME"
            echo "PID file: $PIDFILE"
            echo "LOG file: $LOGFILE"
            ;;            
        *)
            if [ -f "./doc/milk" ]; then
                echo "$(cat ./doc/milk)"
            else
                echo "Usage: $0 {start|stop|status|info|restart}" 
            fi
            exit 1
            ;;
    esac
    exit $EXIT_CODE
else
    echo "Solo el usuario $APPLICATION_USER o el usuario root pueden ejecutar $0"
    exit 4
fi