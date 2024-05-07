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
/** @file        vanilla.h
 *
 *  @brief       Vanilla-JSON - A very simple JSON Parser
 *
 *  @author      $Author: makemake $
 *
 *  @version     $Rev: 814 $
 *
 *  @defgroup    json A very simple JSON Parser
 *  @{
 */
#ifndef VANILLA_JSON_H_INCLUDED
#define VANILLA_JSON_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  options  ------------------------------------------------
 */
/** @name        Compiler Switches
 *  @brief       Options for conditional compilation.
 *  @note        If a define is not defined, it is automatically set to 0.
 *  @{ */
#ifndef OPTION_DISABLED
#define OPTION_DISABLED  0
#endif
/** @note        Set define OPTION_INCLUDE_STDDEF_H to a non-zero value 
 *               (e.g. the build environment) to include header <stddef.h>
 *               and use size_t for the size of objects.
 */
#if (OPTION_INCLUDE_STDDEF_H != OPTION_DISABLED)
#include <stddef.h>                     /* C standard library header */
typedef size_t  jsize_t;                /* to represent the size of an object */
#else
typedef unsigned long  jsize_t;         /* if you don't want to include <stddef.h> */
#endif
/** @} */

/*  -----------  defines  ------------------------------------------------
 */


/*  -----------  types  --------------------------------------------------
 */
/** @brief       JSON value types
 */
typedef enum json_type {                /* JSON value types: */
    JSON_STRING,                        /**< JSON string (zero-terminated) */
    JSON_NUMBER,                        /**< JSON number (long or float) */
    JSON_OBJECT,                        /**< JSON object (dictionary) */
    JSON_ARRAY,                         /**< JSON array of values */
    JSON_TRUE,                          /**< JSON value "true" */
    JSON_FALSE,                         /**< JSON value "false" */
    JSON_NULL,                          /**< JSON value "null" */
    JSON_ERROR = (-1)                   /**< to indicate an error */
} json_type_t;

struct json_member {                    /* dictionary member: */
    char *string;                       /* - key (as zero-terminated string) */
    struct json_node* value;            /* - pointer to a JSON value */
    struct json_member* next;           /* - pointer to next member */
};
struct json_dict {                      /* dictionary (as a linked list): */
    struct json_member* head;           /* - pointer to first member */
    struct json_member* curr;           /* - pointer to current member */
};
struct json_element {                   /* array element: */
    int index;                          /* - index (zero-based) */
    struct json_node* value;            /* - pointer to a JSON value */
    struct json_element* next;          /* - pointer to next element */
};
struct json_array {                     /* array (as a linked list): */
    struct json_element* head;          /* - pointer to first element */
    struct json_element* curr;          /* - pointer to current element */
};
struct json_node {                      /* JSON node: */
    json_type_t type;                   /* - JSON value type */
    union {                             /* - JSON value: */
        char *string;                   /*   - a JSON string or number or literal value */
        struct json_dict dict;          /*   - a JSON key:value dictionary */
        struct json_array array;        /*   - an array of JSON values */
    } value;
};
/** @brief       JSON node
 */
typedef struct json_node *json_node_t;  /* opaque data type! */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
/** @brief       reads a file and build an internal representation of the file's
 *               content (JSON node) if it is a valid JSON file.
 *
 *  @param[in]   filename  - name of the file to be parsed as JSON file
 *
 *  @returns     the JSON root node if successfully read, or NULL on error
 */
extern json_node_t json_read(const char *filename);

/** @brief       frees the memory used by the given JSON node and its childs.
 *
 *  @param[in]   node  - JSON node to be freed
 */
extern void json_free(json_node_t node);

/** @brief       returns the value type of the given JSON node.
 *
 *  @param[in]   node  - JSON node
 *
 *  @returns     the JSON type of the node, or special value JSON_ERROR on error
 */
extern json_type_t json_get_value_type(json_node_t node);

/** @brief       returns the JSON node of the JSON object member specified by
 *               the given string, if the given node is a JSON object.
 *
 *  @param[in]   string  - key of a JSON object member (string)
 *  @param[in]   node    - JSON node of type JSON object
 *
 *  @returns     the JSON node of the member specified by the key, or NULL
 */
extern json_node_t json_get_value_of(const char* string, json_node_t node);

/** @brief       returns the JSON node of the JSON array element specified by
 *               the given index, if the given node is a JSON array.
 *
 *  @param[in]   index  - index of a JSON array element (int)
 *  @param[in]   node   - JSON node of type JSON array
 *
 *  @returns     the JSON node at the specified array index, or NULL
 */
extern json_node_t json_get_value_at(int index, json_node_t node);

/** @brief       returns the JSON node of the first JSON object member,
 *               if the given node is a JSON object, or of the first
 *               JSON array element, if the given node is a JSON array.
 *
 *  @remarks     The first member resp. element of the given JSON node is
 *               marked as the current one, if any.
 * 
 *  @param[in]   node   - JSON node of type JSON object or array
 *
 *  @returns     the JSON node first member resp. element, or NULL
 */
extern json_node_t json_get_value_first(json_node_t node);

