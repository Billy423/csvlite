# Development Plan

**Project:** CSVLite - Command-Line CSV Analytics Tool  
**Course:** SFWRENG 2XC3  
**Team:** Team 21 - MacSQL
**Last Revised:** December 2025

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
- Increment 1 (Lab 11.2): Core functionality (select, where, group-by, order-by, basic I/O) ✅ **COMPLETED**
- Increment 2 (Lab 12.2): Documentation, code quality, and final polish ✅ **COMPLETED**
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
- `row.c/h` - Row data structure


**Deliverables:**
- Working vector and hash map data structures with unit tests
- Main program that connects all modules together
- Makefile that builds and tests the entire project
- Header files that define interfaces for other teams to use
- Integration testing to ensure everything works together
- Comprehensive documentation for all owned modules (Increment 2)

**Code worked on together:**
- Integration points between modules
- Code reviews for all team contributions
- Final integration and testing

---

### CLI/I/O Owner - Nikhil

**Role:** File I/O, command-line parsing, and input validation

**Module(s) owned:**
- `csv.c/h` - CSV file reading and parsing
- `cli.c/h` - Command-line argument parsing
- `where.c/h` - Coloumn select (collaborating with Query team)
- `main.c` - Application entry point and orchestration (collaborating with Lead)


**Deliverables:**
- Code that reads CSV files from disk or stdin
- Row data structure to store CSV row data
- Command-line argument parsing (`--file`, `--select`, `--where`, `--help`)
- Complete input validation - check all user inputs for errors
- Output formatting to display results
- Unit tests for all I/O modules
- End-to-end tests for basic file operations
- Comprehensive documentation for all owned modules (Increment 2)

**Code worked on together:**
- Integration with Lead's data structures
- Integration with Query team's functions
- Error handling coordination
- Integrating main application orchestration with Lead

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
- Comprehensive documentation for all owned modules (Increment 2)

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
- Comprehensive documentation for all owned modules (Increment 2)

**Code worked on together:**
- Integration with CLI/I/O module
- Integration with Lead's DSA functions
- Shared test scripts and CI/CD setup with Member 1

---

## 5. The Increments

### Increment 1: Core Functionality
**Target Date: Lab 11.2**  
**Status: COMPLETED**

**Overall Goal:** Basic CSV processing with select and where operations

#### Lead Developer Contributions:
- `vec.c/h` and `hmap.c/h` - Working data structures with tests ✅
- `main.c` - Full orchestration integrating all modules ✅
- `Makefile` - Complete build system with test and coverage targets ✅
- Interface definitions (header files) ✅
- `row.c/h` - Row data structure implementation ✅

#### CLI/I/O Owner Contributions:
- `csv.c/h` - Read CSV files from disk and stdin ✅
- `row.c/h` - Store row data (collaborated with Lead) ✅
- `cli.c/h` - Parse command-line arguments with full validation ✅
- Complete output formatting ✅
- Unit tests for I/O modules ✅

#### Query Operations Team Member 1 (Nadeem) Contributions:
- `select.c/h` - Column selection (by name and numeric index) ✅
- `where.c/h` - Basic where clause (single conditions like `age>=18`) ✅
- Unit tests for select and where ✅
- GitHub Actions CI/CD pipeline setup ✅
- Utility scripts for coverage and testing ✅

#### Query Operations Team Member 2 (Vivek) Contributions:
- `group.c/h` - Basic group-by operation ✅
- `sort.c/h` - Basic sorting using `qsort()` with ascending/descending support ✅
- Unit tests for group and sort ✅
- End-to-end test scripts (34 integration tests) ✅
- Utility scripts for coverage and testing ✅

