#!/usr/bin/env python3

import re
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Patch


def parse_file(filename):
    """
    Parses getpinfo() output files, extracting PID, priority (queue), 
    start tick, and duration for each scheduling event.
    """
    records = []
    current_pid = None
    
    lines = Path(filename).read_text(errors="replace").splitlines()
    for line in lines:
        text = line.lower()
        
        # Skip standard print output lines from tests to avoid false numbers
        if "io output" in text or "result:" in text or "hi" in text:
            continue
            
        # Detect Process ID header line, e.g., "name= ..., pid= 5"
        pid_match = re.search(r"pid\s*[:=]\s*(\d+)", text)
        if pid_match:
            current_pid = int(pid_match.group(1))
            continue
            
        # Detect scheduling stat line, e.g., "start = 1, duration = 1, priority = 0"
        start_match = re.search(r"start\s*[:=]\s*(-?\d+)", text)
        dur_match = re.search(r"(?:duration|ticks|tick)\s*[:=]\s*(-?\d+)", text)
        prio_match = re.search(r"(?:priority|queue|q)\s*[:=]\s*(-?\d+)", text)
        
        if start_match and dur_match and prio_match and current_pid is not None:
            start_tick = int(start_match.group(1))
            duration = int(dur_match.group(1))
            priority = int(prio_match.group(1))
            records.append((current_pid, priority, start_tick, duration))
            
    if not records:
        raise ValueError("No valid getpinfo scheduling records found in {}".format(filename))
    return records


def draw(records, test_number):
    """
    Draws the MLFQ timeline graph with Priority Queues (Q0, Q1, Q2) on the Y-axis
    and Ticks on the X-axis, color-coded by Process (PID).
    """
    # Group events by priority queue -> list of (start, duration, pid)
    queues_data = {0: [], 1: [], 2: []}
    pids = sorted(list(set(pid for pid, _, _, _ in records)))
    
    # Distinct colors for different processes
    base_colors = ['#2ca02c', '#1f77b4', '#ff7f0e', '#d62728', '#9467bd', '#8c564b']
    pid_colors = {pid: base_colors[i % len(base_colors)] for i, pid in enumerate(pids)}

    for pid, priority, start, duration in records:
        # Clamp priority between 0 and 2 just in case
        prio = max(0, min(2, priority))
        # Ensure duration is at least 0.5 for visibility if 0 ticks
        dur = max(duration, 0.5)
        queues_data[prio].append((start, dur, pid))

    figure, axis = plt.subplots(figsize=(11, 5))

    # Y-axis represents queues: Q0 (top), Q1 (middle), Q2 (bottom)
    y_positions = {0: 2, 1: 1, 2: 0}
    
    for queue_level, events in queues_data.items():
        y_pos = y_positions[queue_level]
        for start, duration, pid in events:
            axis.broken_barh(
                [(start, duration)], 
                (y_pos - 0.3, 0.6), 
                facecolors=pid_colors[pid], 
                edgecolors='none'
            )

    axis.set_yticks([0, 1, 2])
    axis.set_yticklabels(['Q2 (Lowest)', 'Q1 (Medium)', 'Q0 (Highest)'])
    axis.set_ylabel("Priority Queues")
    axis.set_xlabel("Timer Ticks")
    axis.set_title("MLFQ Scheduler Timeline - Test {}".format(test_number))
    axis.grid(True, linestyle="--", alpha=0.5)

    # Legend for processes (PIDs)
    legend_handles = [
        Patch(color=pid_colors[pid], label="PID {}".format(pid)) for pid in pids
    ]
    axis.legend(handles=legend_handles, loc="upper right")

    figure.tight_layout()
    figure.savefig("graph{}.pdf".format(test_number))
    plt.close(figure)


def main():
    filenames = sys.argv[1:4]
    if not filenames:
        filenames = ["test{}.out".format(i) for i in range(1, 4)]
    if len(filenames) != 3:
        raise SystemExit("usage: python3 graph.py test1.out test2.out test3.out")

    summaries = []
    for number, filename in enumerate(filenames, 1):
        records = parse_file(filename)
        draw(records, number)
        summaries.append((filename, records))


if __name__ == "__main__":
    main()