/**
 * Zachary Hill
 * CS 3411
 * 4/19/2016
 *
 * This program is designed to find all the duplicate files contained
 * in a directory tree, starting with the root directory given as the
 * only argument and traversing all directories beneath it to print
 * the duplicate files.
 */
#include<stdlib.h>
#include<stdio.h>


int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage error - incorrect number of arguments\n");
    }


    printf("The directory: %s\n", argv[1]);

    exit(EXIT_SUCCESS);
}

/**
 * References
 *  File generate-files.py written and provided by scott Kuhl
 */
