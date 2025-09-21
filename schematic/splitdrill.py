#!/usr/bin/env python3

import os
import sys

EPILOG = [
    "G53 G0 Z0 (Retract)\n",
    "M5        (Spindle off.)\n",
    "M2        (Program end.)\n"
]

def split_by_tool(input_file, output_file, tool_number):
    with open(input_file, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()

    header = []
    tool_blocks = {}
    current_tool = None
    current_block = []

    for line in lines:
        if line.startswith("T") and line[1:].strip().isdigit(): # tool change detected
            if current_tool is None:
                header = current_block
            else:
                tool_blocks[current_tool] = tool_blocks.get(current_tool, []) + current_block
            current_tool = line.strip()
            current_block = [line]
        else:
            ll = line.lower()
            if "retract" in  ll or "end" in ll or "off" in ll:
                continue
            current_block.append(line)

    # Capture the last tool block
    if current_tool is not None and current_block:
        tool_blocks[current_tool] = tool_blocks.get(current_tool, []) + current_block

    if not tool_blocks:
        print("No tool changes found in the input file.")
        return
    
    if tool_number not in tool_blocks:
        print(f"Tool number {tool_number} not found in the input file.")
        return

    with open(output_file, "w", encoding="utf-8") as f:
        f.writelines(header)
        for line in tool_blocks[tool_number]:
            if line.startswith("M0") or line.startswith("M5"):
                continue
            f.write(line)
        f.write(f"{tool_number}\n")
        f.writelines(EPILOG)
    
    print(f"Written {output_file} with tool {tool_number} operations.")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input.ngc> <output.ngc> <tool_number>")
        sys.exit(1)

    split_by_tool(sys.argv[1], sys.argv[2], sys.argv[3])
