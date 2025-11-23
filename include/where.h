/*
* AUTHOR: Nadeem Mohamed
* DATE: November 17, 2025
* VERSION: v1.0.0
*/

#ifndef WHERE_H
#define WHERE_H

#include "vec.h"
#include "row.h"

/*
 * Filters rows using a single WHERE condition, like "age>=18" or "name==Alice".
 *
 * MEMORY OWNERSHIP:
 * - Returns a new Vec* that the caller must free with vec_free()
 * - Reuses Row* pointers from input (does NOT copy Row objects)
 * - Caller must free Row objects separately (they are shared with input Vec)
 * - Does NOT free the input Vec or Row objects
 *
 * PARAMETERS:
 *  - rows: vector of Row*, row 0 is assumed to be the header row
 *  - condition: where expression from CLI (format: "<column><op><value>")
 *               used operators: ==, !=, >=, <=, >, <
 *
 * RETURNS:
 *  - new Vec* containing header + only matching data rows
 *  - NULL on failure (bad condition, memory issues, etc.)
 */
Vec *where_filter(const Vec *rows, const char *condition);

#endif
