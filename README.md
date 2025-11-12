# CSVlite - Command-Line CSV Analytics Tool

**SFWRENG 2XC3**  
**Team 21 - MacSQL**

CSVlite is a Linux command-line utility for performing SQL-like operations (select, filter, group-by, sort) on CSV files. Built with C and designed for efficiency.

## Quick Start

```bash
# Build the project
make

# View help
./csvlite --help

# Example - select, filter and sort
./csvlite --file data.csv \
    --select name,age \
    --where 'age>=18' \
    --order-by age:desc
```

## Requirements

- Linux environment
- GCC compiler
- Make
- Standard C libraries

## Building

```bash
make          # Build the application
make test     # Run tests
make coverage # Generate coverage report
make clean    # Remove build artifacts
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

| Name | Student Number | GitHub ID | Role Title |
|------|---------------|-----------|------------|
| Billy Wu | 400559765 | [Billy423](https://github.com/Billy423) | Lead Developer |
| Nikhil Ranjith | 400559699 | [nikranjith](https://github.com/nikranjith) | CLI/I/O Owner |
| Nadeem Mohamed | 400562265 | [Nadeem-Mohamed-Mac](https://github.com/Nadeem-Mohamed-Mac) | Query Operations Team |
| Vivek Patel | 400562156 | [VivekP-24](https://github.com/VivekP-24) | Query Operations Team |

## Documentation

- **[Development Plan](development_plan.md)** - Complete project planning document (Lab 9.2 deliverable)