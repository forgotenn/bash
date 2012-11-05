#!/bin/bash

shopt -s nullglob

QPATH=$HOME/.nyaqueue

mkdir -p "$QPATH"
mkdir -p "$QPATH/requests"

for a in "$@"
do
    r=`mktemp --tmpdir="$QPATH/requests"`
    echo "$a" > "$r"
done

if [[ -f "$QPATH/running" ]]
then
    if kill -0 `cat $QPATH/running` 2>/dev/null
    then
        true
    else
        rm '$QPATH/running'
    fi
fi

if [[ ! -f "$QPATH/running" ]]; then
    echo $$ > "$QPATH/running"
    trap "rm '$QPATH/running'" 0
    trap "" SIGHUP

    while true
    do
        for a in "$QPATH/requests"/*
        do
            url=`cat "$a"`
            rm "$a"
            wget -c "$url"
        done
        sleep 1
    done
fi
