#ifndef WHERE_H
#define WHERE_H

#include "vec.h"
#include "row.h"


Vec *where_filter(const Vec *rows, const char *condition);

#endif
