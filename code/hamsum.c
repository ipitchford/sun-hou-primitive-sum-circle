/* Hamilton cycle search in the primitive-sum graph H_q (vertices: all of F_q in base-p digit encoding;
 * x ~ y iff x != y and x + y is primitive). Field from exp/log tables (primitive modulus for f > 1).
 * Posa rotation-extension search. usage: hamsum q p f seed [-p]   (-p prints the modulus and exits);
 * otherwise prints "q=Q CYCLE ..." if found and a FOUND/NOT-FOUND summary. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int p, f, q, n;
static int *EXP, *LOG;          /* EXP[i] = g^i (i<n), LOG[x] for x != 0 */
static int poly[16];            /* monic irreducible (primitive) modulus of degree f, coeffs poly[0..f-1] */

static int add(int a, int b);
static int fieldadd(int a, int b) { if (f == 1) { int z = a + b; return z >= p ? z - p : z; } return add(a, b); }
static int add(int a, int b) {  /* digit-wise mod p */
    int r = 0, m = 1;
    for (int k = 0; k < f; k++) { r += ((a % p + b % p) % p) * m; a /= p; b /= p; m *= p; }
    return r;
}
static int mulx(int a) {        /* a * X mod poly */
    int d[16] = {0}, t = a;
    for (int k = 0; k < f; k++) { d[k] = t % p; t /= p; }
    int top = d[f - 1];
    for (int k = f - 1; k > 0; k--) { d[k] = d[k - 1]; }
    d[0] = 0;
    for (int k = 0; k < f; k++) d[k] = ((d[k] - top * poly[k]) % p + p) % p;
    int r = 0, m = 1; for (int k = 0; k < f; k++) { r += d[k] * m; m *= p; } return r;
}
static long gcdl(long a, long b) { while (b) { long t = a % b; a = b; b = t; } return a; }

/* Build exp table by repeated multiplication by a candidate generator; for f>1 we search for a primitive
 * polynomial (X generates), for f=1 we search for a generator g of Z_p^*. Return 1 on success. */
static int build(void) {
    EXP = malloc(sizeof(int) * n); LOG = malloc(sizeof(int) * q);
    if (f == 1) {
        for (int g = 2; g < p || p == 2; g++) {
            if (p == 2) { EXP[0] = 1; LOG[1] = 0; return 1; }
            memset(LOG, -1, sizeof(int) * q);
            long x = 1; int okk = 1;
            for (int i = 0; i < n; i++) { if (LOG[x] != -1) { okk = 0; break; } EXP[i] = x; LOG[x] = i; x = x * g % p; }
            if (okk) return 1;
        }
        return 0;
    }
    /* enumerate monic polys of degree f; X is primitive iff powers of X hit all n nonzero elements */
    long total = 1; for (int k = 0; k < f; k++) total *= p;
    for (long c = 1; c < total; c++) {
        long t = c; for (int k = 0; k < f; k++) { poly[k] = t % p; t /= p; }
        if (poly[0] == 0) continue;
        memset(LOG, -1, sizeof(int) * q);
        int x = 1, okk = 1;
        for (int i = 0; i < n; i++) { if (LOG[x] != -1) { okk = 0; break; } EXP[i] = x; LOG[x] = i; x = mulx(x); }
        if (okk && x == 1) return 1;
    }
    return 0;
}

static unsigned long long rs;
static inline unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

static char *PR; static int *path, *pos; static char *used; static int NV;
static int fieldadd(int a, int b);
static inline int adj(int i, int j) { return i != j && PR[fieldadd(i, j)]; }

