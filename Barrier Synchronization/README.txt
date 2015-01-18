NOTE:openMP files: central.c dissemination.c
     MPI files : centralMPI.c mcs.c
     Merged file:merge.c

1)To compile the code:

Run the Makefile.

2)To run the code

a)For running OpenMP barriers

  * Open the bash file openmp.sh
  * Provide the executable name as per your machine directory along with the required parameters
  *Appropriately change the number of nodes

  Example:/nethome/mchoudhary8/MVAOSPro2/diss <num_of_threads> <num_of_barriers>    
          /nethome/mchoudhary8/MVAOSPro2/central <num_of_threads>

b)For running MPI Barriers

  * Open the bash file mpi.sh
  * Provide the executable name as per your machine directory along with the required parameters
  *Appropriately change the number of nodes
 
Example:/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 8 /nethome/mchoudhary8/MVAOSPro2/centralMPI

/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 8/nethome/mchoudhary8/MVAOSPro2/mcs <num_of_barriers>

c)For runnig Merge(Combined Barrier)

  * Open the bash file merge.sh
  * Provide the executable name as per your machine directory along with the required parameters
  *Appropriately change the number of nodes

  Example: /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 4 /nethome/mchoudhary8/MVAOSPro2/merge 4 1 




  