/** @brief       returns the JSON node of the next JSON object member,
 *               if the given node is a JSON object, or of the next
 *               JSON array element, if the given node is a JSON array.
 *
 *  @remarks     The next member resp. element of the given JSON node is
 *               marked as the current one, if any.
 *
 *  @param[in]   node   - JSON node of type JSON object or array
 *
 *  @returns     the JSON node next member resp. element, or NULL
 */
extern json_node_t json_get_value_next(json_node_t node);

/** @brief       returns a pointer to the key (string) of the current
 *               JSON object member, if the given node is a JSON object
 *               and if it has a current member.
 *
 *  @param[in]   node   - JSON node of type JSON object
 *
 *  @returns     the key of the current member, or NULL
 */
extern char *json_get_object_string(json_node_t node);

/** @brief       returns the index of the current JSON array element,
 *               if the given node is a JSON array and if it has a
 *               current element.
 *
 *  @param[in]   node   - JSON node of type JSON array
 *
 *  @returns     the index of the current element, or a negative value
 */
extern int json_get_array_index(json_node_t node);

/** @brief       returns a pointer to the content of the given JSON node as
 *               zero-terminated string, if the node is a JSON string.
 *
 *  @remarks     The pointer is invalid if the node or its parent are freed.
 *
 *  @remarks     The buffer for the node value as zero-terminated string is
 *               optional.
 *
 *  @param[in]   node    - JSON node of type JSON string
 *  @param[out]  buffer  - buffer for the node value, or NULL
 *  @param[in]   length  - size of the buffer (in [Byte])
 *
 *  @returns     pointer to the JSON string value, or NULL on error
 */
extern char *json_get_string(json_node_t node, char *buffer, jsize_t length);

/** @brief       returns a pointer to the content of the given JSON node as
 *               zero-terminated string, if the node is a JSON number.
 *
 *  @remarks     The pointer is invalid if the node or its parent are freed.
 *
 *  @remarks     The buffer for the node value as zero-terminated string is
 *               optional.
 *
 *  @param[in]   node    - JSON node of type JSON number
 *  @param[out]  buffer  - buffer for the node value, or NULL
 *  @param[in]   length  - size of the buffer (in [Byte])
 *
 *  @returns     pointer to the JSON number as zero-terminated string, or NULL on error
 */
extern char *json_get_number(json_node_t node, char *buffer, jsize_t length);

/** @brief       returns the content of the given JSON node as integer value,
 *               if the node is a JSON number.
 *
 *  @remarks     The number is converted into long by atol(); see man atol(3).
 *
 *  @remarks     The buffer for the node value as zero-terminated string is
 *               optional.
 *
 *  @param[in]   node    - JSON node of type JSON number
 *  @param[out]  buffer  - buffer for the node value, or NULL
 *  @param[in]   length  - size of the buffer (in [Byte])
 *
 *  @returns     the JSON number converted into an integer value, or 0L on error
 */
extern long json_get_integer(json_node_t node, char *buffer, jsize_t length);

/** @brief       returns the content of the given JSON node as floating point value,
 *               if the node is a JSON number.
 *
 *  @remarks     The number is converted into double by atof(); see man atof(3).
 *
 *  @remarks     The buffer for the node value as zero-terminated string is
 *               optional.
 *
 *  @param[in]   node    - JSON node of type JSON number
 *  @param[out]  buffer  - buffer for the node value, or NULL
 *  @param[in]   length  - size of the buffer (in [Byte])
 *
 *  @returns     the JSON number converted into a floating point value, or 0.0 on error
 */
extern double json_get_float(json_node_t node, char *buffer, jsize_t length);

/** @brief       returns the content of the given JSON node as boolean value,
 *               if the node is JSON "true" or JSON "false".
 *
 *  @remarks     The buffer for the node value as zero-terminated string is
 *               optional.
 *
 *  @param[in]   node    - JSON node of type JSON "true" or "false"
 *  @param[out]  buffer  - buffer for the node value, or NULL
 *  @param[in]   length  - size of the buffer (in [Byte])
 *
 *  @returns     a non-zero value for "true", 0 otherwise and on error
 */
extern int json_get_bool(json_node_t node, char *buffer, jsize_t length);

/** @brief       returns the content of the given JSON node as NULL pointer,
 *               if the node is JSON "null" .
 *
 *  @remarks     The buffer for the node value as zero-terminated string is
 *               optional.
 *
 *  @param[in]   node    - JSON node of type JSON "null"
 *  @param[out]  buffer  - buffer for the node value, or NULL
 *  @param[in]   length  - size of the buffer (in [Byte])
 *
 *  @returns     NULL in every case
 */
extern void* json_get_null(json_node_t node, char *buffer, jsize_t length);

/** @brief       writes the content of the given JSON node and its childs
 *               as JSON format into into a file (or to standard output). 
 *
 *  @param[in]   node      - JSON node to be dumped
 *  @param[in]   filename  - name of the output file, or NULL for 'stdout' 
 */
extern void json_dump(json_node_t node, const char *filename);

/** @name        Deprecated Names
 *  @brief       Deprecated names for compatibility reasons.
 *  @remarks     Deprecated names should not be used anymore!
 *  @{ */
#define json_get_value_string  json_get_object_string
#define json_get_value_index   json_get_array_index
/** @} */

#ifdef __cplusplus
}
#endif
#endif  /* VANILLA_JSON_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de, Homepage: https://www.uv-software.de/
 */
