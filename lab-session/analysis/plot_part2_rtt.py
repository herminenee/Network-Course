#!/usr/bin/env python3
"""Plot RTT from the two single-client ns-3 UDP echo logs.

Usage:
  python3 plot_part2_rtt.py lab1-part2-original-10.txt lab1-part2-modified-10.txt
Requires matplotlib. Outputs PNG and PDF beside this script by default.
RTT values are approximate because the input log timestamps are rounded.
"""
import argparse
from collections import deque
from decimal import Decimal
from pathlib import Path
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

EVENT = re.compile(r"At time \+([\d.eE+-]+)s client (sent|received) 1024 bytes")


def read_rtts(path):
    pending = deque()
    values = []
    sent_times = []
    for line in path.read_text().splitlines():
        match = EVENT.search(line)
        if not match:
            continue
        timestamp, event = Decimal(match[1]), match[2]
        if event == "sent":
            # These lab runs have one client and one outstanding packet.
            if pending:
                raise ValueError(f"{path}: overlapping sends; packet IDs needed")
            pending.append(timestamp)
            sent_times.append(timestamp)
        else:
            if not pending:
                raise ValueError(f"{path}: reply has no corresponding send")
            rtt = (timestamp - pending.popleft()) * 1000
            if rtt <= 0:
                raise ValueError(f"{path}: non-positive RTT")
            values.append(rtt)
    if pending or len(values) != 10:
        raise ValueError(f"{path}: expected 10 complete request/reply pairs")
    if sent_times != [Decimal(i) for i in range(2, 12)]:
        raise ValueError(f"{path}: expected sends at 2, 3, ..., 11 seconds")
    return values


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("original", type=Path)
    parser.add_argument("modified", type=Path)
    parser.add_argument("--output-dir", type=Path, default=Path(__file__).resolve().parent)
    args = parser.parse_args()
    original, modified = read_rtts(args.original), read_rtts(args.modified)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    plt.rcParams.update({"font.size": 11, "pdf.fonttype": 42})
    fig, ax = plt.subplots(figsize=(8.4, 5.4))
    fig.subplots_adjust(left=.11, right=.97, bottom=.19, top=.80)
    fig.text(.11, .94, "Part 2: UDP echo round-trip time", fontsize=17, weight="bold")
    fig.text(.11, .885, "10 packets | 5 CSMA nodes (nCsma = 4) | 1024 bytes | seed 1234",
             fontsize=10.5, color="#475569")
    x = range(1, 11)
    ax.plot(x, [float(v) for v in original], "o-", color="#245b85", lw=2,
            markersize=5.5, label="Original: server on CSMA LAN")
    ax.plot(x, [float(v) for v in modified], "s-", color="#b83239", lw=2,
            markersize=5.5, label="Modified: server beyond added P2P link")
    ax.set(xlabel="Packet number", ylabel="Round-trip time (ms)", xlim=(.7, 10.3), ylim=(0, 25))
    ax.set_xticks(list(x))
    ax.set_yticks(range(0, 26, 5))
    ax.grid(axis="y", color="#d7dfe6", lw=.7)
    ax.spines[["top", "right"]].set_visible(False)
    ax.legend(loc="upper right", frameon=False, fontsize=9.5)
    ax.annotate(f"{original[0]:.2f} ms", (1, float(original[0])), xytext=(8, 7),
                textcoords="offset points", color="#245b85", fontsize=10)
    ax.annotate(f"{modified[0]:.2f} ms", (1, float(modified[0])), xytext=(8, 7),
                textcoords="offset points", color="#b83239", fontsize=10)
    fig.text(.11, .08, "RTT = (client receive time - client send time) x 1000.", fontsize=9, color="#475569")
    fig.text(.11, .045, "Values use rounded log timestamps; tiny late-packet changes reflect log precision.",
             fontsize=9, color="#475569")
    for extension in ("png", "pdf"):
        path = args.output_dir / f"lab1-part2-rtt-comparison.{extension}"
        fig.savefig(path, dpi=300, facecolor="white")
        print(f"Saved {path}")
    print("Packet\tOriginal RTT (ms)\tModified RTT (ms)\tIncrease (ms)")
    for i, (a, b) in enumerate(zip(original, modified), 1):
        print(f"{i}\t{a:.2f}\t{b:.2f}\t{b-a:.2f}")
    print(f"Mean increase for packets 2-10: {sum(b-a for a,b in zip(original[1:],modified[1:]))/9:.3f} ms")


if __name__ == "__main__":
    main()
