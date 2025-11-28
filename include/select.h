/*
* Header file for select.c
* AUTHOR: Nadeem Mohamed
* DATE: November 10, 2025
* VERSION: v2.0.0
*/

#ifndef SELECT_H
#define SELECT_H

#include "hmap.h"
#include "vec.h"
#include "row.h"


int select_parse_indices(const char *, const HMap *, int, int **, int *);
Vec *select_project_rows(const Vec *, const int *, int);


#endif