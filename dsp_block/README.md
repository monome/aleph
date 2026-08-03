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
| `filter_bp_alpha_tab.*` | semitone → coefficient tables for `filter_bp_blk` |
| `peak_meter_blk.*` | `peak_meter_blk` (abs peak-hold + block-rate decay) |

## Base-width filter coefficients

`filter_bp_blk` takes its alphas from the caller. `filter_bp_alpha_tab`
supplies them from two `fract32` tables indexed by **semitone above a
1 Hz root** (`f = 2^(st/12)` Hz), interpolated on the fix16 fraction —
no divides, no transcendentals, and the log axis is what makes a fixed
"width" hold a constant octave span as "base" sweeps.

Both tables terminate in identity coefficients, so a fully open filter is
transparent (HP to 47 LSB of 2^31, LP exactly) and needs no bypass branch.
Regenerate with `utils/param_scaling/gen_bp_alpha_tab.py`.
