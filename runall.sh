#!/bin/bash

# Which tests and input sizes do we want to use?
TESTS=(network_bw_bm cache_latency_bm cpu_bm disk_bw_bm icache_bm memory_bw_bm)

#/c/Users/alxlm/Documents/configFile.txt
for test in ${TESTS[@]}; do
   time_stamp=$(date +%Y-%m-%d_%Hh%Mm%Ss)
   echo " "
   echo "*****************************************************************"
   echo "RUNNING TEST: " $test "  " $time_stamp  " : " 
   echo "*****************************************************************"
   echo " "
   echo $@
   #for K8's test--------------------------------
   #cd $test; make clean; make && python3 run_test.py -c /data/config/configFile.txt; make clean; cd ..;
   cd $test; make clean; make && python3 run_test.py -c ../configFile.txt; make clean; cd ..;
   echo " "
done

