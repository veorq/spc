#include "spc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main()
{

    uint8_t k[16], p[16], c[16], b[16], t[8];
    int fail = 0;

    memset(k, 1, 16);
    memset(p, 2, 16);
    memset(b, 2, 16);
    memset(t, 4, 8);

    for (int i = 0; i < 8; ++i) {

        encrypt(p, c, k, t);
        decrypt(c, b, k, t);

        if (memcmp(p, b, 16)) {
            printf("test FAIL\n");
            fail = 1;
        }
        memcpy(k, p, 16);
        memcpy(p, c, 16);
        memcpy(b, c, 16);
    }
    if (!fail)
        printf("OK\n");

    return 0;
}
