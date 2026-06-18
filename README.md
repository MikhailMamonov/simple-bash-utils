# Simple Bash Utilities (`cat` & `grep`)

A custom implementation of the standard GNU text processing utilities **`cat`** and **`grep`** written in **C**. This project focuses on POSIX-compliant command-line argument parsing, file I/O operations, and regular expression processing.

## 📁 Project Structure

The source code is organized into two main utility directories:
*   `src/cat/` — Source code and Makefile for the `my_cat` utility.
*   `src/grep/` — Source code and Makefile for the `my_grep` utility.

---

## 🛠️ Compilation & Build

The project is built using a `Makefile` with standard GNU compiler flags (`-Wall -Werror -Wextra -std=c11`).

To compile both utilities at once, navigate to the `src` directory and run:
```bash
make all
```

### Individual Compilation:
*   **Compile cat only:** `cd cat && make my_cat`
*   **Compile grep only:** `cd grep && make my_grep`
*   **Clean build artifacts:** `make clean`

---

## 🚀 Features & Supported Flags

### 1. `my_cat`
A utility for concatenating and displaying file content.

| Flag | Description |
| :--- | :--- |
| `-b` | Numbers non-blank output lines. |
| `-e` | Implies `-v` and displays `$` at the end of each line. |
| `-n` | Numbers all output lines. |
| `-s` | Squeezes consecutive empty output lines into a single blank line. |
| `-t` | Implies `-v` and displays tabs as `^I`. |
| `-v` | Displays non-printing characters (except tabs and newlines). |

**Usage Example:**
```bash
./my_cat -n filename.txt
```

### 2. `my_grep`
A pattern matching utility using POSIX Extended Regular Expressions (ERE).

| Flag | Description |
| :--- | :--- |
| `-e` | Specifies a matching pattern (allows multiple patterns). |
| `-i` | Ignores case distinctions in both the pattern and input files. |
| `-v` | Inverts the match (selects non-matching lines). |
| `-c` | Prints only a count of matching lines per file. |
| `-l` | Prints only the names of files containing matches. |
| `-n` | Prefixes each output line with its 1-based line number. |
| `-h` | Suppresses the prefixing of file names on output. |
| `-s` | Suppresses error messages about nonexistent or unreadable files. |
| `-f` | Obtains patterns from a specified file, one per line. |
| `-o` | Prints only the matched (non-empty) parts of a matching line. |

**Usage Example:**
```bash
./my_grep -i "error" logfile.txt
./my_grep -e "pattern1" -e "pattern2" text.txt
```

---

## 🧪 Testing

The implementation can be verified against the original system `cat` and `grep` utilities using automated automated test scripts (`test.sh`). The scripts compare the binary outputs using the `diff` utility to ensure 100% behavior matching.

To run tests:
```bash
sh test.sh
```
