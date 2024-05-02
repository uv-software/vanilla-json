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

    json_node_t root = json_read("/Users/neptune/Projects/vanilla/test.json");

    json_dump(root, "/Users/neptune/Projects/vanilla/dump.json");

    json_free(root);
}
