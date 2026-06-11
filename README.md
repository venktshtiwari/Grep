# Custom C++ Grep Implementation

A lightweight, custom regular expression matching engine written in C++. This project emulates the core functionality of the standard `grep -E` command line tool, parsing and evaluating regex patterns against standard input.

## 🚀 Features

This regex engine supports a robust subset of standard regular expression features:

* **Anchors:**
  * `^` - Matches the start of a string.
  * `$` - Matches the end of a string.
* **Wildcards:**
  * `.` - Matches any single character.
* **Character Classes:**
  * `\d` - Matches any digit (0-9).
  * `\w` - Matches any alphanumeric character or underscore (`[a-zA-Z0-9_]`).
  * `[abc]` - Matches any single character in the set.
  * `[^abc]` - Matches any single character *not* in the set (negation).
* **Quantifiers:**
  * `+` - Matches one or more occurrences of the preceding token.
  * `?` - Matches zero or one occurrence of the preceding token.
* **Alternation (Groups):**
  * `(foo|bar)` - Matches either the exact sequence "foo" or "bar".

### ⚠️ Known Limitations

The following features are not yet supported:

* `*` quantifier (zero or more occurrences)
* Nested groups
* Capture groups / backreferences

Unsupported pattern syntax may produce unexpected results.

## 🛠️ Prerequisites

To compile this project, you will need a C++ compiler that supports **C++17** or higher, as the implementation heavily utilizes `std::string_view` for efficient substring matching without unnecessary memory allocation.

* GCC (g++)
* Clang
* MSVC

## ⚙️ Compilation

Compile the source code using `g++` (or your preferred compiler):

```bash
g++ -std=c++17 -O3 -o mygrep main.cpp
```

## 💻 Usage

The program expects exactly two arguments: the `-E` flag (to denote extended regular expressions) and the pattern itself. Input is read from stdin.

```bash
echo "input_string" | ./mygrep -E "pattern"
```

### Exit Codes

* `0` — A match was found.
* `1` — No match was found, or an error/invalid argument occurred.

## 📖 Examples

**1. Basic matching:**
```bash
echo "hello world" | ./mygrep -E "world"
# Exit code: 0
```

**2. Character classes and quantifiers:**
```bash
echo "apple123" | ./mygrep -E "\w+\d+"
# Exit code: 0
```

**3. Alternation:**
```bash
echo "I like cats" | ./mygrep -E "(dogs|cats)"
# Exit code: 0
```

**4. Start and end anchors:**
```bash
echo "hello" | ./mygrep -E "^he"
# Exit code: 0

echo "hello" | ./mygrep -E "lo$"
# Exit code: 0
```

## 🧠 How It Works

The matching engine uses a recursive backtracking approach to evaluate patterns:

* **`match_pattern`** — Handles unanchored searches by checking every possible starting position in the input string, unless the `^` anchor is present.
* **`match_here`** — Recursively consumes the pattern and the input string. It evaluates alternations, parses tokens (literals, escape sequences, bracket expressions), and applies quantifiers (`+`, `?`) using a greedy strategy — trying the longest match first and backtracking if the remainder of the pattern fails to match.

## 🤝 Contributing

Feel free to fork this repository, open issues, or submit pull requests to add new regex features (such as the `*` quantifier, nested groups, or capture groups).
