#!/usr/bin/env python3
"""
Cache hierarchy benchmark tool.

Generates instruction sequences with a chosen locality probability (N_PROB)
and runs the CPU simulator across various L1/L2/L3 cache sizes,
producing a CSV with hit percentages.

Usage:
  ./benchmark.py --n-prob 50 --n-inst 10000
  ./benchmark.py --l1 4,8,16,32 --l2 8,16,32,64 --l3 16,32,64,128
"""

import argparse
import csv
import itertools
import os
import random
import subprocess
import sys

CC = os.environ.get("CC", "clang")
CFLAGS = os.environ.get("CFLAGS", "-Wall -Wextra -g")
SOURCES = ["main.c", "CPU.c", "RAM.c", "UCM.c", "cache.c"]


def parse_size_list(s: str):
    return [int(x.strip()) for x in s.split(",") if x.strip()]


def _generate_instructions(n_prob: int, n_inst: int, outpath: str):
    N_FOR = 5
    N_MEM = 1000
    N_OPCODE = 6
    N_WORD = 4
    N_ADDR = 6

    rep = [[0] * 7 for _ in range(N_FOR)]
    for i in range(N_FOR):
        op = random.randrange(N_OPCODE)
        rep[i][0] = op
        for j in range(1, N_ADDR + 1, 2):
            rep[i][j] = random.randrange(N_MEM)
            rep[i][j + 1] = random.randrange(N_WORD)

    inst = [[0] * 7 for _ in range(n_inst)]
    i = 0
    while i < n_inst:
        r = random.randrange(100) + 1
        if r <= n_prob and i + N_FOR < n_inst:
            for j in range(N_FOR):
                for k in range(7):
                    inst[i + j][k] = rep[j][k]
            i += N_FOR
        else:
            op = random.randrange(N_OPCODE)
            inst[i][0] = op
            for j in range(1, N_ADDR + 1, 2):
                inst[i][j] = random.randrange(N_MEM)
                inst[i][j + 1] = random.randrange(N_WORD)
            i += 1

    with open(outpath, "w") as f:
        for row in inst:
            f.write(":".join(str(v) for v in row) + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Cache hierarchy benchmark",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Examples:\n"
            "  %(prog)s                                          # defaults\n"
            "  %(prog)s --n-prob 75 --n-inst 5000                # custom locality\n"
            "  %(prog)s --l1 2,4,8 --l2 4,8,16 --l3 8,16,32     # smaller sweep\n"
            "  N_PROB=90 %(prog)s                                 # via env fallback"
        ),
    )
    parser.add_argument("--n-prob", type=int, default=50,
                        help="Locality probability 0-100 (default: 50)")
    parser.add_argument("--n-inst", type=int, default=10000,
                        help="Number of instructions (default: 10000)")
    parser.add_argument("--l1", default="4,8,16,32",
                        help="Comma-separated L1 sizes (default: 4,8,16,32)")
    parser.add_argument("--l2", default="8,16,32,64",
                        help="Comma-separated L2 sizes (default: 8,16,32,64)")
    parser.add_argument("--l3", default="16,32,64,128",
                        help="Comma-separated L3 sizes (default: 16,32,64,128)")
    parser.add_argument("--output", "-o", default="cache_results.csv",
                        help="Output CSV file (default: cache_results.csv)")
    parser.add_argument("--instructions", default="instructions_bench.txt",
                        help="Generated instruction file (default: instructions_bench.txt)")
    args = parser.parse_args()

    # Allow env-var fallback for N_PROB/N_INST
    if "N_PROB" in os.environ:
        args.n_prob = int(os.environ["N_PROB"])
    if "N_INST" in os.environ:
        args.n_inst = int(os.environ["N_INST"])

    l1_sizes = parse_size_list(args.l1)
    l2_sizes = parse_size_list(args.l2)
    l3_sizes = parse_size_list(args.l3)

    if not l1_sizes or not l2_sizes or not l3_sizes:
        print("Error: each cache size list must have at least one value", file=sys.stderr)
        sys.exit(1)

    project_dir = os.path.dirname(os.path.abspath(__file__)) or "."

    # ---- Step 1: Generate instruction file ----
    inst_path = args.instructions
    if not os.path.isabs(inst_path):
        inst_path = os.path.join(project_dir, inst_path)
    print(f"Generating {args.n_inst} instructions (N_PROB={args.n_prob}): {inst_path}")
    _generate_instructions(args.n_prob, args.n_inst, inst_path)

    # ---- Step 2: Build the simulator ----
    print("Building simulator...")
    subprocess.run(
        [CC]
        + [os.path.join(project_dir, s) for s in SOURCES]
        + CFLAGS.split()
        + ["-o", os.path.join(project_dir, "exe_bench")],
        check=True,
    )

    # ---- Step 3: Run all cache size combinations ----
    total = len(l1_sizes) * len(l2_sizes) * len(l3_sizes)
    print(f"Running {total} simulations...")

    rows = []
    for i, (l1, l2, l3) in enumerate(itertools.product(l1_sizes, l2_sizes, l3_sizes), 1):
        result = subprocess.run(
            [os.path.join(project_dir, "exe_bench"), inst_path, str(l1), str(l2), str(l3)],
            capture_output=True, text=True,
        )
        if result.returncode != 0:
            print(f"  [{i:>{len(str(total))}}/{total}] "
                  f"L1={l1:3d} L2={l2:3d} L3={l3:3d}  →  "
                  f"ERROR (exit {result.returncode})")
            if result.stderr:
                for line in result.stderr.strip().splitlines():
                    print(f"         {line}")
            continue

        stats = None
        for line in result.stdout.splitlines():
            if line.startswith("HIT_STATS:"):
                parts = line.split(":", 1)[1].split(",")
                stats = tuple(map(int, parts))
                break

        if stats is None:
            print(f"  [{i:>{len(str(total))}}/{total}] "
                  f"L1={l1:3d} L2={l2:3d} L3={l3:3d}  →  "
                  f"no HIT_STATS in output")
            continue

        l1_h, l2_h, l3_h, ram, tot = stats

        def pct(v):
            return 100.0 * v / tot if tot else 0.0

        row = {
            "Cache L1 size": l1,
            "Cache L2 size": l2,
            "Cache L3 size": l3,
            "L1 hit %": f"{pct(l1_h):.2f}",
            "L2 hit %": f"{pct(l2_h):.2f}",
            "L3 hit %": f"{pct(l3_h):.2f}",
            "RAM hit %": f"{pct(ram):.2f}",
        }
        rows.append(row)
        print(f"  [{i:>{len(str(total))}}/{total}] "
              f"L1={l1:3d} L2={l2:3d} L3={l3:3d}  →  "
              f"L1={row['L1 hit %']:>6s}%  L2={row['L2 hit %']:>6s}%  "
              f"L3={row['L3 hit %']:>6s}%  RAM={row['RAM hit %']:>6s}%")

    # ---- Step 4: Write CSV ----
    fieldnames = [
        "Cache L1 size", "Cache L2 size", "Cache L3 size",
        "L1 hit %", "L2 hit %", "L3 hit %", "RAM hit %",
    ]
    csv_path = args.output
    if not os.path.isabs(csv_path):
        csv_path = os.path.join(project_dir, csv_path)
    with open(csv_path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

    print(f"\nDone! {len(rows)} results written to {csv_path}")

    # ---- Cleanup ----
    for fn in ["exe_bench"]:
        try:
            os.remove(os.path.join(project_dir, fn))
        except OSError:
            pass


if __name__ == "__main__":
    main()
