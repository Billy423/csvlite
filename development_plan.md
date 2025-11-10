# Development Plan

**Project:** CSVLite - Command-Line CSV Analytics Tool  
**Course:** SFWRENG 2XC3  
**Team:** Team 21 - MacSQL

---

## 1. Title of the Project or App

**CSVLite** - A Linux command-line utility for performing SQL-like operations on CSV files.

---

## 2. High Level Description

### 2.1 Description

CSVLite is a command-line application written in C that processes CSV files. Users interact with it through the Linux terminal by typing commands with flags like `--file`, `--select`, `--where`, `--group-by`, and `--order-by`.

**What it does:**
- Reads CSV files from disk or standard input
- Allows users to select specific columns to display
- Filters rows based on conditions
- Sorts rows by one or more columns
- Groups rows together (basic grouping structure)
- Outputs results in a formatted way

**How users interact:**
Users run the program from the command line with arguments. For example:
```bash
./csvlite --file data.csv \ 
    --select name,age \ 
    --where 'age>=18' \ 
    --order-by age:desc
```

### 2.2 Context of Use

**Expected users:**
- Students and researchers who need to quickly analyze CSV data
- Developers working with data files who want SQL-like operations without a database
- Anyone comfortable with Linux command-line tools

**Environment:**
- Linux operating system (Ubuntu, Debian, etc.)
- Terminal/command-line interface
- Files stored locally or piped from other programs
- Typical use cases: analyzing log files, processing survey data, filtering datasets

Users are expected to have basic familiarity with command-line tools and CSV file format. The tool is designed for quick data analysis tasks that don't require setting up a full database system.

### 2.3 Design constraints

**Performance:**
- Should handle CSV files with thousands of rows efficiently
- Memory usage should be reasonable (avoid loading entire large files into memory at once if possible)
- Fast enough for interactive use (responses within seconds for typical files)

**Memory:**
- Must manage memory carefully to avoid leaks
- Use dynamic data structures efficiently
- Free all allocated memory properly

**User Interface:**
- Command-line only (no GUI)
- Must follow Linux command-line conventions
- Clear error messages when something goes wrong
- `--help` flag must provide complete usage information

**Development Time:**
- Increment 1 (Lab 11.2): Basic functionality (select, where, basic I/O)
- Increment 2 (Lab 12.2): Advanced features (group-by, sorting)
- Limited to course timeline and team availability

**Resources:**
- Team of 4 people
- Standard C libraries and Linux development tools
- GitHub for version control and collaboration
- CI/CD pipeline for automated testing

---

## 3. Team Members

