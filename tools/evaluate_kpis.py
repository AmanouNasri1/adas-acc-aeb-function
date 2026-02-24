import csv
import math
import sys

def read_rows(path: str):
    rows = []
    with open(path, newline="") as f:
        r = csv.DictReader(f)
        for row in r:
            out = {}
            for k, v in row.items():
                if k in ("mode", "lead_valid"):
                    out[k] = int(float(v))
                else:
                    out[k] = float(v)
            rows.append(out)
    return rows

def mean(vals):
    vals = [v for v in vals if v is not None and math.isfinite(v)]
    return sum(vals) / len(vals) if vals else float("nan")

def main(path: str, Ts: float = 0.02, ttc_warn: float = 3.0, T_gap: float = 1.5, d0: float = 3.0):
    rows = read_rows(path)
    if not rows:
        print("No data.")
        return 1

    min_d = float("inf")
    min_ttc = float("inf")
    aeb_time = 0.0

    max_jerk_total = 0.0
    max_jerk_comfort = 0.0
    max_jerk_emergency = 0.0
    jerk_samples = 0

    min_a = float("inf")
    max_a = float("-inf")

    prev_a = None
    prev_mode = None

    # For steady-state metrics
    t_end = rows[-1]["t_s"]
    cruise_speed_err = []
    follow_tgap_err = []

    for row in rows:
        t = row["t_s"]
        mode = row["mode"]
        lead_valid = row["lead_valid"]
        d = row["lead_distance_m"]
        ttc = row["ttc_s"]
        a = row["a_cmd_mps2"]
        v = row["ego_speed_mps"]
        v_set = row.get("v_set_mps", float("nan"))

        min_a = min(min_a, a)
        max_a = max(max_a, a)

        if lead_valid == 1 and math.isfinite(d):
            min_d = min(min_d, d)
        if math.isfinite(ttc):
            min_ttc = min(min_ttc, ttc)
        if mode == 3:
            aeb_time += Ts

        # Jerk (exclude AEB and boundary)
        if prev_a is not None and mode != 3 and prev_mode != 3:
            jerk = abs(a - prev_a) / Ts
            jerk_samples += 1
            max_jerk_total = max(max_jerk_total, jerk)
            if math.isfinite(ttc) and ttc < ttc_warn:
                max_jerk_emergency = max(max_jerk_emergency, jerk)
            else:
                max_jerk_comfort = max(max_jerk_comfort, jerk)

        # Steady-state speed error for CRUISE (last 2 seconds)
        if mode == 1 and t >= (t_end - 2.0) and math.isfinite(v_set):
            cruise_speed_err.append(abs(v_set - v))

        # Steady-state time-gap error for FOLLOW (last 5 seconds)
        # actual time gap approx: (d - d0) / v  (if v>0)
        if mode == 2 and lead_valid == 1 and t >= (t_end - 5.0) and v > 0.5 and math.isfinite(d):
            t_gap_actual = (d - d0) / v
            follow_tgap_err.append(abs(t_gap_actual - T_gap))

        prev_a = a
        prev_mode = mode

    print(f"min_distance_m:          {min_d:.3f}")
    print(f"min_ttc_s:               {min_ttc:.3f}")
    print(f"aeb_time_s:              {aeb_time:.3f}")
    print(f"a_cmd_range_mps2:         [{min_a:.3f}, {max_a:.3f}]")
    print(f"jerk_samples_excl_aeb:    {jerk_samples}")
    print(f"max_jerk_total_mps3:      {max_jerk_total:.3f} (excluding AEB)")
    print(f"max_jerk_comfort_mps3:    {max_jerk_comfort:.3f} (ttc >= {ttc_warn})")
    print(f"max_jerk_emergency_mps3:  {max_jerk_emergency:.3f} (ttc < {ttc_warn})")
    print(f"cruise_ss_speed_err_mps:  {mean(cruise_speed_err):.3f} (mean |v_set-v| last 2s in CRUISE)")
    print(f"follow_ss_tgap_err_s:     {mean(follow_tgap_err):.3f} (mean |tgap-T| last 5s in FOLLOW)")

    return 0

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "results/out.csv"
    Ts = float(sys.argv[2]) if len(sys.argv) > 2 else 0.02
    ttc_warn = float(sys.argv[3]) if len(sys.argv) > 3 else 3.0
    T_gap = float(sys.argv[4]) if len(sys.argv) > 4 else 1.5
    d0 = float(sys.argv[5]) if len(sys.argv) > 5 else 3.0
    raise SystemExit(main(path, Ts, ttc_warn, T_gap, d0))