/* Posa rotation-extension search for a Hamilton cycle. Returns 1 if found (path[] holds the cycle). */
static int hamcycle(long budget) {
    int len = 0;
    memset(used, 0, NV);
    int *cand = malloc(sizeof(int) * NV);
    int *un = malloc(sizeof(int) * NV), *upos = malloc(sizeof(int) * NV), nun = NV;   /* unvisited list */
    for (int v = 0; v < NV; v++) { un[v] = v; upos[v] = v; }
#define TAKE(v) do { int _i = upos[v], _l = un[--nun]; un[_i] = _l; upos[_l] = _i; } while (0)
    int s = rnd() % NV; path[len++] = s; used[s] = 1; pos[s] = 0; TAKE(s);
    while (budget-- > 0) {
        int end = path[len - 1];
        if (len == NV) {
            if (adj(path[0], end)) { free(cand); free(un); free(upos); return 1; }
            /* rotate to find an endpoint adjacent to path[0] */
            int k = rnd() % (NV - 2) + 1;   /* try a random rotation pivot */
            if (adj(end, path[k - 1])) { /* reverse path[k..len-1] */
                for (int a = k, b = len - 1; a < b; a++, b--) { int t = path[a]; path[a] = path[b]; path[b] = t; pos[path[a]] = a; pos[path[b]] = b; }
            }
            continue;
        }
        /* try to extend: sample unvisited vertices */
        int ext = -1;
        for (int t = 0; t < 64 && nun > 0; t++) { int v = un[rnd() % nun]; if (adj(end, v)) { ext = v; break; } }
        if (ext < 0 && nun > 0) {  /* sampling failed: scan the whole unvisited list */
            int c = 0; for (int i = 0; i < nun; i++) if (adj(end, un[i])) cand[c++] = un[i];
            if (c) ext = cand[rnd() % c];
        }
        if (ext >= 0) { path[len] = ext; pos[ext] = len; used[ext] = 1; len++; TAKE(ext); continue; }
        /* rotation: pick neighbour path[k] of end (k < len-2), reverse path[k+1..len-1] */
        if (len < 3) { free(cand); free(un); free(upos); return 0; }   /* dead end: restart */
        int k = -1;
        for (int t = 0; t < 64; t++) { int kk = rnd() % (len - 1); if (kk < len - 2 && adj(end, path[kk])) { k = kk; break; } }
        if (k < 0) { int c = 0; for (int kk = 0; kk < len - 2; kk++) if (adj(end, path[kk])) cand[c++] = kk; if (!c) { free(cand); free(un); free(upos); return 0; } k = cand[rnd() % c]; }
        for (int a = k + 1, b = len - 1; a < b; a++, b--) { int t = path[a]; path[a] = path[b]; path[b] = t; pos[path[a]] = a; pos[path[b]] = b; }
    }
    free(cand); free(un); free(upos); return 0;
}

int main(int argc, char **argv) {
    q = atoi(argv[1]); p = atoi(argv[2]); f = atoi(argv[3]); n = q - 1; NV = q;
    rs = (argc > 4 ? strtoull(argv[4], 0, 10) : 88172645463325252ULL) | 1;
    if (!build()) { printf("q=%d BUILD-FAIL\n", q); return 1; }
    if (argc > 5 && !strcmp(argv[5], "-p")) { for (int k = 0; k < f; k++) printf("%d,", f == 1 ? 0 : poly[k]); printf("\n"); return 0; }
    char *cop = malloc(n); for (int k = 0; k < n; k++) cop[k] = gcdl(k, n) == 1;
    PR = calloc(q, 1); for (int z = 1; z < q; z++) PR[z] = cop[LOG[z]];
    path = malloc(sizeof(int) * q); pos = malloc(sizeof(int) * q); used = malloc(q);
    int got = 0; for (int attempt = 0; attempt < 200 && !got; attempt++) got = hamcycle(200L * q + 10000);
    if (got) { printf("q=%d CYCLE", q); for (int i = 0; i < q; i++) printf(" %d", path[i]); printf("\n"); }
    printf("q=%d p=%d f=%d %s\n", q, p, f, got ? "FOUND" : "NOT-FOUND");
    return got ? 0 : 2;
}
