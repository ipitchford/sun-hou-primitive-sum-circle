# Proof of the Sun-Hou primitive adjacent-sum conjecture
Version 1.1.0-candidate; Anonymous; unrefereed computer-assisted proof candidate.

For every prime power q > 7, all elements of Fq admit a cyclic ordering with every adjacent sum primitive. The large-field and prime-field ranges are inherited; this package closes the finite remainder. See [paper.tex](paper.tex), [claims.md](claims.md), [ASSURANCE.md](ASSURANCE.md) and [AI_INDEX.md](AI_INDEX.md).

## Replay

Requirements: Python 3 with SymPy 1.14.0, a C99-compatible compiler, ordinary local filesystem access. No network or credentials are needed for replay.

```
python3 -m pip install -r requirements.txt
sh code/replay.sh
python3 -O code/build_supplement.py --check
python3 -O code/frozen_check.py
sh code/replay.sh --regenerate
python3 code/criterion_route2.py 18888871 /tmp/sun-hou-residual.txt
cmp data/residual.txt /tmp/sun-hou-residual.txt
```

The frozen route checks 675 exact positive witnesses, 53 explicit moduli and cycle hashes, 172655 edges and five rejection controls. Regeneration is optional and uses a temporary directory; existing certificates are never deleted. The integer route separately enumerates the residual set. Original code/criterion.py is a historical full-sieve route with higher memory use.

See REVISION_RESPONSE.md for review dispositions and LICENSES.md for component rights. The archive manifest, not a stale historical receipt, binds the final bytes.
