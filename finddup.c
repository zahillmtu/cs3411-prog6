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
#define _BSD_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<stddef.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<dirent.h>
#include<fts.h>
#include<sys/errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<openssl/md5.h>

struct hashInfo {
    unsigned char hash[MD5_DIGEST_LENGTH];
    char filename[256];
};

void printHash(unsigned char hash[MD5_DIGEST_LENGTH]) {

    for (int j = 0; j < MD5_DIGEST_LENGTH; j++) {
        printf("%02x", hash[j]);
    }
    printf("\n");

}

int countDirs(char * path[]) {

    int count = 0;

    FTS * ftsp = fts_open(path, FTS_PHYSICAL, NULL);
    if (ftsp == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    while(1) {
        FTSENT *ent = fts_read(ftsp);
        if (ent == NULL) {
            if (errno == 0) {
                break;
            } else {
                perror("fts_read");
                exit(EXIT_FAILURE);
            }
        }

        if (ent->fts_info & FTS_F) {
           count = count + 1;
        }
    }

    fts_close(ftsp);

    return count;
}

void getMD5(char * path[], struct hashInfo hashes[]) {

    // start the traversal
    FTS * ftsp = fts_open(path, FTS_PHYSICAL, NULL);
    if (ftsp == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    int i = 0;

    while(1) {
        FTSENT *ent = fts_read(ftsp);
        if (ent == NULL) {
            if (errno == 0) {
                break; // nothing more to traverse
            } else {
                perror("fts_read");
                exit(EXIT_FAILURE);
            }
        }

        if (ent->fts_info & FTS_F) {

            //printf("The file: %s\n", ent->fts_path);

            int fd = open(ent->fts_accpath, O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            // get the file size
            struct stat stb;
            if (stat(ent->fts_accpath, &stb) == -1) {
                perror("stat");
                exit(EXIT_FAILURE);
            }

            // set the values to be empty
            //memset(hashes[i]->hash, 0, MD5_DIGEST_LENGTH);
            //hashes[i]->filename = NULL;

            // save the name of the file
            memcpy(hashes[i].filename, ent->fts_path, strlen(ent->fts_path));

            char * buf = mmap(0, stb.st_size, PROT_READ, MAP_SHARED, fd, 0);
            MD5((unsigned char*) buf, stb.st_size, hashes[i].hash);
            munmap(buf, stb.st_size);

            i = i + 1;

            close(fd);
        }
    }

    fts_close(ftsp);
}

void printDup(struct hashInfo hashes[], int fileCount) {

    for (int i = 0; i < fileCount - 1; i++) {
        if (memcmp(hashes[i].hash, hashes[i + 1].hash, MD5_DIGEST_LENGTH) == 0) {
            printf("%s %s", hashes[i].filename, hashes[i + 1].filename);

            // check for a third
            if (i < fileCount - 2) {
                if (memcmp(hashes[i + 1].hash, hashes[i + 2].hash, MD5_DIGEST_LENGTH) == 0) {
                    printf(" %s", hashes[i + 2].filename);
                }
                i = i + 2;
            }
            printf("\n");

        }
    }

}

static int compare(const void *A, const void *B) {

    struct hashInfo* a = ((struct hashInfo*)A);
    struct hashInfo* b = ((struct hashInfo*)B);

    return strcmp((const char*) a->hash, (const char*) b->hash);
}

int main(int argc, char* argv[]) {

    // error checking
    if (argc != 2) {
        printf("Usage error - incorrect number of arguments\n");
    }

    char * path[] = { argv[1], NULL };


    // traverse the tree to count the number of files
    // print the file count
    int fileCount = countDirs(path);

    // Create an array of the file hashes
    struct hashInfo hashes[fileCount];
    memset(hashes, 0, sizeof hashes);

    // get the hashes
    getMD5(path, hashes);

    // Sort the hashes
    qsort(hashes, fileCount, sizeof(struct hashInfo), compare);

    // print to check in order
    /*for (int i = 0; i < fileCount; i++) {
        for (int j = 0; j < MD5_DIGEST_LENGTH; j++) {
            printf("%02x", hashes[i].hash[j]);
        }
        printf("\n");
    }*/

    // print duplicates
    printDup(hashes, fileCount);

    exit(EXIT_SUCCESS);
}

/**
 * References
 *  File generate-files.py written and provided by Scott Kuhl
 *  Programming examples from the following repository by Scott Kuhl
 *      https://github.com/skuhl/sys-prog-examples/tree/master/simple-examples
 *  Guide for getting md5 hashes
 *      http://stackoverflow.com/questions/1220046/how-to-get-the-md5-hash-of-a-file-in-c
 */
