#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "usr_functions.h"


#define BUFFER_SIZE 2048 

#define LETTER_COUNT 26   



int letter_counter_map(DATA_SPLIT *split, int fd_out) {

    if (!split) return -1;

    const size_t buf_size = 2048;
    char *buf = (char *)malloc(buf_size * sizeof(char));

    if (!buf) return -1;

    ssize_t bytenext;

    int le_Count[LETTER_COUNT] = {0};

    ssize_t totalRead = 0;

    while (totalRead < split->size && (bytenext = read(split->fd, buf, buf_size)) > 0) {
        totalRead += bytenext;

        for (ssize_t i = 0; i < bytenext; ++i) {
            if (isalpha(buf[i])) {
                int idx = tolower(buf[i]) - 'a';
                if (idx >= 0 && idx < LETTER_COUNT) {
                    
                    le_Count[idx]++;
                }
            }
        }
    }

    for (int i = 0; i < LETTER_COUNT; ++i) {
        char lineOutput[50];

        int len = snprintf(lineOutput, sizeof(lineOutput), "%c %d\n", 'a' + i, le_Count[i]);

        if (write(fd_out, lineOutput, len) != len) {
            free(buf);
            return -1;
        }
    }

  
    return 0;
}


/* User-defined reduce function for the "Letter counter" task.  
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data files' file descriptors.
                   The imtermeidate data files are output by the map worker processes, and they
                   are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file descriptor is 
             identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2] respectively.

*/

int letter_counter_reduce(int *p_fd_in, int fd_in_num, int fd_out) {

    if (!p_fd_in || fd_in_num <= 0) return -1;

    int total_Count[LETTER_COUNT] = {0};

    for (int i = 0; i < fd_in_num; ++i) {

        FILE *FP = fdopen(p_fd_in[i], "r");

        if (!FP) return -1;

        char *lineBuf = NULL;

        size_t len_buf = 0;

        while (getline(&lineBuf, &len_buf, FP) != -1) {
            char le_Char;
            int le_Count;

            if (sscanf(lineBuf, "%c %d\n", &le_Char, &le_Count) == 2) {

                int chsrindex = tolower(le_Char) - 'a';
                if (chsrindex >= 0 && chsrindex < LETTER_COUNT) {
                    total_Count[chsrindex] += le_Count;
                }
            }
        }

        fclose(FP);
    }

    char outputBuf[50];

    for (int i = 0; i < LETTER_COUNT; ++i) {
        int outputLen = snprintf(outputBuf, sizeof(outputBuf), "%c %d\n", 'a' + i, total_Count[i]);
        if (write(fd_out, outputBuf, outputLen) != outputLen) {
            return -1;
        }
    }

    return 0;
}

//-------------------------------------------------------Helper Function  ---------------------------------------------------------------------------------------------------------------------


int delimiterchecking(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\0' || c == '.' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?';
}

int is_whole_word(const char *text, const char *word, const char *position) {
    const char *before = position > text ? position - 1 : text;
    const char *after = position + strlen(word);

    if ((position == text || delimiterchecking(*before)) && (delimiterchecking(*after) || *after == '\0')) {
        return 1;
    }
    return 0;
}

int word_finder_map(DATA_SPLIT *split, int fd_out) {
    if (!split || !split->usr_data) return -1;

    FILE *f_stream = fdopen(split->fd, "r");
    if (!f_stream) return -1;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f_stream)) != -1) {
       

        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
            --read;
        }

        if (strstr(line, split->usr_data) && is_whole_word(line, split->usr_data, strstr(line, split->usr_data))) {
            dprintf(fd_out, "%s\n", line);
        }
    }

    free(line);
    fclose(f_stream);

    return 0;
}

// Bonus----------------------------------------------------------------------------------------------


/* User-defined reduce function for the "Word finder" task.  
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data files' file descriptors.
                   The imtermeidate data files are output by the map worker processes, and they
                   are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file descriptor is 
             identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2] respectively.

*/
// int compare_ints(const void *a, const void *b) {
//     int arg1 = *(const int *)a;
//     int arg2 = *(const int *)b;
//     return (arg1 > arg2) - (arg1 < arg2);
// 

int word_finder_reduce(int *p_fd_in, int fd_in_num, int fd_out) {

    char **wl = malloc(10000 * sizeof(char*));
    if (wl == NULL) return -1;

    int count = 0;
    char *line = NULL;
    size_t len = 0;

   
    for (int i = 0; i < fd_in_num; ++i) {
        FILE *fp = fdopen(p_fd_in[i], "r");
        if (fp == NULL) {
            while (count-- > 0) free(wl[count]);
            return -1;
        }

        ssize_t read;

        while ((read = getline(&line, &len, fp)) != -1) {
           

            if (line[read - 1] == '\n') {
                line[read - 1] = '\0';
            }

            int flag = 0;
            
            for (int j = 0; j < count; ++j) {
                if (strcmp(wl[j], line) == 0) {
                    flag = 1;
                    break;
                }
            }

            if (!flag) {
                if (count >= 10000) {
                    fprintf(stderr, "Erroro.\n");
                    break; 
                }
                wl[count] = strdup(line);
                if (wl[count] == NULL) { 
                    while (count-- > 0) free(wl[count]); 
                    fclose(fp);
                    return -1;
                }
                count++;
            }
        }
        fclose(fp);
    }


    
    for (int i = 0; i < count; ++i) {
        if (dprintf(fd_out, "%s\n", wl[i]) < 0) {
            while (count-- > 0) free(wl[count]); 
            return -1; 
        }
    
    }

   
    return 0;
}







//----------------------------------------------------------- EXTRA_CODE-----------------------------------------------------------------------------------------------