**Increment 1 Success Criteria:**
- ✅ Can read a CSV file (from disk and stdin)
- ✅ Can select specific columns (by name and numeric index)
- ✅ Can filter rows with basic conditions
- ✅ Can group rows by column
- ✅ Can sort rows by column (ascending/descending)
- ✅ `--help` works and provides complete usage information
- ✅ All tests pass (unit and integration)
- ✅ No memory leaks detected
- ✅ Test coverage >85% for main.c, >70% overall

---

### Increment 1 Revisions (Based on Experience)

**REVISION 1: Scope Expansion**
- **Change:** GROUP BY and ORDER BY completed in Increment 1 (originally planned for Increment 2)
- **Reason:** Team members completed modules ahead of schedule
- **Impact:** More feature-complete foundation for Increment 2

**REVISION 2: Test Coverage Goals**
- **Change:** Achieved >85% coverage for main.c (exceeded ≥70% target)
- **Reason:** Prioritized comprehensive integration testing
- **Impact:** Higher code quality confidence

**REVISION 3: ORDER BY Direction Syntax**
- **Change:** Added explicit direction syntax (`--order-by age:asc`/`:desc`)
- **Reason:** Better user experience and SQL-like behavior
- **Impact:** More intuitive sorting capabilities

**REVISION 4: Integration Testing**
- **Change:** Expanded to 34 comprehensive tests covering error paths and edge cases
- **Reason:** Need for robust error handling coverage
- **Impact:** Improved reliability and debugging

**REVISION 5: CI/CD Pipeline**
- **Change:** Full CI/CD with automated build, tests, and coverage reporting
- **Reason:** Incremental delivery requirements
- **Impact:** Automated quality checks and easier collaboration

---

### Increment 2: Documentation & Code Quality
**Target Date: Lab 12.2**  
**Status: COMPLETED**

**Overall Goal:** Comprehensive documentation and code quality improvements

**Note:** Increment 2 focused on documentation rather than new features, as core functionality was already complete and well-tested.

#### Lead Developer Contributions:
- Comprehensive documentation for `vec.c/h`, `hmap.c/h`, `row.c/h`, `main.c` ✅
- Documented orchestration logic and memory ownership ✅
- Updated all file headers to v2.0.0 ✅

#### CLI/I/O Owner Contributions:
- Comprehensive documentation for `csv.c/h` and `cli.c/h` ✅
- Documented parsing logic and error handling ✅
- Updated file headers to v2.0.0 ✅

#### Query Operations Team Member 1 (Nadeem) Contributions:
- Comprehensive documentation for `select.c/h` and `where.c/h` ✅
- Documented filtering logic and memory ownership ✅
- Updated file headers to v2.0.0 ✅

#### Query Operations Team Member 2 (Vivek) Contributions:
- Comprehensive documentation for `group.c/h` and `sort.c/h` ✅
- Documented grouping and sorting behavior ✅
- Updated file headers to v2.0.0 ✅

**Increment 2 Success Criteria:**
- ✅ All source files have comprehensive function documentation (following `row.c`/`hmap.c` style)
- ✅ All file headers updated to v2.0.0
- ✅ Function comments document behavior, edge cases, and memory ownership
- ✅ All tests passing (unit and integration)
- ✅ Test coverage >70% (main.c >85%)
- ✅ Group reflection report completed

---

### Increment 2 Revisions (Based on Experience)

**REVISION 1: Focus Shift to Documentation**
- **Change:** Increment 2 focused on documentation and code quality rather than advanced features
- **Reason:** Core functionality complete; documentation prioritized for final delivery
- **Impact:** Production-ready codebase with comprehensive documentation

**REVISION 2: Documentation Standards**
- **Change:** Established consistent documentation style
- **Reason:** Need for uniform code documentation across all modules
- **Impact:** Improved code readability and maintainability

**REVISION 3: Function Behavior Documentation**
- **Change:** Enhanced function comments to document behavior (how it works, edge cases, memory management) rather than just purpose
- **Reason:** Better understanding for future maintenance and debugging
- **Impact:** Reduced cognitive load when reading and modifying code
