#!/usr/bin/env python3
"""
Convert PNG to raw RGBA data for deko3d texture loading
"""
from PIL import Image
import sys
import os

def convert_png_to_rgba(input_png, output_raw):
    """Convert PNG to raw RGBA bytes"""
    # Open and convert image to RGBA
    img = Image.open(input_png).convert('RGBA')
    
    # Get image data as raw bytes
    raw_data = img.tobytes()
    
    # Write to output file
    with open(output_raw, 'wb') as f:
        f.write(raw_data)
    
    print(f"✓ Converted {input_png} ({img.size[0]}x{img.size[1]}) to {output_raw}")
    print(f"  Size: {len(raw_data)} bytes ({len(raw_data) / 1024 / 1024:.2f} MB)")
    print(f"  Format: RGBA8 (uncompressed)")
    
    return img.size

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 convert_to_raw.py <input.png> [output.raw]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    if len(sys.argv) >= 3:
        output_file = sys.argv[2]
    else:
        base = os.path.splitext(input_file)[0]
        output_file = f"{base}.raw"
    
    if not os.path.exists(input_file):
        print(f"Error: File '{input_file}' not found")
        sys.exit(1)
    
    try:
        width, height = convert_png_to_rgba(input_file, output_file)
        print(f"\nNext steps:")
        print(f"1. Use format: DkImageFormat_RGBA8_Unorm")
        print(f"2. Dimensions: {width}x{height}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
