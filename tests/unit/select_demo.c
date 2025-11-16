#include <stdio.h>
#include <stdlib.h>
#include "select.h"
#include "row.h"
#include "vec.h"
#include "hmap.h"

static void print_rows(const Vec *rows) {
    int n = (int)vec_length(rows);   // <-- cast once
    for (int i = 0; i < n; i++) {
        const Row *r = vec_get(rows, i);
        int m = row_num_cells(r);
        for (int j = 0; j < m; j++) {
            const char *cell = row_get_cell(r, j);
            printf("%s%s", cell ? cell : "", (j + 1 < m) ? "," : "");
        }
        printf("\n");
    }
}


int main(void) {
    /* ----- make a tiny header map: name -> index ----- */
    HMap *hdr = hmap_new(8);
hmap_put(hdr, "name", (void*)(long)(0 + 1));
hmap_put(hdr, "age",  (void*)(long)(1 + 1));
hmap_put(hdr, "gpa",  (void*)(long)(2 + 1));


    /* ----- make two sample rows: [name,age,gpa] ----- */
    Vec *rows = vec_new(2);

    Row *r1 = row_new(3);
    row_set_cell(r1, 0, "Alice");
    row_set_cell(r1, 1, "20");
    row_set_cell(r1, 2, "3.9");
    vec_push(rows, r1);

    Row *r2 = row_new(3);
    row_set_cell(r2, 0, "Bob");
    row_set_cell(r2, 1, "19");
    row_set_cell(r2, 2, "3.5");
    vec_push(rows, r2);

    /* ----- parse --select "name,gpa" using the header map ----- */
    int *idx = NULL; int nidx = 0;
    if (select_parse_indices("name,gpa", hdr, 3, &idx, &nidx) != 0) {
        fprintf(stderr, "parse failed\n");
        return 1;
    }

    /* ----- project rows to just those columns ----- */
    Vec *proj = select_project_rows(rows, idx, nidx);
    if (!proj) {
        fprintf(stderr, "project failed\n");
        free(idx);
        return 2;
    }

    /* ----- show result ----- */
    print_rows(proj);  // Expected:
                       // Alice,3.9
                       // Bob,3.5

/* ----- free everything ----- */
int nproj = (int)vec_length(proj);
for (int i = 0; i < nproj; i++) row_free(vec_get(proj, i));
vec_free(proj);

int nrows = (int)vec_length(rows);
for (int i = 0; i < nrows; i++) row_free(vec_get(rows, i));
vec_free(rows);


    hmap_free(hdr);
    free(idx);
    return 0;
}
