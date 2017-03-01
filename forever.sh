#!/bin/bash
function finish() 
{
  echo goodbye
  exit 0
}
trap finish SIGINT SIGTERM
while :; do
  sleep 0.3
done
