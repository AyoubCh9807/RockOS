#!/bin/bash

# Define the output file
OUTPUT_FILE="combined_code.txt"

# Clear the output file if it already exists
> "$OUTPUT_FILE"

# Find all .cpp and .hpp files recursively and loop through them
find . -type f \( -name "*.cpp" -o -name "*.hpp" \) | while read -r file; do
    echo "==================================================" >> "$OUTPUT_FILE"
    echo "FILE: $file" >> "$OUTPUT_FILE"
    echo "==================================================" >> "$OUTPUT_FILE"
    cat "$file" >> "$OUTPUT_FILE"
    echo -e "\n\n" >> "$OUTPUT_FILE" # Adds spacing between files
done

echo "Done! All recursive .cpp and .hpp files have been combined into $OUTPUT_FILE"
