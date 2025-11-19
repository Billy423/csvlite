#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/*
 * String utility functions for CSVlite.
 * Provides common string manipulation functions used across multiple modules.
 *
 * AUTHOR: Team 21
 * DATE: November 2025
 * VERSION: v0.1.0
 */

/*
 * Duplicates a string by allocating memory for it.
 * The caller is responsible for freeing the returned string with free().
 *
 * parameters:
 * - text: string to duplicate (can be NULL)
 *
 * RETURN: pointer to new string on success, NULL on failure or if text is NULL
 */
char *util_strdup(const char *text);

/*
 * Trims leading and trailing whitespace characters from a string in place.
 * Whitespace includes: spaces, tabs, newlines, and carriage returns.
 * Modifies the input string directly.
 *
 * parameters:
 * - s: string to trim (can be NULL)
 *
 * RETURN: pointer to the trimmed string (same as input), or NULL if s is NULL
 */
char *util_trim(char *s);

/*
 * Trims leading and trailing spaces and tabs only (not newlines/carriage returns).
 * Modifies the input string directly.
 *
 * parameters:
 * - s: string to trim (can be NULL)
 *
 * RETURN: pointer to the trimmed string (same as input), or NULL if s is NULL
 */
char *util_trim_spaces(char *s);

/*
 * Checks if a string contains only numeric characters (digits 0-9).
 * Empty strings and NULL are considered non-numeric.
 *
 * parameters:
 * - text: string to check (can be NULL)
 *
 * RETURN: 1 if text is non-empty and contains only digits, 0 otherwise
 */
int util_is_number(const char *text);

#endif /* UTIL_H */

