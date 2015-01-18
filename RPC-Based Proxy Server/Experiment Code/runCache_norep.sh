#/bin/bash
T="$(date +%s%N)"
cat url_norep.txt | while read line; do 
#    echo $line # or whaterver you want to do with the $line variable
	 ./proxy_client $line 128.61.119.8 9090
done
T="$(($(date +%s%N)-T))"
echo "Time in nanoseconds: ${T}"

