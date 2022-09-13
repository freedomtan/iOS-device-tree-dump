// code adapted from Jonathan Levin's http://www.newosxbook.com/src.jl?tree=listings&file=6-bonus.c
// clean-up a bit to
// 1. make "clang -Wall" happy
// 2. remove img3 stuff: device tree files are in im4p nowadays

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dt.h"		   // for DeviceTree

int verbose = 0;

void copyValue (char *dest, char *src, int length)
{
    int i = 0;
    for (i = 0; src[i] || i < length; i++);
    
    if (i != length) {
        strcpy(dest, "(null)");
        return;
    }
    memcpy(dest, src, length);
}

uint32_t dumpTreeNode(DeviceTreeNode *Node, int indent)
{
    char buffer[819200];
    char temp[409600];
    char h_temp[81920];
    char *name;
    
    int prop = 0, child = 0;
    int i = 0;
    memset(buffer, '\0', 819200);
    
    DeviceTreeNodeProperty *dtp = (DeviceTreeNodeProperty * ) ((char*)Node + sizeof(DeviceTreeNode));
    
    char *offset = 0;
    int real_len;
    for (prop = 0; prop < Node->nProperties; prop++) {
        real_len = dtp->length;
        temp[0] = '\0'; // strcat will do the rest
        for (i=0; i< indent ; i++) {
            strcat(temp,"|  ");
        }
        strcat(temp, "+--");
        strncat(buffer, temp, 1024);
        if ((real_len & 0x80000000) > 0)
            real_len = real_len - 0x80000000;
        sprintf(temp, "%s %d bytes: ", dtp->name, real_len);
        strncat(buffer, temp, 1024);
        
        if (strcmp(dtp->name,"name") == 0) {
            name = (char *) &dtp->length + sizeof(uint32_t);
            strncat(buffer, name, dtp->length);
            strcat(buffer,"\n");
        } else {
            copyValue(temp, ((char *) &dtp->length) + sizeof(uint32_t), real_len);
            // Yeah, Yeah, Buffer overflows, etc.. :-)
            if (verbose) {
                char *hex = h_temp;
                for (i=0; i < real_len; i++) {
                    sprintf(hex, " 0x%02x", 0xff & *(((char *) &dtp->length) + sizeof(uint32_t) + i));
                    hex += 5; // len(" 0x??") = 5
                }
            }
            strncat(buffer, temp, real_len);
            if (verbose)
                strcat(buffer, h_temp);
            strcat(buffer, "\n");
        }
        dtp = (DeviceTreeNodeProperty *) (((char *) dtp) + sizeof(DeviceTreeNodeProperty) + real_len);
        
        // Align
        dtp =  (((long) dtp %4) ? (DeviceTreeNodeProperty *) (((char *) dtp)  + (4 - ((long)dtp) %4)): dtp);
        offset = (char *) dtp;
    }
    
    for (i=0; i < indent-1; i++) {
        printf("   ");
    }
    
    if (indent > 1)
        printf ("+--");
    printf("%s:\n", name);
    printf("%s", buffer);
    
    // Now do children:
    for (child = 0; child < Node->nChildren; child++) {
        offset+= dumpTreeNode((DeviceTreeNode *) offset, indent+1);
    }
    
    return ( (char *) offset - (char*) Node);
}


int 
main(int argc, char **argv)
{
    struct stat	stbuf;
    char *filename;
    int rc;
    int fd;
    int filesize;
    char *mmapped;
    
    if (argc < 2) {
        fprintf (stderr,"Usage: %s [-v] _filename_\n", argv[0]);
        exit(0);
    }
    
    if (strcmp(argv[1], "-v") == 0)
        verbose = 1;
    
    filename = argv[argc -1];
    
    rc = stat(filename, &stbuf);
    
    if (rc == -1) { perror (filename); exit (1); }
    
    filesize = stbuf.st_size;
    
    fd = open (filename, O_RDONLY);
    if (fd < 0) { perror (filename); exit(2);}
    
    mmapped = mmap(NULL,
                   filesize,  // size_t len,
                   PROT_READ, // int prot,
                   MAP_SHARED | MAP_FILE,  // int flags,
                   fd,        // int fd,
                   0);        // off_t offset);
    
    if (!mmapped) {
        perror ("mmap");
        exit(3);
    }
    
    char *data = mmapped;
    DeviceTreeNode *dtn = (DeviceTreeNode *) data;
    printf ("\tDevice Tree with %d properties and %d children\n", dtn->nProperties, dtn->nChildren);
    
    printf("Properties:\n");
    dumpTreeNode (dtn,1);
    
    return 0;
}

