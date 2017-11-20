#!/bin/bash
for i in {1..10}
do
   echo "Process $i started"
   ./main &
done