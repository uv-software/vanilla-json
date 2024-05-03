/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  Vanilla-JSON - A very simple JSON Parser
 *
 *  Copyright (c) 2024 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  This file is part of Vanilla-JSON.
 *
 *  Vanilla-JSON is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v3.0 (or any later version).
 *  You can choose between one of them if you use this file.
 *
 *  BSD 2-Clause "Simplified" License:
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  Vanilla-JSON IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF Vanilla-JSON, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  GNU General Public License v3.0 or later:
 *  Vanilla-JSON is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vanilla-JSON is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vanilla-JSON.  If not, see <https://www.gnu.org/licenses/>.
 */
/** @file        vanilla.c
 *
 *  @brief       Vanilla-JSON - A very simple JSON Parser
 *
 *  @author      $Author$
 *
 *  @version     $Rev$
 *
 *  @addtogroup  json
 *  @{
 */
#ifdef _MSC_VER
 //no Microsoft extensions please!
 #ifndef _CRT_SECURE_NO_WARNINGS
 #define _CRT_SECURE_NO_WARNINGS 1
 #endif
#endif

/*  -----------  includes  -----------------------------------------------
 */
#include "vanilla.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
#define TAB_SIZE  8
#if (DEBUG_VALUE != 0)
#define DEBUG_STRING(str)  do { printf(">>> string(%d): \"%s\"\n", (int)strlen(str), str); } while(0)
#define DEBUG_NUMBER(str)  do { printf(">>> number: %s\n", str); } while(0)
#define DEBUG_LITERAL(str) do { printf(">>> literal: %s\n", str); } while(0)
#if (NOT_RECURSIVE != 0)
#define DEBUG_ARRAY(node)  do { dump_array(node, 0, stdout); } while(0)
#define DEBUG_OBJECT(node) do { dump_object(node, 0, stdout); } while(0)
#else
#define DEBUG_ARRAY(node)  do { } while(0)
#define DEBUG_OBJECT(node) do { } while(0)
#endif
#else
#define DEBUG_STRING(str)  do { } while(0)
#define DEBUG_NUMBER(str)  do { } while(0)
#define DEBUG_LITERAL(str) do { } while(0)
#define DEBUG_ARRAY(node)  do { } while(0)
#define DEBUG_OBJECT(node) do { } while(0)
#endif

/*  -----------  types  --------------------------------------------------
 */
typedef struct json_file {              /* JSON file content: */
    char* buf;                          /* - string buffer (entire file) */
    long len;                           /* - length of the buffer/file */
    long pos;                           /* - current read position */
    long row;                           /* - current line number */
    long col;                           /* - current column number */
} json_file_t, *JSON;

/*  -----------  prototypes  ---------------------------------------------
 */
static json_node_t parse_value(JSON json);
static json_node_t parse_string(JSON json);
static json_node_t parse_number(JSON json);
static json_node_t parse_object(JSON json);
static json_node_t parse_array(JSON json);
static json_node_t parse_literal(JSON json, json_type_t type);
static void dump_value(json_node_t node, int depth, FILE* fp);
static void dump_string(json_node_t node, int depth, FILE* fp);
static void dump_number(json_node_t node, int depth, FILE* fp);
static void dump_object(json_node_t node, int depth, FILE* fp);
static void dump_array(json_node_t node, int depth, FILE* fp);
static void dump_literal(json_node_t node, int depth, FILE* fp);
static void dump_value(json_node_t node, int depth, FILE* fp);
static void free_value(json_node_t node);
static void free_string(json_node_t node);
static void free_number(json_node_t node);
static void free_object(json_node_t node);
static void free_array(json_node_t node);
static void free_literal(json_node_t node);
static char* get_string(JSON json);
static long scan_string(JSON json);
static long scan_number(JSON json);
static long scan_literal(JSON json, const char* literal);
static char get_char(JSON json);
static char lookahead(JSON json);

