# dsp_block

Block-rate DSP helpers for `bfin_lib_block` modules (`MODULE_BLOCKSIZE`
frames per `module_process_block`).

## Conventions

- Object names adapted from [`dsp/`](../dsp/) append **`_blk`** after the
  classic name (`filter_1p_lo` → `filter_1p_lo_blk`) so both can coexist
  on the include path.
- **`*_prepare`**: run once per block; compute / advance block-rate state.
- **`*_next`**: per sample (or read prepared values); use stored constants
  only — no integer divides for filter coeffs, no amp slew steps.
- Pass `blocksize` into init / slew setup; do not include
  `module_custom.h` from this library.

## Slew time constants

Bees integrator params still encode **per-sample** one-pole coefficients
(same tables as frame modules). `filter_1p_lo_blk_set_slew` raises
`c_s → c_s^N` (`N = blocksize`) so one `prepare()` matches about `N`
classic `filter_1p_lo_next` steps. UI “seconds to converge” stays
meaningful. Native block-rate integrator tables are a future bees change
(see [TODO.md](TODO.md)).

## Current objects

| File | Types |
|------|--------|
| `filter_1p_blk.*` | `filter_1p_lo_blk` |
| `filter_bp_blk.*` | `filter_hp_blk`, `filter_lp_blk`, `filter_bp_blk` |
| `peak_meter_blk.*` | `peak_meter_blk` (abs peak-hold + block-rate decay) |
