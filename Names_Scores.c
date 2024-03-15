/* Names_Scores.c - A solution to Project Euler problem 22: Names Scores
 *- Sort the provided list of names (names.txt) in alphabetic order.
 *- Calculate a score for each name by finding an alphabetic value for each name and multiplying that value by its alphabetic position in the list.
 *    For example, COLIN is worth 3 + 15 + 12 + 9 + 14 = 53 points and is 938th in the list. So, COLIN would obtain a score of 938 * 53 = 49714.
 *- Find the sum of all the name scores in the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void getFileContents(FILE *fp, char *filetext);
void quickSort(int low, int high, int namesize, char namelist[][namesize]);
int partition(int low, int high, int namesize, char namelist[][namesize]);

int main(int argc, char **argv){
    /* Check command line args */
    if (argc != 2) {
	fprintf(stderr, "usage: %s <names.txt>\n", argv[0]);
	exit(1);
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    if(!fp){
        perror("File opening failed");
        return EXIT_FAILURE;
    }
    
    int i, j;				// Counters for various loops
    
    // Get information about the file of names and its contents
    struct stat info;
    stat(argv[1], &info);
    char filetext[info.st_size];	// The name file size determines how many chars it contains
    getFileContents(fp, filetext);

    int namecount = 0;
    for(i = 0; i < info.st_size; i++){
        if(filetext[i] == ',')
            namecount++;
    }
    namecount++;				// # of names in the file = # of ',' chars + 1
    
    int maxnamesize = 0;
    char *longestname;
    char *token = strtok(filetext, ",");
    for(i = 0; i < namecount; i++){		
        if(strlen(token) > maxnamesize){
            maxnamesize = strlen(token);
            longestname = token;
        }
        token = strtok(NULL, ",");		// Separate the name file by its commas into each name
    }
    maxnamesize++;				// strlen() does not count null characters, so the maximum string size needed for names is maxnamesize + 1
    
    // Convert the name file contents to an array of strings
    rewind(fp);
    getFileContents(fp, filetext);		// strtok() is destructive, so it is necessary to perform this step again to restore the file data

    char namelist[namecount][maxnamesize];

    token = strtok(filetext, ",");
    for(i = 0; i < namecount; i++){
        strncpy(namelist[i], token, strlen(token) + 1);
        token = strtok(NULL, ",");
    }
    printf("maximum name length = %d\n", maxnamesize);
    printf("# of names = %d\n\n", namecount);
    printf("names 1, 2, and 3 prior to sorting: %s, %s, %s\n", namelist[0], namelist[1], namelist[2]);
    printf("names N-2, N-1, and N prior to sorting: %s, %s, %s\n\n", namelist[namecount-3], namelist[namecount-2], namelist[namecount-1]);


    quickSort(0, namecount-1, maxnamesize, namelist);
    printf("names 1, 2, and 3 after sorting: %s, %s, %s\n", namelist[0], namelist[1], namelist[2]);
    printf("names N-2, N-1, and N after sorting: %s, %s, %s\n\n", namelist[namecount-3], namelist[namecount-2], namelist[namecount-1]);
    
    // Sum the name scores
    int namescore = 0;
    int scoresum = 0;
    for(i = 0; i < namecount; i++){
        for(j = 0; j < maxnamesize; j++){
            if(namelist[i][j] != '\0' && namelist[i][j] != '"'){	// Quotes and null terminator characters do not count toward the score
            	namescore += (int) namelist[i][j] - 64;			// Character score = ASCII code - 64
            }
        }
        namescore *= i + 1;						// Name score = Total character score * Name's place in the sorted list
        scoresum += namescore;
        namescore = 0;
    }
    printf("sum of all name scores = %d\n", scoresum);

    fclose(fp);
    return 0;
}

/* getFileContents - Copies the contents of a text file to a string
 *   arguments:
 *     fp - A file object associated with the file
 *     filetext - An empty string for the file contents
 */
void getFileContents(FILE *fp, char *filetext){
    int i = 0;
    int c = 0;
    while ((c = fgetc(fp)) != EOF) {
        filetext[i] = c;
        i++;
    }
}

/* quickSort - Sorts an array of strings using recursion and a helper function partition()
 *   arguments:
 *     low - An array index indicating the sub-array from indices [low, pivot - 1] contains words that come before namelist[pivot] alphabetically
 *     high - An array index indicating the sub-array from indices [pivot, high] contains words that come in the same order as or after namelist[pivot] alphabetically
 *     namesize - The size of each string in namelist
 *     namelist - The array of strings
 */
void quickSort(int low, int high, int namesize, char namelist[][namesize]){
    if(low >= high || low < 0)					// Base case: Check for valid low and high indices
        return;
    
    int pivot = partition(low, high, namesize, namelist);	// Partition the array of strings to get the pivot index and sort each partition
    
    quickSort(low, pivot - 1, namesize, namelist);
    quickSort(pivot + 1, high, namesize, namelist);

}

/* partition - Splits an array into two partitions and sorts each partition for quicksort
 *   arguments:
 *     low - An array index indicating the sub-array from indices [low, pivot - 1] contains words that come before namelist[pivot] alphabetically
 *     high - An array index indicating the sub-array from indices [pivot, high] contains words that come in the same order as or after namelist[pivot] alphabetically
 *     namesize - The size of each string in namelist
 *     namelist - The array of strings
 *
 *   return value: the pivot index which was used to split the array
 */
int partition(int low, int high, int namesize, char namelist[][namesize]){
    char pivotitem[namesize];
    strncpy(pivotitem, namelist[high], namesize);
    char temp[namesize];	// holds string data temporarily during a swap
    
    int splitindex = low - 1;  
    int i;			// loop counter
    
    for(i = low; i < high; i++){				// For the names in sub-array namelist[low] to namelist[high - 1]...
        if(strncmp(namelist[i], pivotitem, namesize) <= 0){	// ...if the current name comes before or in the same alphabetic order as the pivot name...
            //...then move the split index forward and swap the names.
            splitindex++;
            strncpy(temp, namelist[splitindex], namesize);
            strncpy(namelist[splitindex], namelist[i], namesize);
            strncpy(namelist[i], temp, namesize);
        }
    }
    // Move the element which was used to split the array between the two newly partitioned sub-arrays
    splitindex++;
    
    strncpy(temp, namelist[splitindex], namesize);
    strncpy(namelist[splitindex], namelist[high], namesize);
    strncpy(namelist[high], temp, namesize);
    
    return splitindex;
}