/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */
json_node_t json_read(const char* filename) {
    json_node_t root = NULL;
    json_file_t file;
    FILE* fp = NULL;

    errno = 0;
    (void)memset(&file, 0, sizeof(json_file_t));
    /* (1) open the file */
    if ((fp = fopen(filename, "rb")) == NULL) {
        /* errno set */
        return NULL;
    }
    /* (2) determine its size */
    if (fseek(fp, 0, SEEK_END) != 0) {
        /* errno set */
        (void)fclose(fp);
        return NULL;
    }
    if ((file.len = ftell(fp)) < 0) {
        /* errno set */
        (void)fclose(fp);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        /* errno set */
        (void)fclose(fp);
        return NULL;
    }
    /* (3) read its content into a buffer */
    if ((file.buf = (char*)malloc((size_t)file.len + 1)) == NULL) {
        /* errno set */
        (void)fclose(fp);
        return NULL;
    }
    if (fread(file.buf, sizeof(char), (size_t)file.len, fp) != (size_t)file.len) {
        /* errno set */
        free(file.buf);
        (void)fclose(fp);
        return NULL;
    }
    /* (4) close it again */
    if (fclose(fp) != 0) {
        /* errno set */
        free(file.buf);
        return NULL;
    }
    /* (5) parse the content of the file */
    file.buf[file.len] = '\0';
    root = parse_value(&file);
    free(file.buf);
    return root;
}

void json_free(json_node_t node) {
     free_value(node);
}

json_type_t json_get_value_type(json_node_t node) {
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return JSON_ERROR;
    }
    return node->type;
}

json_node_t json_get_value_of(const char* string, json_node_t node) {
    struct json_member* curr = NULL;
    struct json_node* value = NULL;
    errno = 0;
    if (!node || !string) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->type != JSON_OBJECT) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->value.dict) {
        curr = node->value.dict;
        while ((curr != NULL) && (curr->string != NULL) && (strcmp(curr->string, string)))
            curr = curr->next;
        if ((curr != NULL) && (curr->string != NULL) && (!strcmp(curr->string, string)))
            value = curr->value;
        else
            errno = EINVAL;  /* FIXME: error code */
    }
    return value;
}

json_node_t json_get_value_at(int index, json_node_t node) {
    struct json_element* curr = NULL;
    struct json_node* value = NULL;
    errno = 0;
    if (!node || (index < 0)) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->type != JSON_ARRAY) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->value.array) {
        curr = node->value.array;
        while ((curr != NULL) && (curr->index != index))
            curr = curr->next;
        if ((curr != NULL) && (curr->index == index))
            value = curr->value;
        else
            errno = EINVAL;  /* FIXME: error code */
    }
    return value;
}

char* json_get_string(json_node_t node, char* buffer, jsize_t length) {
    jsize_t i = (jsize_t)0;
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->type != JSON_STRING) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (!node->value.string) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (buffer && (length > 0UL)) {
        do {
            buffer[i] = node->value.string[i];
            i++;
        } while ((node->value.string[(i - 1)] != '\0') && (i < length));
        buffer[(length - 1)] = '\0';
    }
    return node->value.string;
}

char* json_get_number(json_node_t node, char* buffer, jsize_t length) {
    jsize_t i = (jsize_t)0;
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->type != JSON_NUMBER) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (!node->value.string) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (buffer && (length > 0UL)) {
        do {
            buffer[i] = node->value.string[i];
            i++;
        } while ((node->value.string[(i - 1)] != '\0') && (i < length));
        buffer[(length - 1)] = '\0';
    }
    return node->value.string;
}

