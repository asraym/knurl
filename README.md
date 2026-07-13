# Knurl

Knurl is a high-performance static analysis utility written in modern C++ designed to map, analyze, and visualize architectural dependencies within large-scale codebases. 

In complex software repositories, deep and implicit file dependencies present a significant barrier to entry for incoming developers. Knurl parses codebase architectures to establish precise dependency graphs, allowing engineers to instantly determine the blast radius of a code modification and understand how individual files interact across the system.

---

## Key Features

* **High-Performance Traversal:** Leverages native C++17 filesystem mechanics to recursively index large directories with minimal overhead.
* **Intelligent Scope Pruning:** Implements active recursion disabling to bypass dependency environments (`venv`, `node_modules`) and version control directories (`.git`), optimizing execution speed.
* **Decoupled Architecture:** Built with a strict separation of concerns between CLI orchestration, directory scanning subsystems, and target language parsers.

---

## Repository Structure

```text
knurl/
├── CMakeLists.txt        # Cross-platform build orchestration
├── app/
│   └── main.cpp          # CLI entry point and argument parsing
└── scanning/
    ├── FileWalker.cpp    # Recursive filesystem traversal implementation
    └── FileWalker.hpp    # FileWalker interface definition
```

---

## Installation & Build

### Prerequisites
* A compiler with full **C++17** support (GCC 9+, Clang 10+, or MSVC 2019+)
* **CMake** (Version 3.16 or higher)

### Build Steps
Knurl utilizes CMake to generate platform-agnostic build environments. To compile the binary, execute the following commands from the project root:

```bash
# Generate build configuration files
cmake -B build -S .

# Compile the optimized executable
cmake --build build --config Release
```

The compiled executable will be located within the newly created `build/` directory.

---

## Usage

Run the compiled executable from the command line, passing the absolute or relative path of the target source repository as the primary argument:

```bash
./build/knurl /path/to/target/repository
```

---

## License

This project is open-source software licensed under the [MIT License](LICENSE).
