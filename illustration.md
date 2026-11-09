# libmetal Bus Fix — Host Test Report

**Subject**: 03-bus-lookup.patch — fixes `metal_bus_find(name, NULL)` not returning 0,
which allowed duplicate bus registration.

**Environment**: Windows 11 / PowerShell, Python 3.11.7, MSYS2 clang 23.1.5,
libmetal `ddb1586` (branch `bus-fix`), verify_libmetal_candidates.py (bus, 5 cases, UBSan).

**Results**:

| Code | Result |
|------|--------|
| Original | 3/5 (`exists-without-result` → -2, `duplicate` → 0; both failures as expected, exit 1) |
| Patched | 5/5 passed, exit 0 |

**Conclusion**: Patch works as intended — `metal_bus_find(name, NULL)` returns 0,
duplicate registration returns `-EEXIST`. Matches README expectation (3/5 → 5/5).

**Notes**: Windows run needs the `..\Internship\.hostcc` wrapper (COFF ld cannot gc
dead-code sections; stub satisfies their undefined refs). Script and libmetal sources
unmodified. `corrupt .drectve` warnings are harmless. On-target (CH32H417) testing
not done yet.

---

# libmetal IRQ Allocation Fix — Host Test Report

**Subject**: commit `7023255` in `libmetal-1` (`fix(irq): Fix the issue caused by
range overlap during IRQ allocation`) — restart the controller scan after moving an
automatic IRQ range past a conflict. Logic identical to
`libmetal-irq-fix/01-irq-range.patch`; file content matches the pack's `lib/irq.c`
(differs only by a missing trailing newline).

**Environment**: same as above; libmetal-1 at `7023255` (`ddb1586` + bus fix +
irq fix); verify_libmetal_candidates.py from `libmetal-irq-fix` (irq, 6 cases, UBSan).

**Results**:

| Code | Result |
|------|--------|
| Fixed (7023255) | **6/6** passed, exit 0 (`unsorted`/`sorted` allocate [30,45); `adjacent` allocates [10,20); `fixed-overlap`, `idempotent`, `zero-count` OK) |
| Original baseline | not re-run locally; pack README expects 5/6 (`unsorted` allocates overlapping [10,25)) |

**Conclusion**: the fix is correct — automatic allocation never overlaps an
already-registered controller.

**Notes**: irq mode additionally required `__ubsan_handle_*_abort` stubs in
win_host_stubs.c: MSYS2 ships no ubsan runtime and GNU ld drops the drectve
auto-link entry. Stubs abort on violation, preserving fatal UBSan semantics.
