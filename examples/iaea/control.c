/*

   This program is to test the coupling of FERMI with other
   codes. 

   Termo-hydraulic Feedback

   This programs starts sending a group of cross sections to FERMI
   and this last one calculates the power distribution and sends 
   it here. Using that power "control.c"  calculates the new set 
   of cross sections and the process is repeated.

   Authors: 

   Guido Giuntoli

*/

#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "commdom_wrapper.h"  

#define NITER 100

int main(int argc,char **argv)
{

  FILE   *fl=fopen("control_otu.dat","w");
  int    i;
  int    rank       = -1;
  int    size       = -1;
  int    globa_rank = -1;
  int    globa_size = -1;
  int    local_rank = -1;
  int    local_size = -1;
  int    size_commij;
  int    ierr;

  char   file_c[64];

  MPI_Comm WORLD_Comm   = MPI_COMM_WORLD; // Global communicator
  MPI_Comm CONTROL_Comm = MPI_COMM_NULL ; // Local communicator

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(WORLD_Comm, &globa_rank);
  MPI_Comm_size(WORLD_Comm, &globa_size);
  printf("control.c : globa_rank = %d globa_size = %d\n", globa_rank, globa_size);

  char world[]     = "acople";
  char my_name[]   = "control" ;
  char my_friend[] = "fermi";
  int  myID        = 2;
  int  fermiID     = 1;


  commdom_create();

  commdom_set_names( world, my_name);
  
  // We create local communicators (collective = split inside)
  commdom_create_commij(&WORLD_Comm, &CONTROL_Comm);

  MPI_Comm_rank(CONTROL_Comm, &local_rank);
  MPI_Comm_size(CONTROL_Comm, &local_size);
  printf("control.c : globa_rank = %d CONTROL_Comm = %d\n", globa_rank, CONTROL_Comm);

  int      inter_size;
  int      inter_rank;
  MPI_Comm INTER_Comm   = MPI_COMM_NULL ; // Inter-communicator

  // We get the inter-communicator to talk with FERMI
  commdom_get_commij(my_friend, &INTER_Comm); 

  MPI_Comm_rank(INTER_Comm, &inter_rank);
  MPI_Comm_size(INTER_Comm, &inter_size);
  printf("\ncontrol.c : inter_rank = %d inter_size = %d \n", inter_rank, inter_size);

  // array to determine the global rank using Allgather 
  int * share;
  int value;
  int remote_rank;

  share = (int*)malloc(inter_size * sizeof(int));

  value = myID;
  ierr = MPI_Allgather(&value,1,MPI_INT,share,1,MPI_INT,INTER_Comm);

  if(ierr){
    return 1;
  }

  for(i=0;i<inter_size;i++){
    if(share[i]==fermiID){
      remote_rank = i;
    }
  }

  free(share);

  // We are ready to send cross sections here doing Bcast with INTER_Comm
  // and receiving powers with Recv (can be reduced by rank 0 in 0 and the send by him)
  //
  int tag=0;
  double xs_a0 = 0.2;
  double xs[20]={             \
    1.5,0.2  ,0.2,5.4e-6,1.0, \
    1.5,0.2  ,0.2,5.4e-6,1.0, \
    1.5,0.2  ,0.2,5.4e-6,1.0, \
    1.5,0.2  ,0.0,0.0   ,1.0  };

  double pow[4]={0.0,0.0,0.0,0.0};
  double pow_0;
  int  mat = 1;

  MPI_Status    status;

  xs[mat*5 + 1] = xs_a0;
  for(i=0;i<NITER;i++){

    // send xs
    ierr = MPI_Ssend(xs,20,MPI_DOUBLE,remote_rank,tag,INTER_Comm);

    // recv pow
    ierr = MPI_Recv(pow,4,MPI_DOUBLE,remote_rank,tag,INTER_Comm,&status);

    // we calculate the new set of cross sections 
    // using the power
    if(i==0) pow_0 = pow[mat];
    // negative feedback model for water
    // if pow grows xs_a grows and pow decrease
    xs[mat*5 + 1] = xs_a0 + (pow[mat] - pow_0)* 0.0e-6;
    fprintf(fl,"%e %e\n",pow[mat],xs[mat*5 + 1]);

  }

  fclose(fl);
  MPI_Barrier(WORLD_Comm);
  MPI_Finalize();

  return 0;
} 
