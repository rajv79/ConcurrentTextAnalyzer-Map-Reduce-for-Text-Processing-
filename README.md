# ConcurrentTextAnalyzer: Multithreaded Map-Reduce for Text Processing

## Overview
**ConcurrentTextAnalyzer** is a multithreaded map-reduce framework written in C, designed to efficiently process large text files. By dividing files into segments and processing them in parallel with worker threads, it significantly reduces processing time and improves text analysis performance.

---

## Features
- **Map-Reduce Framework**: Implements map-reduce for efficient text file analysis.
- **Multithreading**: Utilizes 8 worker threads for concurrent processing.
- **Performance Optimization**: Reduces processing time by up to 50% compared to sequential methods.
- **Flexible Input**: Works with large text files of varying sizes.

---

## Directory Structure
```
.
├── Makefile              # Build script for compiling the project
├── README.md             # Project documentation
├── common.h              # Common utilities and definitions
├── input-alice30.txt     # Sample input file: Alice in Wonderland excerpt
├── input-moon10.txt      # Sample input file: First Moon Landing Transcript
├── input-warpeace.txt    # Sample input file: War and Peace excerpt
├── main.c                # Main program to run the framework
├── mapreduce.c           # Map-reduce logic implementation
├── mapreduce.h           # Header file for mapreduce.c
├── mr-0.itm              # Intermediate file for worker 0
├── mr-1.itm              # Intermediate file for worker 1
├── mr-2.itm              # Intermediate file for worker 2
├── mr-3.itm              # Intermediate file for worker 3
├── mr-4.itm              # Intermediate file for worker 4
├── mr.rst                # Final results file
├── testinput.txt         # Example input file for testing
├── usr_functions.c       # User-defined functions for custom logic
├── usr_functions.h       # Header file for usr_functions.c
```

---

## Installation
### Prerequisites
- A C compiler (e.g., GCC).
- Make utility.
- POSIX-compliant operating system (Linux or macOS).

### Steps
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd ConcurrentTextAnalyzer
   ```
2. Build the project:
   ```bash
   make
   ```

---

## Usage
### Run the Program
1. Prepare input files (e.g., `input-alice30.txt`).
2. Execute the program:
   ```bash
   ./ConcurrentTextAnalyzer <input-file>
   ```
   Example:
   ```bash
   ./ConcurrentTextAnalyzer input-alice30.txt
   ```

### Outputs
- Letter and word counts are displayed in the terminal.
- Intermediate files (e.g., `mr-0.itm`) and result files (e.g., `mr.rst`) are generated.

---

## Customization
To customize text processing logic:
1. Edit `usr_functions.c` to modify or extend map or reduce logic.
2. Rebuild the project:
   ```bash
   make clean
   make
   ```

---
