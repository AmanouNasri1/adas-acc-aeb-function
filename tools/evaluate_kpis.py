import csv
import math
import sys

def read_csv(path):
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

def main(path, Ts=0.02):
  rows = read_csv(path)
  if not rows:
    print("No data.")
    return 1

  min_d = float("inf")
  min_ttc = float("inf")
  max_jerk = 0.0
  aeb_time = 0.0

  prev_a = None
  for row in rows:
    mode = row["mode"]
    lead_valid = row["lead_valid"]
    d = row["lead_distance_m"]
    ttc = row["ttc_s"]
    a = row["a_cmd_mps2"]

    if lead_valid == 1 and math.isfinite(d):
      min_d = min(min_d, d)
    if math.isfinite(ttc):
      min_ttc = min(min_ttc, ttc)

    max_jerk_comfort = 0.0
  max_jerk_emergency = 0.0

  # inside the loop, replace the jerk logic with:
  if prev_a is not None and mode != 3:
    jerk = abs(a - prev_a) / Ts
    # classify by TTC
    if math.isfinite(ttc) and ttc < 3.0:   # matches cfg.ttc_warn_s currently
      max_jerk_emergency = max(max_jerk_emergency, jerk)
    else:
      max_jerk_comfort = max(max_jerk_comfort, jerk)

      if mode == 3:
        aeb_time += Ts

    prev_a = a

  print(f"min_distance_m: {min_d:.3f}")
  print(f"min_ttc_s:      {min_ttc:.3f}")
  print(f"max_jerk_mps3:  {max_jerk:.3f} (excluding AEB)")
  print(f"aeb_time_s:     {aeb_time:.3f}")
  print(f"max_jerk_comfort_mps3:   {max_jerk_comfort:.3f}")
  print(f"max_jerk_emergency_mps3: {max_jerk_emergency:.3f}")
  return 0

if __name__ == "__main__":
  path = sys.argv[1] if len(sys.argv) > 1 else "results/out.csv"
  Ts = float(sys.argv[2]) if len(sys.argv) > 2 else 0.02
  raise SystemExit(main(path, Ts))
