/* Separately implemented verifier for H_q: all q field elements are vertices,
 * and distinct x,y are adjacent iff x+y is primitive. This is producer-side
 * implementation diversity, not an external peer review.
 * Input: "q=Q CYCLE x0 ... x_{q-1}" followed by an optional search summary.
 * Usage: verifysum q p f c0,c1,...,c_{f-1}
 * Field elements use base-p digits; the monic modulus is
 * X^f + c_{f-1}X^{f-1} + ... + c0 (prime fields use modulus argument 0).
 * Checks p prime, q=p^f, order(X)=q-1 for f>1, a permutation of 0..q-1,
 * and primitivity of every adjacent sum including the closing edge.
 * The order test establishes fieldhood before the primitivity tests.
 * Exit 0 requires exactly one valid Hamilton cycle and no invalid cycle.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int p, f, q, n; static int md[16];
static void dig(int a, int *d) { for (int k = 0; k < f; k++) { d[k] = a % p; a /= p; } }
static int und(const int *d) { int r = 0, m = 1; for (int k = 0; k < f; k++) { r += d[k] * m; m *= p; } return r; }
static int fadd(int a, int b) { int x[16], y[16]; dig(a, x); dig(b, y); for (int k = 0; k < f; k++) x[k] = (x[k] + y[k]) % p; return und(x); }
static int fmul(int a, int b) {
    int x[16], y[16], z[32] = {0}; dig(a, x); dig(b, y);
    for (int i = 0; i < f; i++) for (int j = 0; j < f; j++) z[i + j] = (int)(((long long)z[i + j] + (long long)x[i] * y[j]) % p);
    for (int t = 2 * f - 2; t >= f; t--) {           /* reduce X^t = X^{t-f} * X^f, X^f = -sum md[k] X^k */
        int c = z[t]; z[t] = 0; if (!c) continue;
        for (int k = 0; k < f; k++) z[t - f + k] = (int)((((long long)z[t - f + k] - (long long)c * md[k]) % p + p) % p);
    }
    return und(z);
}
static int fpow(int a, long e) { int r = 1; while (e) { if (e & 1) r = fmul(r, a); a = fmul(a, a); e >>= 1; } return r; }

int main(int argc, char **argv) {
    if (argc != 5) { printf("usage: verifysum q p f modulus\n"); return 3; }
    q = atoi(argv[1]); p = atoi(argv[2]); f = atoi(argv[3]); n = q - 1;
    if (p < 2 || f < 1 || f > 16 || q < 3) { printf("bad parameters\n"); return 3; }
    { long long pw = 1; for (int k = 0; k < f; k++) { pw *= p; if (pw > 2000000000LL) { printf("overflow\n"); return 3; } }
      if (pw != q) { printf("q != p^f\n"); return 3; } }
    { char *t = argv[4]; for (int k = 0; k < f; k++) { char *e; long c = strtol(t, &e, 10); if (e == t || c < 0 || c >= p) { printf("bad modulus\n"); return 3; } md[k] = (int)c; t = e; if (*t == ',') t++; } }
    if (f == 1) md[0] = 0;
    for (int d = 2; d * d <= p; d++) if (p % d == 0) { printf("p not prime\n"); return 1; }
    int rs[32], nr = 0; { int m = n; for (int d = 2; (long)d * d <= m; d++) if (m % d == 0) { rs[nr++] = d; while (m % d == 0) m /= d; } if (m > 1) rs[nr++] = m; }
    if (f > 1) {   /* X = element with digit vector (0,1,0,...) must have order exactly n */
        int X = p; if (fpow(X, n) != 1) { printf("modulus check failed\n"); return 1; }
        for (int i = 0; i < nr; i++) if (fpow(X, n / rs[i]) == 1) { printf("modulus not primitive\n"); return 1; }
    }
    char *prim = calloc(q, 1);
    for (int z = 1; z < q; z++) { int ok = 1; for (int i = 0; i < nr && ok; i++) if (fpow(z, n / rs[i]) == 1) ok = 0; prim[z] = ok; }
    int *cyc = malloc(sizeof(int) * q); char *seen = calloc(q, 1);
    char *line = NULL; size_t cap = 0; int found = 0, bad = 0;
    while (getline(&line, &cap, stdin) > 0) {
        int qq; if (sscanf(line, "q=%d CYCLE", &qq) != 1 || !strstr(line, "CYCLE")) continue;
        if (qq != q || found) { bad = 1; continue; }
        char *s = strstr(line, "CYCLE") + 5, *e; int len = 0;
        while (len < q) { long x = strtol(s, &e, 10); if (e == s) break; s = e; if (x < 0 || x >= q || seen[x]) { len = -1; break; } seen[x] = 1; cyc[len++] = (int)x; }
        { char *e2; strtol(s, &e2, 10); if (e2 != s) len = -1; }   /* reject trailing entries */
        if (len != q) { bad = 1; printf("bad permutation\n"); continue; }
        for (int i = 0; i < q; i++) { int z = fadd(cyc[i], cyc[(i + 1) % q]); if (!z || !prim[z]) { bad = 1; printf("bad edge %d %d\n", cyc[i], cyc[(i + 1) % q]); break; } }
        if (!bad) found = 1;
    }
    printf("q=%d %s\n", q, (found && !bad) ? "CYCLE-VERIFIED" : "NOT-VERIFIED");
    return (found && !bad) ? 0 : 2;
}
