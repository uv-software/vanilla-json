//
//  main.c
//  vanilla
//
//  Created by Uwe Vogt on 02.05.24.
//  Copyright © 2024 UV Software. All rights reserved.
//

#include <stdio.h>
#include "vanilla.h"

int main(int argc, const char * argv[]) {
    (void)argc;
    (void)argv;

    json_node_t root = json_read("test.json");
#if (0)
    json_dump(root, "dump.json");
#else
    json_dump(root, NULL);
#endif
    json_free(root);

    return 0;
}
