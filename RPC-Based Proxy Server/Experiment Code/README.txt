1)Place all the files under one directory.

2)Do a make clean to clear all the object files and executables(if any).

3)Do a make to build all the executables.

4)Run the proxy server first on one terminal.

    ./proxy_server <Cache_Size_in_KB>  <Cache_Replacement_Policy>

    Cache_Replacement_Policy = 1 for Random Replacement policy
    Cache_Replacement_Policy = 2 for LRU policy
    Cache_Replacement_Policy = 3 for MAXS policy

5)Run the client on another terminal or from a remote machine

    ./proxy_client <URL> <Proxy Server IP> 9090


6)To execute the different workloads execute the following on Client terminal

  AntiLRU     ->  ./runCache_antilru.sh
  AntiMAXS    ->  ./runCache_antimax.sh
  MostCommon  ->  ./runCache_mostcommon.sh
  NoRepeat    ->  ./runCache_norep.sh  

  Note: Please change the IP Address in the bash scripts before running the workloads.
   

   

         
