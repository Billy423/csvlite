#!/bin/bash
# Generate coverage report for a specific source file
# Usage: generate_coverage.sh <source_file>
# Example: generate_coverage.sh src/hmap.c
#
# Author: Nadeem Mohamed, Vivek Patel
# Date: November 23, 2025
# Version: v2.0.0
#######################################################

source_file="$1"
source_basename=$(basename "$source_file" .c)

# Determine module-specific base name for gcov artifacts.
# Modules follow test_<module>-<module> convention.
# main.c uses csvlite-main because it has no dedicated unit test executable.
if [ "$source_basename" = "main" ]; then
    base_name="csvlite-main"
else
    base_name="test_${source_basename}-${source_basename}"
fi

gcda_file="${base_name}.gcda"

# create coverage reports directory
COVERAGE_DIR="coverage_reports"
mkdir -p "$COVERAGE_DIR"

# check if coverage file exists
if [ -f "${base_name}.gcno" ] && [ -f "$gcda_file" ]; then
    # get coverage percentage from gcov output
    gcov_output=$(gcov "${base_name}.c" 2>&1)
    coverage_line=$(echo "$gcov_output" | grep -i "Lines executed" | head -1) # get the first line of the gcov output
    
    if [ -n "$coverage_line" ]; then
        # move .gcov file to coverage reports directory
        if [ -f "${source_basename}.c.gcov" ]; then
            mv "${source_basename}.c.gcov" "$COVERAGE_DIR/" 2> /dev/null
        fi
        echo "  $source_basename.c: $coverage_line"
    else
            # gcov ran but returned no usable coverage info.
        echo "  $source_basename.c: No coverage data found"
    fi
else
    # gcov data files were not generated for this module.
    echo "  $source_basename.c: No coverage data found"
fi