"""Second, independently written route to the H_q residual set: sympy factorisation, integer-only arithmetic
(all rationals cleared by common denominators), enumeration by prime power construction rather than sieving."""
import sys, itertools
from math import prod, isqrt
from sympy import factorint, primerange
N=int(sys.argv[1])
qs=set()
for p in primerange(2, N+1):
    if p<=61: qs.add(p)
    if p*p>N: continue
    x=p*p
    while x<=N: qs.add(x); x*=p
qs=sorted(q for q in qs if q>=8)
def ok(q):
    n=q-1; ps=sorted(factorint(n)); phi=n
    for r in ps: phi=phi//r*(r-1)
    for k in range(len(ps)+1):
        for K in itertools.combinations(ps,k):
            rest=[r for r in ps if r not in K]; s=len(rest)
            # delta = A/B with B = prod(rest); A = B - sum B/r
            B=prod(rest) if rest else 1; A=B-sum(B//r for r in rest)
            if A<=0: continue
            # Delta = (s-1)B/A + 2 = ((s-1)B + 2A)/A  (or 1 if s=0)
            Dn,Dd=((s-1)*B+2*A, A) if s else (1,1)
            # c = 1/(theta(e) delta) = (prod K)/(prod (r-1) for r in K) * B/A
            cn=prod(K)*B; cd=prod(r-1 for r in K)*A
            W=2**k
            # need phi - c >= 0 and (phi - c)^2 >= 4 W^2 Delta^2 q  ->  (phi*cd - cn)^2 * Dd^2 >= 4 W^2 Dn^2 q cd^2
            L=phi*cd-cn
            if L>=0 and L*L*Dd*Dd >= 4*W*W*Dn*Dn*q*cd*cd: return True
    return False
res=[q for q in qs if not ok(q)]
print("in scope",len(qs),"uncleared",len(res)); open(sys.argv[2],"w").write("\n".join(map(str,res))+"\n")
