# AI index: primitive adjacent sums

## Claim and scope

For each prime power q > 7, the graph on all elements of Fq whose distinct vertices are adjacent exactly when their sum is primitive has a Hamilton cycle. Read the exact statements in [paper.tex](paper.tex) and the claim map in [claims.md](claims.md). Do not substitute a graph on only nonzero vertices or a shifted-product adjacency rule.

## Assurance boundary

This is an unrefereed computer-assisted candidate. [ASSURANCE.md](ASSURANCE.md) distinguishes inherited theorems, written arguments, exact finite checks, internal review and unresolved external validation. No proof-assistant verification or four-star assessment is claimed. [prior-art.md](prior-art.md) records source locators and bounded search limitations.

## Replay and evidence

Start with [README.md](README.md). [code/replay.sh](code/replay.sh) verifies frozen artifacts without deleting or regenerating them. [code/build_supplement.py](code/build_supplement.py) checks exact criterion witnesses and the common 728-field scope. [data/criterion-witnesses.json](data/criterion-witnesses.json) contains 675 positive witnesses. [data/residual.txt](data/residual.txt) identifies the 53 exceptions. [data/field-manifest.json](data/field-manifest.json) binds field moduli, representations and cycle hashes. [code/frozen_check.py](code/frozen_check.py) drives a separately implemented C verifier and requires five invalid controls to fail. Optional regeneration compares search output bytes in a temporary directory. Search success alone is not the verifier's claim.

## Dependencies and trust

The large-field and prime-field ranges depend on She arXiv:2609.19114v1, with COT's consecutive primitive theorem. The criterion uses Chvatal–Erdos. Characteristic two has an alternative classical primitive-normal-basis/Gray-code construction. The new deletion corollary is sufficient, not optimal. See [REVISION_RESPONSE.md](REVISION_RESPONSE.md) for review dispositions.

## Identity and reuse

Creator: Anonymous. See [STATUS.md](STATUS.md), [PROVENANCE.md](PROVENANCE.md) and [LICENSES.md](LICENSES.md). Original prose/data are CC0 and original code MIT; upstream rights are separate. Historical receipts do not certify later changed bytes. The final package manifest is the byte-level inventory, not a correctness certificate.
