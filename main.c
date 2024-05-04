/*
 *  main.c
 *  vanilla-json
 *
 *  Created by Uwe Vogt on 02.05.24.
 *  Copyright © 2024 UV Software. All rights reserved.
 */
#include "vanilla.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, const char * argv[]) {
    json_node_t root;

    if ((argc < 2) || (3 < argc)) {
        fprintf(stderr, "Usage: %s <jsonfile> [<dumpfile>]\n", argv[0]);
        return 1;
    }
    root = json_read(argv[1]);
    if (root == NULL) {
        fprintf(stderr, "Error reading JSON file %s\n", argv[1]);
        perror("+++ error");
        return 1;
    }
    if (argc > 2) {
        json_dump(root, argv[2]);
    } else {
        json_dump(root, NULL);
    }
    json_free(root);

    return 0;
}
