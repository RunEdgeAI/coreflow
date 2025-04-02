#!/usr/bin/env python3
import argparse
import os
import re
import xml.etree.ElementTree as ET
from xml.dom import minidom

def split_initializer(initializer):
    # Naively split on commas not inside double quotes.
    parts = []
    current = []
    in_quote = False
    i = 0
    while i < len(initializer):
        ch = initializer[i]
        # If not in quotes and we encounter a double slash, skip until the end of line.
        if not in_quote and ch == '/' and i + 1 < len(initializer) and initializer[i+1] == '/':
            # Skip until a newline is encountered.
            while i < len(initializer) and initializer[i] != '\n':
                i += 1
            continue  # Skip the newline in this iteration.
        # Toggle quote state if we encounter an unescaped double quote.
        if ch == '"' and (i == 0 or initializer[i-1] != '\\'):
            in_quote = not in_quote
        # If we encounter a comma while not in quote, finish the current part.
        if ch == ',' and not in_quote:
            part = ''.join(current).strip()
            if part:  # Only add non-empty parts.
                parts.append(part)
            current = []
        else:
            current.append(ch)
        i += 1
    if current:
        parts.append(''.join(current).strip())
    return parts

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

                # Build a dictionary of parameter block definitions.
                param_defs = {}
                for m in re.finditer(
                    r'static\s+(?:const(?:expr)?\s+)?vx_param_description_t\s+([A-Za-z0-9_]+)\s*\[.*?\]\s*=\s*\{(.*?)\}\s*;',
                    content, re.DOTALL | re.IGNORECASE):
                    param_name = m.group(1)
                    param_block = m.group(2)
                    param_defs[param_name] = param_block
                # Find kernel descriptions by capturing the entire initializer block.
                kernel_init_pattern = re.compile(
                    r'vx_kernel_description_t\s+[A-Za-z0-9_]+\s*=\s*\{(.*?)\};',
                    re.DOTALL | re.IGNORECASE)
                for km in kernel_init_pattern.finditer(content):
                    initializer = km.group(1)
                    parts = split_initializer(initializer)
                    # Expecting at least 4 parts:
                    # [0]: kernel id,
                    # [1]: kernel name,
                    # [2]: kernel function,
                    # [3]: parameter array variable,
                    # [4]: parameter count, etc.
                    if len(parts) < 4:
                        continue
                    # Extract kernel name (remove quotes) and the raw fourth parameter.
                    kernel_name = parts[1].strip().strip('"')
                    raw_param_var = parts[3].strip()
                    # Try to extract the parameter variable name from a const_cast expression.
                    m_param = re.search(r'const_cast\s*<\s*vx_param_description_t\s*\*\s*>\s*\(\s*([A-Za-z0-9_:]+)\s*\)', raw_param_var)
                    if m_param:
                        full_name = m_param.group(1)  # e.g. "VxOrtRunner::kernelParams"
                        param_var = full_name.split("::")[-1]  # Extracts "kernelParams"
                    else:
                        # Alternatively, try to match a simple namespaced token.
                        m_param2 = re.search(r'VxOrtRunner::([A-Za-z0-9_]+)', raw_param_var)
                        if m_param2:
                            param_var = m_param2.group(1)
                        else:
                            # Fallback: use the raw string.
                            param_var = raw_param_var

                    inputs = []
                    outputs = []
                    # Look up the parameter block using the extracted param_var
                    if param_var in param_defs:
                        param_block = param_defs[param_var]
                        # Find parameter entries enclosed in braces.
                        params = re.findall(r'\{([^}]+)\}', param_block)
                        for param in params:
                            if "VX_INPUT" in param:
                                m_in = re.search(
                                    r'VX_INPUT\s*,\s*(?:"([^"]+)"|\'([^\']+)\'|([A-Za-z0-9_]+))',
                                    param)
                                if m_in:
                                    value = m_in.group(1) or m_in.group(2) or m_in.group(3)
                                    inputs.append(value)
                            elif "VX_OUTPUT" in param:
                                m_out = re.search(
                                    r'VX_OUTPUT\s*,\s*(?:"([^"]+)"|\'([^\']+)\'|([A-Za-z0-9_]+))',
                                    param)
                                if m_out:
                                    value = m_out.group(1) or m_out.group(2) or m_out.group(3)
                                    outputs.append(value)
                    # Update results using kernel_name as key.
                    if kernel_name:
                        if target_name not in results:
                            results[target_name] = {}
                        results[target_name][kernel_name] = {"inputs": inputs, "outputs": outputs}

    # Build the XML structure: one Target element per first-level subdirectory
    root_el = ET.Element("Workspace")
    for target, kernels in sorted(results.items()):
        target_el = ET.SubElement(root_el, "Target", name=target)
        for kernel, details in sorted(kernels.items()):
            kernel_el = ET.SubElement(target_el, "Kernel", name=kernel)
            # Add inputs element if available
            if details["inputs"]:
                inputs_el = ET.SubElement(kernel_el, "Inputs")
                for inp in details["inputs"]:
                    inp_el = ET.SubElement(inputs_el, "Input")
                    inp_el.text = inp
            # Add outputs element if available
            if details["outputs"]:
                outputs_el = ET.SubElement(kernel_el, "Outputs")
                for out in details["outputs"]:
                    out_el = ET.SubElement(outputs_el, "Output")
                    out_el.text = out

    # Pretty-print the XML and write to file supported.xml
    xml_str = minidom.parseString(ET.tostring(root_el)).toprettyxml(indent="  ")
    with open(os.path.join(output_dir, "supported.xml"), "w") as f:
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
        help="Output directory for the XML file (default: %(default)s)",
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