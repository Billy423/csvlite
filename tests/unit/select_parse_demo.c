#include <stdio.h>
#include <stdlib.h>
#include "select.h"

int main(void) {
    const char *spec_ok = "0, 2, 4";
    const char *spec_oob = "0,10";        /* out of bounds when total_cols=5 */
    const char *spec_bad = "0,,2";        /* empty token -> should fail */

    int *idx = NULL;
    int n = 0;

    /* --- happy path --- */
    if (select_parse_indices(spec_ok, NULL, 5, &idx, &n) != 0) {
        printf("[FAIL] parse '%s'\n", spec_ok);
        return 1;
    }
    printf("[OK] '%s' -> count=%d\n", spec_ok, n);
    for (int i = 0; i < n; i++) {
        printf("  idx[%d] = %d\n", i, idx[i]);
    }
    free(idx);

    /* --- out of bounds --- */
    idx = NULL; n = 0;
    if (select_parse_indices(spec_oob, NULL, 5, &idx, &n) == 0) {
        printf("[FAIL] expected out-of-bounds error for '%s'\n", spec_oob);
        free(idx);
        return 1;
    } else {
        printf("[OK] out-of-bounds correctly rejected: '%s'\n", spec_oob);
    }

    /* --- bad token (empty between commas) --- */
    idx = NULL; n = 0;
    if (select_parse_indices(spec_bad, NULL, 5, &idx, &n) == 0) {
        printf("[FAIL] expected failure for '%s'\n", spec_bad);
        free(idx);
        return 1;
    } else {
        printf("[OK] bad token correctly rejected: '%s'\n", spec_bad);
    }

    return 0;
}
