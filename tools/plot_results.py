import argparse
import csv
import math
from pathlib import Path

import matplotlib.pyplot as plt


def read_csv(path: Path):
    with path.open(newline="") as f:
        r = csv.DictReader(f)
        rows = list(r)

    # Convert columns when present
    def get(col, cast=float, default=None):
        if not rows or col not in rows[0]:
            return None
        out = []
        for row in rows:
            v = row.get(col, "")
            if v is None or v == "":
                out.append(default)
            else:
                out.append(cast(v))
        return out

    t = get("t_s", float)
    mode = get("mode", int)
    lead_valid = get("lead_valid", int)
    ego_speed = get("ego_speed_mps", float)
    v_set = get("v_set_mps", float)
    lead_dist = get("lead_distance_m", float)
    a_cmd = get("a_cmd_mps2", float)
    ttc = get("ttc_s", float)
    a_cruise = get("a_cruise_mps2", float)
    a_follow = get("a_follow_mps2", float)

    # Optional: relative speed (helps remove TTC spikes when v_rel ~ 0)
    v_rel = get("lead_rel_speed_mps", float)

    # Replace inf/NaN TTC with NaN for plotting
    if ttc is not None:
        ttc = [
            math.nan if (x is None or (isinstance(x, float) and not math.isfinite(x))) else x
            for x in ttc
        ]

    return {
        "t": t,
        "mode": mode,
        "lead_valid": lead_valid,
        "ego_speed": ego_speed,
        "v_set": v_set,
        "lead_dist": lead_dist,
        "a_cmd": a_cmd,
        "ttc": ttc,
        "a_cruise": a_cruise,
        "a_follow": a_follow,
        "v_rel": v_rel,
    }


def save_plot(outpath: Path, title: str, xlabel: str, ylabel: str):
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)
    outpath.parent.mkdir(parents=True, exist_ok=True)
    plt.tight_layout()
    plt.savefig(outpath, dpi=160)
    plt.close()


def plot_speed(data, prefix: str, outdir: Path):
    t = data["t"]
    plt.figure()
    plt.plot(t, data["ego_speed"], label="ego_speed_mps")
    if data["v_set"] is not None:
        plt.plot(t, data["v_set"], label="v_set_mps")
    plt.legend()
    save_plot(outdir / f"{prefix}_speed.png", f"{prefix}: Speed", "time [s]", "speed [m/s]")


def plot_distance(data, prefix: str, outdir: Path):
    if data["lead_dist"] is None:
        return
    t = data["t"]
    plt.figure()
    plt.plot(t, data["lead_dist"], label="lead_distance_m")
    plt.legend()
    save_plot(outdir / f"{prefix}_distance.png", f"{prefix}: Lead Distance", "time [s]", "distance [m]")


def plot_accel(data, prefix: str, outdir: Path):
    t = data["t"]
    plt.figure()
    plt.plot(t, data["a_cmd"], label="a_cmd_mps2")
    if data["a_cruise"] is not None:
        plt.plot(t, data["a_cruise"], label="a_cruise_mps2")
    if data["a_follow"] is not None:
        plt.plot(t, data["a_follow"], label="a_follow_mps2")
    plt.legend()
    save_plot(outdir / f"{prefix}_accel.png", f"{prefix}: Acceleration Command", "time [s]", "accel [m/s^2]")


def _clean_and_clip_ttc(ttc, v_rel=None, v_rel_eps=0.1, ttc_max=30.0):
    """
    - If |v_rel| < v_rel_eps, TTC is not meaningful -> NaN.
    - Clip TTC values above ttc_max to NaN so plot stays readable.
    """
    out = []
    for i, x in enumerate(ttc):
        if x is None or (isinstance(x, float) and (math.isnan(x) or not math.isfinite(x))):
            out.append(math.nan)
            continue

        # Remove spikes when rel speed is too close to zero
        if v_rel is not None and i < len(v_rel) and v_rel[i] is not None and math.isfinite(v_rel[i]):
            if abs(v_rel[i]) < v_rel_eps:
                out.append(math.nan)
                continue

        # Clip for readability
        if x > ttc_max:
            out.append(math.nan)
        else:
            out.append(x)
    return out


def plot_ttc_and_mode(data, prefix: str, outdir: Path, ttc_warn_s=3.0, ttc_aeb_s=1.5, ttc_max=30.0):
    t = data["t"]

    # TTC (clean + clipped + thresholds)
    if data["ttc"] is not None:
        ttc_plot = _clean_and_clip_ttc(data["ttc"], v_rel=data.get("v_rel"), v_rel_eps=0.1, ttc_max=ttc_max)

        plt.figure()
        plt.plot(t, ttc_plot, label="ttc_s (clipped)")
        plt.axhline(ttc_warn_s, linestyle="--", linewidth=1.0, label=f"ttc_warn_s={ttc_warn_s}")
        plt.axhline(ttc_aeb_s, linestyle="--", linewidth=1.0, label=f"ttc_aeb_s={ttc_aeb_s}")
        plt.legend()
        save_plot(outdir / f"{prefix}_ttc.png", f"{prefix}: TTC", "time [s]", "TTC [s]")

    # Mode (step plot)
    if data["mode"] is not None:
        plt.figure()
        plt.step(t, data["mode"], where="post")
        save_plot(
            outdir / f"{prefix}_mode.png",
            f"{prefix}: Mode (0=OFF,1=CRUISE,2=FOLLOW,3=AEB,4=FAULT)",
            "time [s]",
            "mode [-]",
        )


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True, help="Path to CSV log (from sim_runner)")
    ap.add_argument("--outdir", default="docs/plots", help="Output directory for PNGs")
    ap.add_argument("--prefix", default=None, help="Filename prefix for plots (default: csv stem)")

    # TTC plot controls
    ap.add_argument("--ttc-warn", type=float, default=3.0, help="TTC warning threshold line [s]")
    ap.add_argument("--ttc-aeb", type=float, default=1.5, help="TTC AEB threshold line [s]")
    ap.add_argument("--ttc-max", type=float, default=30.0, help="Clip TTC above this value for readability [s]")

    args = ap.parse_args()

    csv_path = Path(args.csv)
    outdir = Path(args.outdir)
    prefix = args.prefix if args.prefix else csv_path.stem

    data = read_csv(csv_path)

    plot_speed(data, prefix, outdir)
    plot_distance(data, prefix, outdir)
    plot_accel(data, prefix, outdir)
    plot_ttc_and_mode(data, prefix, outdir, ttc_warn_s=args.ttc_warn, ttc_aeb_s=args.ttc_aeb, ttc_max=args.ttc_max)

    print(f"Wrote plots to: {outdir.resolve()}")


if __name__ == "__main__":
    main()