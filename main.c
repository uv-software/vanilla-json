/*
 *  main.c
 *  vanilla
 *
 *  Created by Uwe Vogt on 02.05.24.
 *  Copyright © 2024 UV Software. All rights reserved.
 */

#include "vanilla.h"
#include <stdio.h>

int main(int argc, const char * argv[]) {
    json_node_t root;
    (void)argc;
    (void)argv;

    root = json_read("test.json");
#if (0)
    json_dump(root, "dump.json");
#else
    json_dump(root, NULL);
#endif
    json_free(root);

    return 0;
}
