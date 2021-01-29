#!/bin/bash
for d in *; do
  if [ -d "$d" ]; then
    echo -n "$d  "
  
  # echo -n "Template Slave.ino  "
    if arduino --verify "$d"/"$d".ino &>> log; then
      echo -e "\033[0;32mSuccess\033[0m";
    else
      echo -e "\033[0;31mFailure\033[0m";
    fi
  fi
done