import argparse

parser = argparse.ArgumentParser(description="Demo of optional arguments")

# Optional argument with default value
parser.add_argument('--verbose', action='store_false', help='Enable verbose output')

# Optional argument that takes a value
parser.add_argument('--output', type=str, help='Output file name', default='out.txt')

args = parser.parse_args()

# Accessing optional arguments
if args.verbose:
    print("Verbose mode is ON")

print("Output will be written to:", args.output)