long json_get_integer(json_node_t node, char* buffer, jsize_t length) {
    jsize_t i = (jsize_t)0;
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return 0L;
    }
    if (node->type != JSON_NUMBER) {
        errno = EINVAL;  /* FIXME: error code */
        return 0L;
    }
    if (!node->value.string) {
        errno = EINVAL;  /* FIXME: error code */
        return 0L;
    }
    if (buffer && (length > 0UL)) {
        do {
            buffer[i] = node->value.string[i];
            i++;
        } while ((node->value.string[(i - 1)] != '\0') && (i < length));
        buffer[(length - 1)] = '\0';
    }
    return atol(node->value.string);
}
double json_get_float(json_node_t node, char* buffer, jsize_t length) {
    jsize_t i = (jsize_t)0;
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return 0.0;
    }
    if (node->type != JSON_NUMBER) {
        errno = EINVAL;  /* FIXME: error code */
        return 0.0;
    }
    if (!node->value.string) {
        errno = EINVAL;  /* FIXME: error code */
        return 0.0;
    }
    if (buffer && (length > 0UL)) {
        do {
            buffer[i] = node->value.string[i];
            i++;
        } while ((node->value.string[(i - 1)] != '\0') && (i < length));
        buffer[(length - 1)] = '\0';
    }
    return atof(node->value.string);
}

int json_get_bool(json_node_t node, char* buffer, jsize_t length) {
    jsize_t i = (jsize_t)0;
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return 0;
    }
    if ((node->type != JSON_TRUE) && (node->type != JSON_FALSE)) {
        errno = EINVAL;  /* FIXME: error code */
        return 0;
    }
    if (!node->value.string) {
        errno = EINVAL;  /* FIXME: error code */
        return 0;
    }
    if (buffer && (length > 0UL)) {
        do {
            buffer[i] = node->value.string[i];
            i++;
        } while ((node->value.string[(i - 1)] != '\0') && (i < length));
        buffer[(length - 1)] = '\0';
    }
    return (node->type != JSON_TRUE) ? 1 : 0;
}

