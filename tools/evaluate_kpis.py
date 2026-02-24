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

def main(path: str, Ts: float = 0.02, ttc_warn: float = 3.0):
    rows = read_rows(path)
    if not rows:
        print("No data.")
        return 1

    min_d = float("inf")
    min_ttc = float("inf")
    aeb_time = 0.0

    # jerk metrics (exclude AEB mode)
    max_jerk_total = 0.0
    max_jerk_comfort = 0.0
    max_jerk_emergency = 0.0
    jerk_samples = 0

    # sanity: accel range
    min_a = float("inf")
    max_a = float("-inf")

    prev_a = None
    prev_mode = None

    for row in rows:
        mode = row["mode"]
        lead_valid = row["lead_valid"]
        d = row["lead_distance_m"]
        ttc = row["ttc_s"]
        a = row["a_cmd_mps2"]

        min_a = min(min_a, a)
        max_a = max(max_a, a)

        if lead_valid == 1 and math.isfinite(d):
            min_d = min(min_d, d)

        if math.isfinite(ttc):
            min_ttc = min(min_ttc, ttc)

        if mode == 3:
            aeb_time += Ts

        # Jerk: exclude AEB samples (and also exclude jerk computed across AEB boundary)
        if prev_a is not None and mode != 3 and prev_mode != 3:
            jerk = abs(a - prev_a) / Ts
            jerk_samples += 1
            max_jerk_total = max(max_jerk_total, jerk)

            if math.isfinite(ttc) and ttc < ttc_warn:
                max_jerk_emergency = max(max_jerk_emergency, jerk)
            else:
                max_jerk_comfort = max(max_jerk_comfort, jerk)

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

    return 0

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "results/out.csv"
    Ts = float(sys.argv[2]) if len(sys.argv) > 2 else 0.02
    ttc_warn = float(sys.argv[3]) if len(sys.argv) > 3 else 3.0
    raise SystemExit(main(path, Ts, ttc_warn))