| Name | Student Number | GitHub ID | Role Title |
|------|---------------|-----------|------------|
| Billy Wu | 400559765 | [Billy423](https://github.com/Billy423) | Lead Developer |
| Nikhil Ranjith | 400559699 | [nikranjith](https://github.com/nikranjith) | CLI/I/O Owner |
| Nadeem Mohamed | 400562265 | [Nadeem-Mohamed-Mac](https://github.com/Nadeem-Mohamed-Mac) | Query Operations Team |
| Vivek Patel | 400562156 | [VivekP-24](https://github.com/VivekP-24) | Query Operations Team |

---

## 4. Team Responsibility

### Lead Developer - Billy

**Role:** System architecture, core data structures, and integration

**Module(s) owned:**
- `vec.c/h` - Dynamic array implementation
- `hmap.c/h` - Hash map implementation  
- `main.c` - Application entry point and orchestration
- `Makefile` - Build system
- `where.c/h` - Advanced where clause evaluation (Increment 2 - logical operators)

**Deliverables:**
- Working vector and hash map data structures with unit tests
- Main program that connects all modules together
- Makefile that builds and tests the entire project
- Header files that define interfaces for other teams to use
- Integration testing to ensure everything works together
- Advanced where clause with logical operators (`&&`, `||`) in Increment 2

**Code worked on together:**
- Integration points between modules
- Code reviews for all team contributions
- Final integration and testing

---

### CLI/I/O Owner - Nikhil

**Role:** File I/O, command-line parsing, and input validation

**Module(s) owned:**
- `csv.c/h` - CSV file reading and parsing
- `row.c/h` - Row data structure
- `cli.c/h` - Command-line argument parsing

**Deliverables:**
- Code that reads CSV files from disk or stdin
- Row data structure to store CSV row data
- Command-line argument parsing (`--file`, `--select`, `--where`, `--help`)
- Complete input validation - check all user inputs for errors
- Output formatting to display results
- Unit tests for all I/O modules
- End-to-end tests for basic file operations

**Code worked on together:**
- Integration with Lead's data structures
- Integration with Query team's functions
- Error handling coordination

---

### Query Operations Team Member 1 - Nadeem

**Role:** Query operations implementation (filtering operations) and CI/CD

**Module(s) owned:**
- `select.c/h` - Column selection
- `where.c/h` - Where clause evaluation (Increment 1 - basic single conditions)
- `tests/unit/select_test.c` - Unit tests for select
- `tests/unit/where_test.c` - Unit tests for where
- `.github/workflows/` - CI/CD configuration
- `scripts/` - Utility scripts for coverage and benchmarks

**Deliverables:**
- Select operation that can pick specific columns (by name or index)
- Basic where clause that can filter rows (single conditions like `age>=18`) - Increment 1
- Integration with Lead's `vec` and `hmap` data structures
- Unit tests for select and where modules
- GitHub Actions CI/CD pipeline setup
- Utility scripts for test coverage and benchmarking

**Code worked on together:**
- Integration with CLI/I/O module
- Integration with Lead's DSA functions
- Shared test scripts and CI/CD setup with Member 2

---

### Query Operations Team Member 2 - Vivek

**Role:** Query operations implementation (grouping/ordering) and testing

**Module(s) owned:**
- `group.c/h` - Group-by operations (basic grouping structure - returns one representative row per group)
- `sort.c/h` - Sorting operations
- `tests/unit/group_test.c` - Unit tests for group
- `tests/unit/sort_test.c` - Unit tests for sort
- `tests/e2e/` - End-to-end test scripts
- `scripts/` - Utility scripts for coverage and benchmarks

**Deliverables:**
- Group-by operation that groups rows together using Lead's `hmap`
- Sort operation using C's built-in `qsort()` function (not custom sorting algorithm)
- Integration with Lead's `vec` and `hmap` data structures
- Unit tests for group and sort modules
- End-to-end test scripts

**Code worked on together:**
- Integration with CLI/I/O module
- Integration with Lead's DSA functions
- Shared test scripts and CI/CD setup with Member 1

---

## 5. The Increments

### Increment 1: Core Functionality
**Target Date: Lab 11.2**

**Overall Goal:** Basic CSV processing with select and where operations

#### Lead Developer Contributions:
- `vec.c/h` and `hmap.c/h` - Working data structures with tests
- `main.c` - Minimal orchestration to run the program
- `Makefile` - Build system for the project
- Interface definitions (header files)

#### CLI/I/O Owner Contributions:
- `csv.c/h` - Read CSV files
- `row.c/h` - Store row data
- `cli.c/h` - Parse command-line arguments with full validation
- Basic output formatting
- Unit tests for I/O modules

#### Query Operations Team Member 1 (Nadeem) Contributions:
- `select.c/h` - Column selection (by name and index)
- `where.c/h` - Basic where clause (single conditions like `age>=18`)
- Unit tests for select and where
- GitHub Actions CI/CD pipeline setup
- Utility scripts for coverage and testing (collaborate with Member 2)

#### Query Operations Team Member 2 (Vivek) Contributions:
- `group.c/h` - Basic group-by operation
- `sort.c/h` - Basic sorting using `qsort()`
- Unit tests for group and sort
- End-to-end test scripts
- Utility scripts for coverage and testing (collaborate with Member 1)

**Increment 1 Success Criteria:**
- Can read a CSV file
- Can select specific columns
- Can filter rows with basic conditions
- `--help` works
- All tests pass
- No memory leaks
- Test coverage ≥70%

---

### Increment 2: Advanced Operations
**Target Date: Lab 12.2**

**Overall Goal:** Enhanced features including advanced filtering

#### Lead Developer Contributions:
- Advanced where clause (logical operators: `&&`, `||`) - taking over from Query team
- Optimized DSA implementations
- Enhanced interfaces for Query team
- Optimized build system

#### CLI/I/O Owner Contributions:
- Enhanced CSV parsing (quoted fields, escaped quotes)
- Improved output formatting options
- Better error messages

#### Query Operations Team Member 1 (Nadeem) Contributions:
- Enhanced select (column aliasing, computed columns)
- Hand over where.c/h to Lead Developer for advanced features
- Complete unit tests for select (≥80% coverage)
- Optimized CI/CD pipeline

#### Query Operations Team Member 2 (Vivek) Contributions:
- Enhanced group-by (grouping structure - returns one representative row per group)
- Enhanced sort (multiple columns, ascending/descending)
- Complete unit tests for group and sort (≥80% coverage)
- Comprehensive E2E test scripts
- Performance benchmarks

**Increment 2 Success Criteria:**
- All operations working (select, where, group-by, order-by)
- Quoted CSV fields supported
- Group-by grouping structure working
- Advanced where clauses
- Test coverage ≥80%
- Performance benchmarks completed
