"""Verify frozen cycles; --regenerate additionally searches in a temporary directory."""
import argparse, hashlib, json, subprocess, tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def require(ok,message):
    if not ok: raise ValueError(message)
def run(args,**kw):
    return subprocess.run(args,check=True,text=True,capture_output=True,**kw)
def main():
    parser=argparse.ArgumentParser(); parser.add_argument('--regenerate',action='store_true')
    args=parser.parse_args()
    records=json.loads((ROOT/'data/field-manifest.json').read_text())
    require([r['q'] for r in records]==list(map(int,(ROOT/'data/residual.txt').read_text().split())),'scope')
    require(len(records)==53,'53 cycles required')
    with tempfile.TemporaryDirectory(prefix='sun-hou-check-') as temp:
        exe=str(Path(temp)/'verify'); search=str(Path(temp)/'search')
        run(['cc','-O2','-Wall','-Wextra','-o',exe,str(ROOT/'code/verifysum.c')])
        if args.regenerate: run(['cc','-O2','-Wall','-Wextra','-o',search,str(ROOT/'code/hamsum.c')])
        for r in records:
            data=(ROOT/r['cycle']).read_bytes()
            require(hashlib.sha256(data).hexdigest()==r['sha256'],'cycle hash')
            modulus=','.join(map(str,r['modulusLowToHigh'][:-1]))
            out=run([exe,str(r['q']),str(r['p']),str(r['f']),modulus],input=data.decode()).stdout
            require(out.strip()==f"q={r['q']} CYCLE-VERIFIED",'verifier response')
            if args.regenerate:
                regen=run([search,str(r['q']),str(r['p']),str(r['f']),'424242']).stdout.encode()
                require(regen==data,'regeneration mismatch')
        line=(ROOT/'logs/cycles/q11.txt').read_text().splitlines()[0].split()[2:]
        for bad in [line[:-1],[line[0]]+line[:-1],list(map(str,range(11)))]:
            result=subprocess.run([exe,'11','11','1','0'],input='q=11 CYCLE '+' '.join(bad)+'\n',text=True,capture_output=True)
            require(result.returncode!=0,'bad cycle accepted')
        for params in [('9','9','1','0'),('9','3','2','0,0')]:
            result=subprocess.run([exe,*params],input='',text=True,capture_output=True)
            require(result.returncode!=0,'bad field accepted')
    print(json.dumps(dict(cycles=53,edges=sum(r['q'] for r in records),negativeControls=5,
                          regenerated=args.regenerate,status='pass')))
if __name__=='__main__': main()
