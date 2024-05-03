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
 *  @author      $Author$
 *
 *  @version     $Rev$
 *
 *  @defgroup    json A very simple JSON Parser
 *  @{
 */
#ifndef VANILLA_JSON_H_INCLUDED
#define VANILLA_JSON_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  includes  -----------------------------------------------
 */


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
#ifndef JSON_LINE_LENGTH
#define JSON_LINE_LENGTH  1024
#endif

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

struct json_member {                    /* JSON dictionary member: */
    char *string;                       /* - key (as zero-terminated string) */
    struct json_node* value;            /* - pointer to a JSON value */
    struct json_member* next;           /* - pointer to next member */
};
struct json_element {                   /* JSON array element: */
    int index;                          /* - index (zero-based) */
    struct json_node* value;            /* - pointer to a JSON value */
    struct json_element* next;          /* - pointer to next member */
};
struct json_node {                      /* JSON value node: */
    json_type_t type;                   /* - JSON value type */
    union {                             /* - JSON value: */
        char *string;                   /*   - a JSON string or number or literal value */
        struct json_member* dict;       /*   - a JSON key:value dictionary */
        struct json_element* array;     /*   - an array of JSON values */
    } value;
};
/** @brief       JSON value node
 */
typedef struct json_node *json_node_t;  /* opaque data type! */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
/** @brief       tbd.
 *
 *  @param[in]   filename - ...
 *
 *  @returns     ... or NULL on error
 */
extern json_node_t json_read(const char *filename);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 */
extern void json_free(json_node_t node);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern json_type_t json_get_value_type(json_node_t node);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern json_node_t json_get_value_of(const char* string, json_node_t node);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern json_node_t json_get_value_at(int index, json_node_t node);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern char *json_get_string(json_node_t node, char *buffer, unsigned long length);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern char *json_get_number(json_node_t node, char *buffer, unsigned long length);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern long json_get_integer(json_node_t node, char *buffer, unsigned long length);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern double json_get_float(json_node_t node, char *buffer, unsigned long length);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern int json_get_bool(json_node_t node, char *buffer, unsigned long length);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern void* json_get_null(json_node_t node, char *buffer, unsigned long length);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern json_node_t json_get_value_first(json_node_t node);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern json_node_t json_get_value_next(json_node_t node);

/** @brief       tbd.
 *
 *  @param[in]   node - ...
 *
 *  @returns     ...
 */
extern int json_dump(json_node_t node, const char *filename);

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
