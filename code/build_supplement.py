"""Reconstruct exact witnesses and explicit moduli without altering cycles."""
from fractions import Fraction as F
from itertools import combinations
from math import isqrt, prod
from pathlib import Path
import hashlib, json, subprocess, tempfile, sys
from sympy import factorint, primerange
ROOT=Path(__file__).resolve().parents[1]
N=18888871
qs=set(primerange(8,62))
for p in primerange(2,isqrt(N)+1):
    q=p*p
    while q<=N:
        if q>=8: qs.add(q)
        q*=p
witnesses=[]; residual=[]; counts=[0,0,0]; maxima=[0,0,0]
for q in sorted(qs):
    factors=factorint(q-1); ps=sorted(map(int,factors))
    phi=int(prod((r-1)*r**(a-1) for r,a in factors.items()))
    she=(phi-1)**2*(q-1)**2>=9*q**3*4**len(ps)
    lhs=F(phi)-F(q-1,phi)
    plain=lhs>=0 and lhs**2>=4*4**len(ps)*q
    chosen=None
    for k in range(len(ps)+1):
        for kept in combinations(ps,k):
            rest=[r for r in ps if r not in kept]
            delta=1-sum((F(1,r) for r in rest),F(0))
            if delta<=0: continue
            Delta=F(len(rest)-1)/delta+2 if rest else F(1)
            c=1/(prod(F(r-1,r) for r in kept)*delta)
            lhs=phi-c; slack=lhs**2-4*4**k*Delta**2*q
            if lhs>=0 and slack>=0:
                chosen=dict(q=q,e=prod(kept),keptPrimes=list(kept),phi=phi,
                            delta=str(delta),Delta=str(Delta),c=str(c),squaredSlack=str(slack))
                break
        if chosen: break
    for i,ok in enumerate([she,plain,chosen is not None]):
        counts[i]+=bool(ok)
        if not ok: maxima[i]=q
    if chosen: witnesses.append(chosen)
    else: residual.append(q)
expected=list(map(int,(ROOT/'data/residual.txt').read_text().split()))
if residual!=expected or len(qs)!=728 or counts!=[638,662,675]:
    raise ValueError((len(qs),counts,maxima))
records=[]
with tempfile.TemporaryDirectory(prefix='sun-hou-moduli-') as temp:
    exe=str(Path(temp)/'search')
    subprocess.run(['cc','-O2','-o',exe,str(ROOT/'code/hamsum.c')],check=True)
    for q in residual:
        (p,f),=factorint(q).items(); p,f=int(p),int(f)
        result=subprocess.run([exe,str(q),str(p),str(f),'1','-p'],check=True,capture_output=True,text=True)
        cs=[int(x) for x in result.stdout.strip().strip(',').split(',')]
        path=f'logs/cycles/q{q}.txt'
        records.append(dict(q=q,p=p,f=f,modulusLowToHigh=cs+[1],cycle=path,
                            sha256=hashlib.sha256((ROOT/path).read_bytes()).hexdigest()))
for name,data in [('field-manifest.json',records),('criterion-witnesses.json',witnesses),
                  ('criterion-comparison.json',dict(scope=len(qs),covered=counts,
                   residual=[len(qs)-x for x in counts],largestResidual=maxima,
                   order=['She','unsieved','sieved']))]:
    target=ROOT/'data'/name
    if '--check' in sys.argv:
        if json.loads(target.read_text())!=data:
            raise ValueError(f'Supplement mismatch: {name}')
    else:
        target.write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps(dict(scope=len(qs),covered=counts,residual=len(residual),largestResidual=maxima,moduli=len(records))))
