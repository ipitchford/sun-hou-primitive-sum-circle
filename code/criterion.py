"""Exact criterion for H_q (x ~ y iff x != y, x + y primitive): Hamiltonian if for some prime subset K of n = q-1
(e = prod K; sieving primes = rest; delta = 1 - sum 1/p > 0; Delta = (s-1)/delta + 2 or 1 if s = 0),
with M = W(e) Delta sqrt(q), c = 1/(theta(e) delta):   phi(n) - M >= M + c.
Checked exactly: phi - c >= 0 and (phi - c)^2 >= 4 W^2 Delta^2 q.
Scope: proper prime powers 8 <= q <= N and primes 11 <= p <= 61."""
import sys, itertools
from fractions import Fraction as Fr
N=int(sys.argv[1])
spf=list(range(N+1)); i=2
while i*i<=N:
    if spf[i]==i:
        for j in range(i*i,N+1,i):
            if spf[j]==j: spf[j]=i
    i+=1
def primes_of(m):
    ps=[]
    while m>1:
        r=spf[m]; ps.append(r)
        while m%r==0: m//=r
    return ps
def cleared(q):
    n=q-1; ps=primes_of(n); phi=n
    for r in ps: phi=phi//r*(r-1)
    for k in range(len(ps),-1,-1):
        for K in itertools.combinations(ps,k):
            rest=[r for r in ps if r not in K]
            delta=1-sum(Fr(1,r) for r in rest)
            if delta<=0: continue
            Delta=Fr(len(rest)-1)/delta+2 if rest else Fr(1)
            th=Fr(1)
            for r in K: th*=Fr(r-1,r)
            lhs=phi-1/(th*delta)
            if lhs>=0 and lhs*lhs>=4*(2**k)**2*Delta*Delta*q: return True
    return False
scope=[]; out=[]
for q in range(8,N+1):
    p=spf[q]; m=q
    while m%p==0: m//=p
    if m!=1: continue
    if q==p and q>61: continue          # primes > 61: She's construction
    scope.append(q)
    if not cleared(q): out.append(q)
print("in scope",len(scope),"uncleared",len(out),"max",max(out) if out else None)
open(sys.argv[2],"w").write("\n".join(map(str,out))+"\n")
