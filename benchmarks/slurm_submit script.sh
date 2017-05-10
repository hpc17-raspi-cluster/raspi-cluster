read -d '' text << EOF
#!/bin/bash
#----------------------------------------------------
# SLURM job script to run MPI applications on 
# TACC's Stampede system.
#
#----------------------------------------------------

#SBATCH -J hpcclass           # Job name
#SBATCH -o %j.$1.out       # Name of stdout output file (%j expands to jobId)
#SBATCH -p normal        # Queue name
#SBATCH -N 16                  # Total number of nodes requested (16 cores/node)
#SBATCH -n 256                 # Total number of mpi tasks requested
#SBATCH -t 00:30:00           # Run time (hh:mm:ss) - 1.5 hours

#SBATCH -A TG-ASC170005      # <-- Allocation name to charge job against

# Launch the MPI executable named "a.out"

$1
EOF

echo  "$text"