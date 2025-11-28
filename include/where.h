/*
* Header file for where.c
* AUTHOR: Nadeem Mohamed
* DATE: November 17, 2025
* VERSION: v1.0.0
*/

#ifndef WHERE_H
#define WHERE_H

#include "vec.h"
#include "row.h"


Vec *where_filter(const Vec *rows, const char *condition);

#endif
