#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <stdlib.h> 
#include <errno.h>   
#include <sys/wait.h> 
  
int main(){ 
   pid_t  pid1, pid2; 

   pid1 = fork(); 
  
   if (pid1 == -1){ 
      printf("can't fork, error occured\n"); 
      exit(EXIT_FAILURE); 
   } 
   else if (pid1 == 0){  
      char * argv_list[] = {"kinematyka_odwrotna","-lart","/home/SCZR-robot/SCZR-robot/",NULL}; 
  
      execv("kinematyka_odwrotna",argv_list); 
      exit(0);
   }
   usleep(1000);
   pid2 = fork(); 
  
   if (pid2 == -1){ 
      printf("can't fork, error occured\n"); 
      exit(EXIT_FAILURE); 
   } 
   else if (pid2 == 0){  
      char * argv_list[] = {"zadawanie_pozycji","-lart","/home/SCZR-robot/SCZR-robot/",NULL}; 
  
      execv("zadawanie_pozycji",argv_list); 
      exit(0);
   }

   return 0;
}
