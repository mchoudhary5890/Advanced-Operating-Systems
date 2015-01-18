#PBS -q cs6210
#PBS -l nodes=4:sixcore
#PBS -l walltime=00:01:00
#PBS -N compare8
OMPI_MCA_mpi_yield_when_idle=0
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 4 /nethome/mchoudhary8/MVAOSPro2/merge 4 1 