void* json_get_null(json_node_t node, char* buffer, jsize_t length) {
    jsize_t i = (jsize_t)0;
    errno = 0;
    if (!node) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (node->type != JSON_NULL) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (!node->value.string) {
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
    if (buffer && (length > 0UL)) {
        do {
            buffer[i] = node->value.string[i];
            i++;
        } while ((node->value.string[(i - 1)] != '\0') && (i < length));
        buffer[(length - 1)] = '\0';
    }
    return NULL;
}

int json_dump(json_node_t node, const char* filename) {
    FILE* fp = NULL;
    errno = 0;
    if (filename) {
        if ((fp = fopen(filename, "w")) == NULL) {
            /* errno set */
            return errno;
        }
        dump_value(node, (-1), fp);
        fputc('\n', fp); fflush(fp);
        (void)fclose(fp);
    } else {
        dump_value(node, (-1), stdout);
        putchar('\n');
    }
    return 0;
}

/*  -----------  local functions  ----------------------------------------
 */
static char get_char(JSON json) {
    assert(json);
    assert(json->buf);
    assert(json->len >= 1);
    assert(json->pos <= json->len);

    if (json->pos < json->len) {
        if (json->buf[json->pos] == '\n') {
            json->col = 0;
            json->row++;
        }
        else if (json->buf[json->pos] == '\t') {
            json->col = ((json->col % TAB_SIZE) + 1) * TAB_SIZE;
        }
        else {
            json->col++;
        }
        return json->buf[json->pos++];
    }
    return 0;
}

/*  <whitespace> : ' ' | '\n' | '\r' | '\t'
 *               ;
 */
static char lookahead(JSON json) {
    assert(json);
    assert(json->buf);
    assert(json->len >= 1);
    assert(json->pos <= json->len);

    while ((json->pos < json->len) &&
          ((json->buf[json->pos] == ' ') || (json->buf[json->pos] == '\n') ||
          (json->buf[json->pos] == '\r') || (json->buf[json->pos] == '\t'))) {
        if (json->buf[json->pos] == '\n') {
            json->col = 0;
            json->row++;
        }
        else if (json->buf[json->pos] == '\t') {
            json->col = ((json->col % TAB_SIZE) + 1) * TAB_SIZE;
        }
        else {
            json->col++;
        }
        json->pos++;
    }
    return json->buf[json->pos];
}

/*  <value>      : <object>
 *               | <array>
 *               | <string>
 *               | <number>
 *               | "true"
 *               | "false"
 *               | "null"
 *               ;
 */
static json_node_t parse_value(JSON json) {
    char ch = lookahead(json);
    switch (ch) {
    case '{':
        return parse_object(json);
    case '[':
        return parse_array(json);
    case '"':
        return parse_string(json);
    case '-':
    case '0': case '1': case '2':  case '3': case '4':
    case '5': case '6': case '7':  case '8': case '9':
        return parse_number(json);
    case 't':
        return parse_literal(json, JSON_TRUE);
    case 'f':
        return parse_literal(json, JSON_FALSE);
    case 'n':
        return parse_literal(json, JSON_NULL);
    default:
        errno = EINVAL;  /* FIXME: error code */
        return NULL;
    }
}

static void free_value(json_node_t node) {
    if (node) {
        switch (node->type) {
        case JSON_OBJECT: free_object(node); break;
        case JSON_ARRAY: free_array(node); break;
        case JSON_STRING: free_string(node); break;
        case JSON_NUMBER: free_number(node); break;
        case JSON_TRUE: free_literal(node); break;
        case JSON_FALSE: free_literal(node); break;
        case JSON_NULL: free_literal(node); break;
        default: break;
        }
    }
}

static void dump_value(json_node_t node, int depth, FILE* fp) {
    if (node) {
        switch (node->type) {
        case JSON_OBJECT: dump_object(node, depth + 1, fp); break;
        case JSON_ARRAY: dump_array(node, depth + 1, fp); break;
        case JSON_STRING: dump_string(node, depth + 1, fp); break;
        case JSON_NUMBER: dump_number(node, depth + 1, fp); break;
        case JSON_TRUE: dump_literal(node, depth + 1, fp); break;
        case JSON_FALSE: dump_literal(node, depth + 1, fp); break;
        case JSON_NULL: dump_literal(node, depth + 1, fp); break;
        default: break;
        }
    }
}

/*  <object>     : '{' '}'
 *               | '{' <members> '}'
 *               ;
 *  <members>    : <member>
 *               | <member> ',' <members>
 *               ;
 *  <member>     : <string> ':' <value>
 *               ;
 */
static json_node_t parse_object(JSON json) {
    struct json_node* node = NULL;
    struct json_node* value = NULL;
    struct json_member* curr = NULL;
    struct json_member* next = NULL;
    char* string = NULL;
    
    if (get_char(json) != '{') {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if ((node = (struct json_node*)malloc(sizeof(struct json_node))) == NULL) {
        /* errno set */
        return NULL;
    }
    node->type = JSON_OBJECT;
    node->value.dict = NULL;
    node->value.string = NULL;
    /* get member (optional) */
    if ((string = get_string(json)) != NULL) {
        if (lookahead(json) == '\0') {
            free(string);
            free(node);
            errno = EINVAL; /* FIXME: error code */
            return NULL;
        }
        if (get_char(json) != ':') {
            free(string);
            free(node);
            errno = EINVAL; /* FIXME: error code */
            return NULL;
        }
        /* get member value (as JSON value) */
        value = parse_value(json);
        if (value == NULL) {
            /* errno set */
            free(string);
            free(node);
            return NULL;
        }
        if ((curr = (struct json_member*)malloc(sizeof(struct json_member))) == NULL) {
            /* errno set */
            free_value(value);
            free(string);
            free(node);
            return NULL;
        }
        curr->string = string;
        curr->value = value;
        curr->next = NULL;
        node->value.dict = curr;
        /* loop over obect members, if more */
        while (lookahead(json) == ',') {
            if (get_char(json) != ',') {
                free_object(node);
                errno = EINVAL; /* FIXME: error code */
                return NULL;
            }
            /* get member key (as string) */
            string = get_string(json);
            if (string == NULL) {
                /* errno set */
                free_object(node);
                return NULL;
            }
            if (lookahead(json) == '\0') {
                free(string);
                free_object(node);
                errno = EINVAL; /* FIXME: error code */
                return NULL;
            }
            if (get_char(json) != ':') {
                free(string);
                free_object(node);
                errno = EINVAL; /* FIXME: error code */
                return NULL;
            }
            /* get member value (as JSON value) */
            value = parse_value(json);
            if (value == NULL) {
                /* errno set */
                free(string);
                free_object(node);
                return NULL;
            }
            if ((next = (struct json_member*)malloc(sizeof(struct json_member))) == NULL) {
                /* errno set */
                free(string);
                free_value(value);
                free_object(node);
                return NULL;
            }
            next->string = string;
            next->value = value;
            next->next = NULL;
            curr->next = next;
            /* get next member, if any */
            curr = next;
        }
    } else {
        /* skip whitespaces */
        (void)lookahead(json);
    }
    if (get_char(json) != '}') {
        free_object(node);
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    DEBUG_OBJECT(node);
    return node;
}

static void free_object(json_node_t node) {
    struct json_member* curr = NULL;
    struct json_member* temp = NULL;

    if (node && (node->type == JSON_OBJECT)) {
        curr = node->value.dict;
        while (curr) {
            temp = curr;
            curr = temp->next;
            if (temp) {
                if (temp->value)
                    free_value(temp->value);
                if (temp->string)
                    free(temp->string);
                free(temp);
            }
        }
        free(node);
    }
}

static void dump_object(json_node_t node, int depth, FILE* fp) {
    struct json_member* curr = NULL;
    int i;
    assert(fp);
    if (node && (node->type == JSON_OBJECT)) {
        /* opening bracket */
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        fputc('{', fp); fputc('\n', fp);
        if (node->value.dict) {
            /* first member */
            curr = node->value.dict;
            for (i = 0; i < depth + 1; i++) {
                fputc(' ', fp); fputc(' ', fp);
            }
            if (curr->string)
                fprintf(fp, "\"%s\"", curr->string);
            fputc(':', fp); fputc('\n', fp);
            if (curr->value)
                dump_value(curr->value, depth + 2, fp);
            /* other members, if any */
            curr = curr->next;
            while (curr != NULL) {
                fputc(',', fp); fputc('\n', fp);
                for (i = 0; i < depth + 1; i++) {
                    fputc(' ', fp); fputc(' ', fp);
                }
                if (curr->string)
                    fprintf(fp, "\"%s\"", curr->string);
                fputc(':', fp); fputc('\n', fp);
                if (curr->value)
                    dump_value(curr->value, depth + 2, fp);
                curr = curr->next;
            }
        }
        fputc('\n', fp);
        /* closing bracket */
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        fputc('}', fp);
        fflush(fp);
    }
}

/*  <array>      : '[' ']'
 *               | '[' <elements> ']'
 *               ;
 *  <elements>   : <element>
 *               | <element> ',' <elements>
 *               ;
 *  <element>    : <value>
 *               ;
 */
static json_node_t parse_array(JSON json) {
    struct json_node* node = NULL;
    struct json_node* value = NULL;
    struct json_element* curr = NULL;
    struct json_element* next = NULL;
    int index = 0;

    if (get_char(json) != '[') {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if ((node = (struct json_node*)malloc(sizeof(struct json_node))) == NULL) {
        /* errno set */
        return NULL;
    }
    node->type = JSON_ARRAY;
    node->value.array = NULL;
    /* first element (optional) */
    if ((value = parse_value(json)) != NULL) {
        if ((curr = (struct json_element*)malloc(sizeof(struct json_element))) == NULL) {
            /* errno set */
            free_value(value);
            free(node);
            return NULL;
        }
        curr->index = index++;
        curr->value = value;
        curr->next  = NULL;
        node->value.array = curr;
        /* loop over array elements, if more */
        while (lookahead(json) == ',') {
            if (get_char(json) != ',') {
                free_array(node);
                errno = EINVAL; /* FIXME: error code */
                return NULL;
            }
            if ((value = parse_value(json)) == NULL) {
                /* errno set */
                free_array(node);
                return NULL;
            }
            if ((next = (struct json_element*)malloc(sizeof(struct json_element))) == NULL) {
                /* errno set */
                free_value(value);
                free_array(node);
                return NULL;
            }
            next->index = index++;
            next->value = value;
            next->next = NULL;
            curr->next = next;
            /* get next element, if any */
            curr = next;
        }
    } else {
        /* skip whitespaces */
        (void)lookahead(json);
    }
    if (get_char(json) != ']') {
        free_array(node);
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    DEBUG_ARRAY(node);
    return node;
}

static void free_array(json_node_t node) {
    struct json_element* curr = NULL;
    struct json_element* temp = NULL;

    if (node && (node->type == JSON_ARRAY)) {
        curr = node->value.array;
        while (curr) {
            temp = curr;
            curr = temp->next;
            if (temp) {
                if (temp->value)
                    free_value(temp->value);
                free(temp);
            }
        }
        free(node);
    }
}

static void dump_array(json_node_t node, int depth, FILE* fp) {
    struct json_element* curr = NULL;
    int i;
    assert(fp);
    if (node && (node->type == JSON_ARRAY)) {
        /* opening bracket */
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        fputc('[', fp); fputc('\n', fp);
        if (node->value.array) {
            /* first element */
            curr = node->value.array;
            if (curr->value)
                dump_value(curr->value, depth + 1, fp);
            /* other elements, if any */
            curr = curr->next;
            while (curr != NULL) {
                fputc(',', fp); fputc('\n', fp);
                if (curr->value)
                    dump_value(curr->value, depth + 1, fp);
                curr = curr->next;
            }
        }
        fputc('\n', fp);
        /* closing bracket */
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        fputc(']', fp);
        fflush(fp);
    }
}

/*  <string>     : '"' characters '"'
 *               ;
 *  <characters> :
 *               | <character> <characters>
 *               ;
 *  <character>  : '0020' .. '10FFFF' - '"' - '\'
 *               | '\' <escape>
 *               ;
 *  <escape>     : '"' | '\' | '/' | 'b' | 'f' | 'n' | 'r' | 't'
 *               | 'u' hex hex hex hex
 *               ;
 *  <hex>        | <digit>
 *               | <'A' .. 'F'
 *               | <'a' .. 'f'
 *               ;
 *  <digits>     : <digit>
 *               | <digit> <digits>
 *               ;
 *  <digit>      : '0'
 *               | <onenine>
 *               ;
 *  <onenine>    : '1' .. '9'
 *               ;
 */
static long scan_string(JSON json) {
    long len = 0;
    long idx = 0;
    assert(json);
    assert(json->buf);
    assert(json->len >= 1);
    assert(json->pos <= json->len);

    idx = json->pos;
    while ((idx < json->len) && (json->buf[idx] != '"')) {
        /* escape '\"' (ignore the other escape sequences) */
        if (json->buf[idx] == '\\') {
            len++;
            idx++;
            if ((idx < json->len) && (json->buf[idx] == '"')) {
                len++;
                idx++;
            }
        } else {
            len++;
            idx++;
        }
    }
    return len;
}

static char* get_string(JSON json) {
    char* string = NULL;
    long length = 0;

    if (lookahead(json) != '"') {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if (get_char(json) != '"') {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if ((length = scan_string(json)) < 0) {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if ((string = (char*)malloc((size_t)((unsigned long)length + 1UL))) == NULL) {
        /* errno set */
        return NULL;
    }
    if ((string = strncpy(string, &json->buf[json->pos], (size_t)length)) != NULL) {
        string[length] = '\0';
        json->pos += length;
        json->col += length;
    }
    if (get_char(json) != '"') {
        errno = EINVAL; /* FIXME: error code */
        free(string);
        return NULL;
    }
    return string;
}

static json_node_t parse_string(JSON json) {
    struct json_node* node = NULL;
    char* string = NULL;

    if ((string = get_string(json)) == NULL) {
        /* errno set */
        return NULL;
    }
    if ((node = (struct json_node*)malloc(sizeof(struct json_node))) == NULL) {
        /* errno set */
        free(string);
        return NULL;
    }
    node->type = JSON_STRING;
    node->value.string = (char*)string;
    DEBUG_STRING(node->value.string);
    return node;
}

static void free_string(json_node_t node) {
    if (node && (node->type == JSON_STRING)) {
        if (node->value.string)
            free(node->value.string);
        free(node);
    }
}

static void dump_string(json_node_t node, int depth, FILE* fp) {
    int i;
    assert(fp);
    if (node && (node->type == JSON_STRING)) {
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        if (node->value.string)
            fprintf(fp, "\"%s\"", node->value.string);
        fflush(fp);
    }
}

/*  <number>     : <integer> <fraction> <exponent>
 *               ; 
 *  <integer>    : <digit>
 *               | <onenine> <digits>
 *               | '-' <digit>
 *               | '-' <onenine> <digits>
 *               ;
 *  <digits>     : <digit>
 *               | <digit> <digits>
 *               ;
 *  <digit>      : '0'
 *               | <onenine>
 *               ;
 *  <onenine>    : '1' .. '9'
 *               ;
 *  <fraction>   :
 *               | '.' digits
 *               ;
 *  <exponent>   :
 *               | 'E' sign digits
 *               | 'e' sign digits
 *               ;
 *  <sign>       :
 *               | '+'
 *               | '-'
 *               ;
 */
static long scan_number(JSON json) {
    long len = 0;
    long idx = 0;
    assert(json);
    assert(json->buf);
    assert(json->len >= 1);
    assert(json->pos <= json->len);

    idx = json->pos;
    /* the zero value */
    if ((idx < json->len) && (json->buf[idx] == '0')) {
        return 1;
    }
    /* negative value? */
    if ((idx < json->len) && (json->buf[idx] == '-')) {
        len++;
        idx++;
    }
    /* one to nine (mandatory) */
    if ((idx < json->len) && (('1' <= json->buf[idx]) && (json->buf[idx] <= '9'))) {
        len++;
        idx++;
    }
    else {
        return (-1);
    }
    /* zero to nine (optional) */
    while ((idx < json->len) && (('0' <= json->buf[idx]) && (json->buf[idx] <= '9'))) {
        len++;
        idx++;
    }
    /* fraction (optional) */
    if ((idx < json->len) && (json->buf[idx] == '.')) {
        len++;
        idx++;
        /* zero to nine (mandatory) */
        if ((idx < json->len) && (('0' <= json->buf[idx]) && (json->buf[idx] <= '9'))) {
            len++;
            idx++;
        }
        else {
            return (-1);
        }
        /* zero to nine (optional) */
        while ((idx < json->len) && (('0' <= json->buf[idx]) && (json->buf[idx] <= '9'))) {
            len++;
            idx++;
        }
    }
    /* exponent (optional) */
    if ((idx < json->len) && ((json->buf[idx] == 'e') || (json->buf[idx] == 'E'))) {
        len++;
        idx++;
        /* sign (optional) */
        if ((idx < json->len) && ((json->buf[idx] == '+') || (json->buf[idx] == '-'))) {
            len++;
            idx++;
        }
        /* zero to nine (mandatory) */
        if ((idx < json->len) && (('0' <= json->buf[idx]) && (json->buf[idx] <= '9'))) {
            len++;
            idx++;
        }
        else {
            return (-1);
        }
        /* zero to nine (optional) */
        while ((idx < json->len) && (('0' <= json->buf[idx]) && (json->buf[idx] <= '9'))) {
            len++;
            idx++;
        }
    }
    return len;
}

static json_node_t parse_number(JSON json) {
    struct json_node* node = NULL;
    char* string = NULL;
    long length = 0;

    if ((length = scan_number(json)) <= 0) {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if ((string = (char*)malloc((size_t)((unsigned long)length + 1UL))) == NULL) {
        /* errno set */
        return NULL;
    }
    if ((string = strncpy(string, &json->buf[json->pos], (size_t)length)) != NULL) {
        string[length] = '\0';
        json->pos += length;
        json->col += length;
    }
    if ((node = (struct json_node*)malloc(sizeof(struct json_node))) == NULL) {
        /* errno set */
        free(string);
        return NULL;
    }
    node->type = JSON_NUMBER;
    node->value.string = (char*)string;
    DEBUG_NUMBER(node->value.string);
    return node;
}

static void free_number(json_node_t node) {
    if (node && (node->type == JSON_NUMBER)) {
        if (node->value.string)
            free(node->value.string);
        free(node);
    }
}

static void dump_number(json_node_t node, int depth, FILE* fp) {
    int i;
    assert(fp);
    if (node && (node->type == JSON_NUMBER)) {
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        if (node->value.string)
            fprintf(fp, "%s", node->value.string);
        fflush(fp);
    }
}

/*  <literal>    : "true"
 *               | "false"
 *               | "null"
 *               ;
 */
static long scan_literal(JSON json, const char* literal) {
    size_t len = 0;
    assert(json);
    assert(json->buf);
    assert(json->len >= 1);
    assert(json->pos <= json->len);
    assert(literal);

    len = strlen(literal);
    if (!strncmp(&json->buf[json->pos], literal, len))
        return (long)len;
    else
        return (long)0;
}

static json_node_t parse_literal(JSON json, json_type_t type) {
    struct json_node* node = NULL;
    char* string = NULL;
    long length = 0;

    switch (type) {
    case JSON_TRUE:  length = scan_literal(json, "true"); break;
    case JSON_FALSE: length = scan_literal(json, "false"); break;
    case JSON_NULL:  length = scan_literal(json, "null"); break;
    default: errno = EINVAL; return NULL;
    }
    if (length <= 0) {
        errno = EINVAL; /* FIXME: error code */
        return NULL;
    }
    if ((string = (char*)malloc((size_t)((unsigned long)length + 1UL))) == NULL) {
        /* errno set */
        return NULL;
    }
    if ((string = strncpy(string, &json->buf[json->pos], (size_t)length)) != NULL) {
        string[length] = '\0';
        json->pos += length;
        json->col += length;
    }
    if ((node = (struct json_node*)malloc(sizeof(struct json_node))) == NULL) {
        /* errno set */
        free(string);
        return NULL;
    }
    node->type = type;
    node->value.string = (char*)string;
    DEBUG_LITERAL(node->value.string);
    return node;
}

static void free_literal(json_node_t node) {
    if (node && ((node->type == JSON_NULL) ||
                 (node->type == JSON_FALSE) ||
                 (node->type == JSON_TRUE))) {
        if (node->value.string)
            free(node->value.string);
        free(node);
    }
}

static void dump_literal(json_node_t node, int depth, FILE* fp) {
    int i;
    assert(fp);
    if (node && ((node->type == JSON_NULL) ||
                 (node->type == JSON_FALSE) ||
                 (node->type == JSON_TRUE))) {
        for (i = 0; i < depth; i++) {
            fputc(' ', fp); fputc(' ', fp);
        }
        if (node->value.string)
            fprintf(fp, "%s", node->value.string);
        fflush(fp);
    }
}
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de, Homepage: https://www.uv-software.de/
 */
