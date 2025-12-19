# CSVlite - Command-Line CSV Analytics Tool

**Version: v2.0.1**  

CSVlite is a Linux command-line utility for performing SQL-like operations (select, filter, group-by, sort) on CSV files. Built with C and designed for efficiency.

## Quick Start

```bash
# Build the project
make

# View help
./csvlite --help

# Example - select, filter, group, and sort
./csvlite --file data.csv \
    --select name,age \
    --where 'age>=25' \
    --group-by department \
    --order-by age:desc
```

## Features

### Column Selection
Select specific columns by name or numeric index:
```bash
./csvlite --file data.csv --select name,age
./csvlite --file data.csv --select 0,1  # Using numeric indices
```

### Row Filtering
Filter rows based on conditions:
```bash
./csvlite --file data.csv --where 'age>=25'
./csvlite --file data.csv --where 'salary>50000'
```

### Grouping
Group rows by a column:
```bash
./csvlite --file data.csv --group-by department
./csvlite --file data.csv --group-by 2  # Using numeric index
```

### Sorting
Sort rows in ascending or descending order:
```bash
./csvlite --file data.csv --order-by age
./csvlite --file data.csv --order-by age:desc
./csvlite --file data.csv --order-by salary:asc
```

### Input from stdin
Read CSV data from standard input:
```bash
cat data.csv | ./csvlite -
echo "name,age\nAlice,25" | ./csvlite - --select name
```

## Requirements

- Linux environment
- GCC compiler
- Make
- Standard C libraries

## Building

```bash
make                # Build the application
make test           # Run all tests (unit + integration)
make test-unit      # Run unit tests only
make test-e2e       # Run integration tests only
make coverage       # Generate coverage report
make clean          # Remove build artifacts
```

## Testing

The project includes comprehensive testing:

- **Unit Tests:** Individual module tests in `tests/unit/`
- **Integration Tests:** 34 end-to-end tests in `tests/e2e/`
- **Coverage:** Automated coverage reporting via `make coverage`
- **CI/CD:** Automated testing on every push via GitHub Actions

Run tests:
```bash
make test              # Run all tests
make test-unit         # Unit tests only
make test-e2e          # Integration tests only
make coverage          # Generate coverage reports
```

## Project Structure

```
.
├── include/     # Header files
├── src/         # Source code
├── tests/       # Test files
│   ├── unit/    # Unit tests
│   └── e2e/     # End-to-end tests
├── scripts/     # Utility scripts
├── data/        # Sample CSV files
└── Makefile     # Build configuration
```

## Team Members

| Name | GitHub ID | Role Title |
|------|-----------|------------|
| Billy Wu | [Billy423](https://github.com/Billy423) | Lead Developer |
| Nikhil Ranjith | [nikranjith](https://github.com/nikranjith) | CLI/I/O Owner |
| Nadeem Mohamed | [Nadeem-Mohamed-Mac](https://github.com/Nadeem-Mohamed-Mac) | Query Operations Team |
| Vivek Patel | [VivekP-24](https://github.com/VivekP-24) | Query Operations Team |

## Version History

- **v2.0.0** - Final release: Comprehensive documentation and code quality improvements
- **v1.0.0** - Initial release: Core functionality (select, where, group-by, order-by)
