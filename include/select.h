#ifndef SELECT_H
#define SELECT_H

#include "hmap.h"
#include "vec.h"
#include "row.h"


int select_parse_indices(const char *, const HMap *, int, int **, int *);
Vec *select_project_rows(const Vec *, const int *, int);


#endif