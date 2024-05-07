/*
 *  main.c
 *  vanilla-json
 *
 *  Created by Uwe Vogt on 02.05.24.
 *  Copyright © 2024 UV Software. All rights reserved.
 */
#include "vanilla.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _MSC_VER
#define OPT_DUMPFILE_LONG   "/DUMPFILE:"
#define OPT_DUMPFILE_SHORT  "/D:"
#define OPT_DUMPFILE_ARG    ':'
#define OPT_VERBOSE_LONG    "/VERBOSE"
#define OPT_VERBOSE_SHORT   "/V"
#else
#define OPT_DUMPFILE_LONG   "--dumpfile="
#define OPT_DUMPFILE_SHORT  "-d="
#define OPT_DUMPFILE_ARG    '='
#define OPT_VERBOSE_LONG    "--verbose"
#define OPT_VERBOSE_SHORT   "-v"
#endif
#define MAX_BUFFER  16

struct options {
    char* jsonfile;
    char* dumpfile;
    int verbose;
};
int scan_commandline(int argc, char* argv[], struct options* opts);
void usage(char* program);
void traverse(json_node_t node, int level);

int main(int argc, char * argv[]) {
    json_node_t root;
    struct options opts;
    int rc = 0;
#if !defined(_MSC_VER)
    fprintf(stdout, "vanilla-json (%s %s %s)\n",__DATE__,__TIME__,__VERSION__);
#else
#if !defined(_WIN64)
    fprintf(stdout, "vanilla-json (%s %s MSC %i x86)\n",__DATE__,__TIME__,_MSC_VER);
#else
    fprintf(stdout, "vanilla-json (%s %s MSC %i x64)\n",__DATE__,__TIME__,_MSC_VER);
#endif
#endif
    memset(&opts, 0, sizeof(struct options));
    errno = 0;

    rc = scan_commandline(argc, argv, &opts);
    if ((rc != 0) || (opts.jsonfile == NULL)) {
        if (rc < 0)
            usage(argv ? argv[0] : NULL);
        else
            perror("error");
        return rc;
    }
    root = json_read(opts.jsonfile);
    if (root == NULL) {
        if (!errno)
            fprintf(stderr, "Error reading JSON file %s\n", opts.jsonfile);
        else
            perror("");
        return 1;
    }
    if (opts.dumpfile) {
        json_dump(root, opts.dumpfile);
    } else if (!opts.verbose) {
        json_dump(root, NULL);
    }
    else {
        traverse(root, 0);
    }
    json_free(root);

    return 0;
}
void traverse(json_node_t node, int level) {
    char* string;
    size_t length;
    char buffer[MAX_BUFFER + 1];
    jsize_t max = MAX_BUFFER;
    json_node_t curr;
    int i;

    /* recursion terminates here */
    if (!node)
        return;

    /* print level indentation */
    for (i = 0; i <= level; i++)
        fputc('+', stdout);
    fputc(' ', stdout);

    /* handel the JSON node */
    switch (json_get_value_type(node)) {
    case JSON_STRING:
        /* JSON string values: */
        /* - get a pointer to the content and copy the first n characters into a buffer */
        if ((string = json_get_string(node, buffer, max)) != NULL)
            length = strlen(string);
        else
            length = 0;
        /* - print the content of the buffer (possibly only a part of the string value) */
        fprintf(stdout, "found a JSON string of length %i at level %i: string = \"%s", (int)length, (level + 1), buffer);
        if (length > strlen(buffer))
            fprintf(stdout, "...\"\n");
        else
            fprintf(stdout, "\"\n");
        break;
        /* ! note: the application is resposible to handle escaped characters */
    case JSON_NUMBER:
        /* JSON number values: */
        /* - print the number as integer value (long) and as floating point value (double) */
        fprintf(stdout, "found a JSON number at level %i: integer = %li (floating point = %g)\n", (level + 1), 
                         json_get_integer(node, NULL, 0L), json_get_float(node, NULL, 0L));
        /* ! note: the application is resposible to handle the data type */
        break;
    case JSON_OBJECT:
        /* JSON objects: */
        /* - loop over all object member (if any) to determine the number of members */
        i = 0;
        curr = json_get_value_first(node);
        while (curr) {
            i++;
            curr = json_get_value_next(node);
        }
        /* - print the number of object members */
        fprintf(stdout, "found a JSON object with %i member(s) at level %i\n", i, (level + 1));
        /* - loop over all object member (if any): */
        curr = json_get_value_first(node);
        while (curr) {
            /* -- print level indentation */
            for (i = 0; i <= level; i++)
                fputc('-', stdout);
            /* -- print the key (string) of the current object member */
            fprintf(stdout, "> object member \"%s\":\n", json_get_object_string(node));
            /* -- traverse the current object member (recursive) */
            traverse(curr, level + 1);
            /* -- next please */
            curr = json_get_value_next(node);
        }
        break;
    case JSON_ARRAY:
        /* JSON arrays: */
        /* - loop over all array elements (if any) to determine the number of elements */
        i = 0;
        curr = json_get_value_first(node);
        while (curr) {
            i++;
            curr = json_get_value_next(node);
        }
        /* - print the number of array elements */
        fprintf(stdout, "found a JSON array with %i element(s) at level %i\n", i, (level + 1));
        /* - loop over all array elements (if any): */
        curr = json_get_value_first(node);
        while (curr) {
            /* -- print level indentation */
            for (i = 0; i <= level; i++)
                fputc('-', stdout);
            /* -- print the index of the current array element */
            fprintf(stdout, "> array index %i:\n", json_get_array_index(node));
            /* -- traverse the current array element (recursive) */
            traverse(curr, level + 1);
            /* -- next please */
            curr = json_get_value_next(node);
        }
        break;
    case JSON_TRUE:
        /* JSON value "true" */
        /* - print the C value of JSON true */
        fprintf(stdout, "found a JSON true at level %i: true = %i\n", (level + 1), json_get_bool(node, NULL, 0L));
        break;
    case JSON_FALSE:
        /* JSON value "false" */
        /* - print the C value of JSON false */
        fprintf(stdout, "found a JSON false at level %i: false = %i\n", (level + 1), json_get_bool(node, NULL, 0L));
        break;
    case JSON_NULL:
        /* JSON value "null" */
        /* - print the C value of JSON null */
        fprintf(stdout, "found a JSON null at level %i: null = %s\n", (level + 1), (json_get_null(node, NULL, 0L) == NULL) ? "NULL" : "?");
        break;
    case JSON_ERROR:
        /* this should never happen ! */
        fprintf(stdout, "discovered an error node at level %i\n", (level + 1));
        break;
    default:
        /* something went terribly wrong ? */
        fprintf(stdout, "discovered something terribly wrong at level %i\n", (level + 1));
        break;
    }
}

