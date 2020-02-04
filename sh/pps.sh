#!/bin/sh
# args=$@

sigint_count=0

handle_int(){
  sigint_count=$((sigint_count + 1))
  if [ "${sigint_count}" -ne 4 ]; then return; fi
  echo "Active terminals:"
  ps a -o tty= | sort | uniq

  # echo "WC:" && wc $args
  exit 0
}

trap 'handle_int' 2

echo "Active processes:"
ps ax -o pid= | cat
printf "Enter anything to exit: "
while true; do
  read _ && exit 0
done

