#!/bin/sh

sigint_count=0

_handle_int(){
  if [ "${sigint_count}" -lt 3 ]; then ((sigint_count++)); return; fi
  echo 1
  exit 0
}

trap _handle_int SIGINT
for ((i = 0; i < 100; i++)); do
ps ax | awk '{print $1}'
done