int scan_commandline(int argc, char* argv[], struct options* opts) {
    int i; char* ptr;

    if ((argc <= 1) || (4 < argc) || !argv || !opts) {
        errno = EINVAL;
        return (-1);
    }
    errno = 0;
    opts->jsonfile = NULL;
    opts->dumpfile = NULL;
    opts->verbose = 0;

    for (i = 1; i < argc; i++) {
        if (!strncmp(argv[i], OPT_DUMPFILE_LONG, strlen(OPT_DUMPFILE_LONG)) || 
            !strncmp(argv[i], OPT_DUMPFILE_SHORT, strlen(OPT_DUMPFILE_SHORT))) {
            if (opts->dumpfile) {
                errno = EINVAL;
                return (-1);
            }
            ptr = strchr(argv[i], OPT_DUMPFILE_ARG);
            if (!ptr || (*(++ptr) == '\0')) {
                errno = EINVAL;
                return (-1);
            }
            opts->dumpfile = ptr;
        }
        else if (!strcmp(argv[i], OPT_VERBOSE_LONG) || 
                 !strcmp(argv[i], OPT_VERBOSE_SHORT)) {
            if (opts->verbose) {
                errno = EINVAL;
                return (-1);
            }
            opts->verbose = 1;
        }
        else {
            if (opts->jsonfile) {
                errno = EINVAL;
                return (-1);
            }
            opts->jsonfile = argv[i];
        }
    }
    return 0;
}

#ifdef _MSC_VER
char* basename(char* path) {
    static const char exe[] = "agimus.exe";
    char* ptr = NULL;
    if (path)
        ptr = strrchr(path, '\\');
    if (ptr)
        ptr++;
    return (ptr ? ptr : exe);
}
void usage(char* program) {
    fprintf(stderr, "Usaage: %s <jsonfile> [/Dumpfile:<dumpfile>] [/Verbose]\n", basename(program));
}
#else
#include <libgen.h>  /* see man basename(3) */
void usage(char* program) {
    fprintf(stderr, "Usaage: %s [--verbose] [--dumpfile=<dumpfile>] <jsonfile>\n", basename(program));
}
#endif
