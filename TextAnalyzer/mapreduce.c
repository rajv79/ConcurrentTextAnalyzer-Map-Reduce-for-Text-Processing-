#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#include <ctype.h>
#include <unistd.h>


#include "mapreduce.h"
#include "common.h"

// add your code here ...


void mapreduce(MAPREDUCE_SPEC * spec, MAPREDUCE_RESULT * result)
{
    struct timeval start, end;


    gettimeofday(&start, NULL);


    int fd = open(spec->input_data_filepath, O_RDONLY);

    if (fd < 0) {

        EXIT_ERROR(2, "EEOR\n");
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    
    int split_size = file_size / spec->split_num;

    lseek(fd, 0, SEEK_SET); 

 

    result->map_worker_pid = malloc(spec->split_num * sizeof(int));

    for (int i = 0; i < spec->split_num; i++) {
        int pid = fork();


        if (pid == 0) { 


            DATA_SPLIT split;
            split.fd = fd;
            split.size = split_size;
            split.usr_data = spec->usr_data;

        

            char temp_file_name[64];
            sprintf(temp_file_name, "mr-%d.itm", i);
            
            int fd_out = open(temp_file_name, O_RDWR | O_CREAT| O_TRUNC, 0666);/// we have to use overwrite  all the contents  so we will use O_TRUNC
            if (fd_out < 0) {
                EXIT_ERROR(3, "ERROR\n");
            }

           
            if (i == spec->split_num - 1) {
                split.size += file_size % spec->split_num;
            }

            spec->map_func(&split, fd_out);

            close(fd_out);

            exit(0); 

        } else if (pid > 0) { 
            result->map_worker_pid[i] = pid;
        } else {
            EXIT_ERROR(4, "ERROR.\n");
        }
    }

    
    for (int i = 0; i < spec->split_num; i++) {
        waitpid(result->map_worker_pid[i], NULL, 0);
    }

   

   //-----------------------------------------------------------------------------------------------------------------------------------

    int reduce_pid = fork();


    if (reduce_pid == 0) { 
        
        int *fd_in = malloc(spec->split_num * sizeof(int));
        for (int i = 0; i < spec->split_num; i++) {
            char temp_file_name[64];


            sprintf(temp_file_name, "mr-%d.itm", i);
            fd_in[i] = open(temp_file_name, O_RDONLY);
            if (fd_in[i] < 0) {
                EXIT_ERROR(5, "ERROR.\n");
            }
        }

       
        int fd_out = open(result->filepath, O_RDWR | O_CREAT|O_TRUNC, 0666); /// we have to use overwrite  all the contents  so we will use O_TRUNC
 
        if (fd_out < 0) {
            EXIT_ERROR(6, "ERRRO.\n");
        }

        spec->reduce_func(fd_in, spec->split_num, fd_out);

        for (int i = 0; i < spec->split_num; i++) {
            close(fd_in[i]);
        }
        free(fd_in);
        close(fd_out);

        exit(0); 
        
    } else if (reduce_pid > 0) {
        result->reduce_worker_pid = reduce_pid;
    } else {
        EXIT_ERROR(7, "ERROR\n");
    }

  
    waitpid(result->reduce_worker_pid, NULL, 0);

    gettimeofday(&end, NULL);

    result->processing_time = (end.tv_sec - start.tv_sec) * US_PER_SEC + (end.tv_usec - start.tv_usec);
}





























//----
// void mapreduce(MAPREDUCE_SPEC *spec, MAPREDUCE_RESULT *result) {
//     struct timeval start, end;

//     if (NULL == spec || NULL == result) {
//         EXIT_ERROR(ERROR, "NULL pointer!\n");
//     }

  
//     FILE *file = open(spec->input_data_filepath, "r");
//     //get th file desscriptor from here


//     if (file == NULL) {
//         EXIT_ERROR(ERROR, "Failed to open input file!\n");
//     }

//     int file_size = get_file_size(spec->input_data_filepath); // Implement this function as needed
//     int split_size = file_size / spec->split_num;

//     gettimeofday(&start, NULL);



//     for (int i = 0; i < spec->split_num; i++) {
//         int start_pos = i * split_size;
//         int end_pos = (i + 1) * split_size;
//         if (i == spec->split_num - 1) {
//             end_pos = file_size;
//         }

//         pid_t pid = fork();

//         if (pid == 0) { // Child process

//             //----------- to do
//             // so to creat  a new file useing  open and then usin Lseek to change the file postion 
//              // then create data_Split to get the fd  , size  from here to put into map func 

//             // to do pass file descripter instead file object---------------
//             lseek(file, start_pos, SEEK_SET);

//             int size_to_read = end_pos - start_pos;

//             // we whave to put this global  define 
//             char *buffer = malloc(size_to_read + 1); 

//             if (buffer == NULL) {
//                 EXIT_ERROR(ERROR, "Memory allocation failed!\n");
//             }
//             fread(buffer, size_to_read, 1, file);



//             buffer[size_to_read] = '\0'; // Ensure null-terminated string

           
//             spec->map_func("???????");

//             free(buffer);

//             close(file);  ///   change from f to linux


//             exit(0); // Exit child process
//         } 
        
        
//         else if (pid < 0) {
//             EXIT_ERROR(ERROR, "fork failed!\n");
//         }
//     }

//     // Parent process: wait for all children to exit
//     while (wait(NULL) > 0);

//     fclose(file); // Close the file in the parent process

//     // 
//     spec->reduce_func(spec->usr_data);

//     gettimeofday(&end, NULL);
//     result->processing_time = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
// }


// //-------------------------------------------------------------------------

