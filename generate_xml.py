#!/usr/bin/env python3
import argparse
import os
import re
import xml.etree.ElementTree as ET
from xml.dom import minidom

def generate_xml(base_dir, output_dir):
    # Dictionary to store target : set(kernels)
    results = {}

    # Walk through the targets directory recursively
    for root, dirs, files in os.walk(base_dir):
        # Get the first-level target folder name; skip files at base_dir
        rel_path = os.path.relpath(root, base_dir)
        if rel_path == '.':
            continue
        target_name = rel_path.split(os.sep)[0]

        for filename in files:
            if filename.endswith(('.c', '.cpp')) and filename != 'vx_interface.cpp':
                file_path = os.path.join(root, filename)
                with open(file_path, 'r', errors='ignore') as f:
                    content = f.read()
                # Only proceed if the file contains a kernel definition
                if "_kernel_description_t" in content:
                    # Find all strings starting with kernel definitions (adjust regex as needed)
                    matches = re.findall(r'"((?:[A-Za-z0-9_:-]+\.){2,}[A-Za-z0-9_:-]+)"', content)
                    if matches:
                        results.setdefault(target_name, set()).update(matches)

    # Build the XML structure: one Target element per first-level subdirectory
    root_el = ET.Element("Workspace")
    for target, kernels in sorted(results.items()):
        target_el = ET.SubElement(root_el, "Target", name=target)
        for kernel in sorted(kernels):
            kernel_el = ET.SubElement(target_el, "Kernel")
            kernel_el.text = kernel

    # Pretty-print the XML and write to file supported.xml
    xml_str = minidom.parseString(ET.tostring(root_el)).toprettyxml(indent="  ")
    with open(output_dir + "supported.xml", "w") as f:
        f.write(xml_str)

    print("XML file 'supported.xml' generated successfully.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate an XML file of supported targets and kernels from your workspace."
    )
    parser.add_argument(
        "-d",
        "--base-dir",
        type=str,
        default="/Users/Andrew/Projects/coreVX/targets",
        help="Directory where target folders are located (default: %(default)s)",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        type=str,
        default="./",
        help="Output XML file path (default: %(default)s)",
    )
    args = parser.parse_args()

    # check if output dir argument ends with a slash
    if not args.output_dir.endswith("/"):
        args.output_dir += "/"

    # Check if the base directory exists and is a directory
    if not os.path.exists(args.base_dir):
        print(f"Error: The directory {args.base_dir} does not exist.")
        exit(1)
    if not os.path.exists(args.output_dir):
        print(f"Error: The output path {args.output_dir} is not a directory.")
        exit(1)

    generate_xml(args.base_dir, args.output_dir)