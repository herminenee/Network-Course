#!/usr/bin/env python3
"""Plot a single-client ns-3 UDP echo log for Lab Part 3.

Usage: python3 plot_part3_rtt.py lab1-part3-10.txt
Requires matplotlib. Writes PNG and PDF beside this script by default.
Log timestamps are rounded, so the calculated RTTs are approximate.
"""
import argparse
from decimal import Decimal
from pathlib import Path
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

EVENT = re.compile(r"At time \+([\d.eE+-]+)s client (sent|received) 1024 bytes")


def read_rtts(path):
    pending = None
    sent_times, values = [], []
    for line in path.read_text().splitlines():
        match = EVENT.search(line)
        if not match:
            continue
        timestamp, event = Decimal(match[1]), match[2]
        if event == "sent":
            if pending is not None:
                raise ValueError("Overlapping sends: packet identifiers are needed")
            pending = timestamp
            sent_times.append(timestamp)
        else:
            if pending is None:
                raise ValueError("Reply without corresponding send")
            rtt = (timestamp - pending) * 1000
            if rtt <= 0:
                raise ValueError("Non-positive RTT")
            values.append(rtt)
            pending = None
    if pending is not None or len(values) != 10:
        raise ValueError("Expected ten complete request/reply pairs")
    if sent_times != [Decimal(i) for i in range(2, 12)]:
        raise ValueError("Expected sends at 2, 3, ..., 11 seconds")
    return values


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("--output-dir", type=Path, default=Path(__file__).resolve().parent)
    args = parser.parse_args()
    values = read_rtts(args.log)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    plt.rcParams.update({"font.size": 11, "pdf.fonttype": 42})
    fig, ax = plt.subplots(figsize=(8.4, 5.4))
    fig.subplots_adjust(left=.11, right=.97, bottom=.19, top=.80)
    fig.text(.11, .94, "Part 3: UDP echo round-trip time", fontsize=17, weight="bold")
    fig.text(.11, .885, "Two Wi-Fi networks | 4 STAs per network | 1024 bytes | seed 1234",
             fontsize=10.5, color="#475569")
    x = list(range(1, 11))
    ax.plot(x, [float(v) for v in values], "o-", color="#245b85", lw=2, markersize=5.5)
    ax.set(xlabel="Packet number", ylabel="Round-trip time (ms)", xlim=(.7, 10.3), ylim=(0, 25))
    ax.set_xticks(x)
    ax.set_yticks(range(0, 26, 5))
    ax.grid(axis="y", color="#d7dfe6", lw=.7)
    ax.spines[["top", "right"]].set_visible(False)
    ax.annotate(f"{values[0]:.2f} ms", (1, float(values[0])), xytext=(9, 7),
                textcoords="offset points", color="#245b85", fontsize=10)
    ax.annotate(f"Packet 3: {values[2]:.2f} ms", (3, float(values[2])), xytext=(18, 34),
                textcoords="offset points", color="#245b85", fontsize=10,
                arrowprops={"arrowstyle": "-", "color": "#245b85", "lw": .8})
    fig.text(.11, .08, "RTT = (client receive time - client send time) x 1000.", fontsize=9, color="#475569")
    fig.text(.11, .045, "Approximate values from rounded logs; packets 2-10 span 8.00-8.96 ms.",
             fontsize=9, color="#475569")
    for extension in ("png", "pdf"):
        path = args.output_dir / f"lab1-part3-rtt.{extension}"
        fig.savefig(path, dpi=300, facecolor="white")
        print(f"Saved {path}")
    print("Packet\tRTT (ms)")
    for i, value in enumerate(values, 1):
        print(f"{i}\t{value:.2f}")
    print(f"Mean for packets 2-10: {sum(values[1:])/9:.3f} ms")


if __name__ == "__main__":
    main()
