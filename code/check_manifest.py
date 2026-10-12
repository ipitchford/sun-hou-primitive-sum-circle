"""Fail closed on missing, changed or unexpected files in a fresh payload."""
import hashlib, json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
rows=json.loads((root/'MANIFEST.json').read_text())['files']
expected={x['path'] for x in rows}|{'MANIFEST.json'}
actual={p.relative_to(root).as_posix() for p in root.rglob('*') if p.is_file() and '.git' not in p.parts}
if actual!=expected: raise ValueError(('Inventory mismatch',sorted(actual^expected)))
for row in rows:
    p=root/row['path']
    if p.stat().st_size!=row['size_bytes'] or hashlib.sha256(p.read_bytes()).hexdigest()!=row['sha256']:
        raise ValueError('Digest mismatch: '+row['path'])
print(json.dumps({'status':'pass','files':len(rows)}))
