# dsp_block TODO

Potential next steps (not required for the initial mx44 migration).

## Performance

- [ ] **`LSETUP` / zero-overhead hardware loops** for frame kernels in
      hot `module_process_block` paths (and shared helpers where useful).
- [ ] **Dual-MAC / multi-issue** hand scheduling (Blackfin 32-bit compute
      parallel with 16-bit loads/stores; `A0`/`A1` accumulators for mix
      sums).
- [ ] **`fract16` / packed SIMD** paths where headroom and noise allow
      higher MAC throughput.
- [ ] Shared **cycle-count instrumentation** helpers for block modules
      (wrap `CYCLES` around prepare vs sample loop stages).

## Control / UI

- [ ] Bees **block-rate integrator tables** so slew params are native
      block poles and `filter_1p_lo_blk_set_slew` need not compute
      `c_s^N` from sample-rate coeffs.

## Adoption

- [ ] Migrate other `modules_block` consumers (spray, parallel, …) onto
      `dsp_block` objects.
- [ ] Optional BPF bypass when wet ≈ 0 and slews synced (trade warm
      state vs CPU) if still needed after alpha caching.
