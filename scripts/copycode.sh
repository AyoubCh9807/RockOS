#!/bin/bash

# Define the output file
OUTPUT_FILE="combined_code.txt"

# Clear the output file if it already exists
> "$OUTPUT_FILE"

# Loop through all .cpp and .hpp files in the current directory
for file in *.cpp *.hpp; do
    # Check if the file actually exists (handles the case where no matches are found)
    [ -e "$file" ] || continue

    echo "==================================================" >> "$OUTPUT_FILE"
    echo "FILE: $file" >> "$OUTPUT_FILE"
    echo "==================================================" >> "$OUTPUT_FILE"
    cat "$file" >> "$OUTPUT_FILE"
    echo -e "\n\n" >> "$OUTPUT_FILE" # Adds spacing between files
done

echo "Done! All .cpp and .hpp files have been combined into $OUTPUT_FILE"
