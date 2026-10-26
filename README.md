# Huffpress!
<p align="center">
  <img src="./huffpress-demo.gif" alt="Huffpress Demo" width="100%">
</p>

**Huffpress!** is a high-performance, cross-platform, dependency-free C++ Huffman compression suite designed for rock-solid binary integrity and extreme stability. Engineered from the ground up to process massive files (such as the 150MB `rockyou.txt` stress-test) without memory exhaustion, bit corruption, or OS-level newline translation bugs.

---

## 🚀 Key Architecture & Technical Highlights

* **Constant-Memory Bit-Streaming (`writeModule` & `readModule`)**: Replaces inefficient string-based bit manipulation with low-level bit-shifting buffers, guaranteeing a constant $O(1)$ memory footprint regardless of file size.
* **Sentinel-Based Hard-Stop Header**: Completely eliminates fragile padding-byte heuristics by embedding an 8-byte `unsigned long long` total character count sentinel at the head of every `.huf` file, giving decoders a strict, exact termination boundary.
* **Strict Binary Integrity (`ios::binary`)**: Enforces strict binary stream operations across all encoders, decoders, and diagnostics to prevent cross-platform carriage-return (`\r\n`) corruption.
* **Advanced Error Diagnostics & Validation**: Features an exhaustive integrity suite (`verifyStreamIntegrity` and `runDetailedDiagnostics`) capable of pinpointing byte content mismatches, length divergences, and bitstream corruption with granular error codes and indices.
* **Edge-Case Resilience**: Fully handles single-unique-character edge cases, empty files, and utilizes recursive heap-clearing destructors to eliminate memory leaks.

---

## 📂 Project Structure

### 🧬 `HuffmanTree.hpp`: Core Data Structures & Tree Synthesis Engine

`HuffmanTree.hpp` serves as the foundational analytical and structural engine of **Huffpress!**. It handles file frequency scanning, custom priority queue sorting, greedy Huffman tree building, prefix code generation, and rich terminal visualizations (including ANSI heatmapping and hierarchical tree rendering).

---

### 1. Core Data Structures

* **`struct huffNode`**:
  * Represents individual nodes in the binary Huffman tree.
  * Stores an `unsigned char myChar` for leaf characters and an `unsigned long long myFreq` for node weight.
  * Features left and right child pointers (`huffNode* left`, `huffNode* right`).
  * Implements a **recursive destructor (`~huffNode()`)** that automatically cascades `delete` calls down the entire subtree, ensuring that disposing of the root node completely flushes the heap with zero memory leaks.


* **`struct codeNode`**:
  * Manages variable-length binary code strings (`string myCode`) mapped to specific characters, alongside an `bool isActive` flag to verify active encoding status.



---

### 2. Custom Priority Queue (`pQueue`)

To build an optimal Huffman tree, nodes must be continuously sorted by frequency in ascending order. Instead of relying on STL containers, `Huffpress!` implements a custom singly-linked **Priority Queue (`pQueue`)**:

* **`enqueue(huffNode* data)`**: Inserts nodes dynamically in sorted order based on their frequency (`myFreq`), ensuring the lowest-frequency nodes always sit at the front of the queue.
* **`dequeue()`**: Extracts the minimum-frequency node from the front of the list in $O(1)$ time, maintaining high execution speed even during massive iterations.

---

### 3. Custom Square Root Approximation (`mySqrt`)

* **Babylonian Method Implementation**: Rather than depending on heavier math libraries or floating-point overhead, `mySqrt` implements an 8-iteration Newton-Raphson approximation loop to calculate frequency square roots. This drives the terminal heatmapping color distribution cleanly and efficiently.

---

### 4. The `HuffmanTree` Class Workflow

Upon initialization with a target filename, the `HuffmanTree` class executes an automated pipeline:

1. **Occurrence Mapping (`createOccurenceMap`)**:
* Opens the file in binary mode (`ios::binary`) to prevent newline translation corruption.
* Iterates through every single byte, populating a 256-entry frequency table (`map[256]`) and accumulating `totalChars`.


2. **Queue Generation (`createQueue`)**:
* Iterates through the frequency map and pushes active characters into the custom priority queue wrapped inside `huffNode` instances.


3. **Tree Synthesis (`createTree`)**:
* Implements greedy Huffman tree construction. It dequeues the two lowest-frequency nodes, merges them under a parent node whose frequency equals their sum, and re-queues the parent.
* Handles single-character edge cases cleanly by creating a synthetic root wrapper when queue count equals 1.


4. **Code Generation (`createCodes`)**:
* Recursively traverses the finalized tree, appending `"0"` for left branches and `"1"` for right branches to build unique prefix-free binary codes for every character.



---

### 5. Terminal Diagnostics & Visualization Suite

`HuffmanTree` includes advanced ANSI-styled terminal inspection tools:

* **ANSI Heatmapped Text (`printHeatmappedText`)**:
* Calculates relative frequency ratios using square-root scaling against the maximum text frequency.
* Maps character density dynamically across a 216-color ANSI terminal spectrum (from cool blues to hot reds) to visually highlight text patterns and frequency distribution directly in the console.


* **Hierarchical Tree Visualizer (`printTreeRecursive` & `displayTree`)**:
* Renders ASCII box-drawing characters (`├─`, `└─`, `│`) to draw the Huffman tree structure top-to-bottom in the terminal, complete with escape handling for whitespace characters (`\n`, `\t`, `\r`, `[space]`) and node frequencies.


* **Frequency Table (`displayFrequencies`)**:
* Dumps a clean inventory of every active byte and its exact occurrence count.


* **Code Inspector (`displayCodesRecursive`)**:
* Recursively lists every character alongside its assigned variable-length binary Huffman code.

---
### ⚙️ `EncoderModule.hpp`: Bit-Packing Engine & Serialization Pipeline

`EncoderModule.hpp` contains the low-level bit-manipulation engine (`writeModule`) and the core compression orchestrator (`EncoderModule`). Together, they translate in-memory structures and raw text streams into a compressed, tightly packed binary `.huf` file format.

---

### 1. `writeModule`: Constant-Memory Bit-Packing Buffer

Because files are stored on disk in 8-bit bytes but Huffman codes have variable lengths (e.g., 3 bits, 7 bits, 12 bits), data cannot be written straight to disk character-by-character without packing. `writeModule` manages this bit-level buffering with an $O(1)$ memory footprint:

* **Internal State Buffers**:
* `unsigned char buf`: A rolling 8-bit register holding accumulated bits before they are flushed to disk.
* `int ctr`: Tracks how many bits are currently stored in `buf` (ranging from `0` to `8`).
* `ofstream file`: Manages the output binary file stream.


* **Bit-Shifting Mechanics (`shiftAddBit`)**:
* Every time a bit is pushed, `buf` shifts left by one (`buf = (buf << 1) | bit`) and `ctr` increments.
* Once `ctr == 8`, the buffer is full, triggering an automatic flush to disk via `writeToFile()`.


* **Byte Serialization (`writeByte`)**:
* Deconstructs a full 8-bit character into individual bits by iterating from bit 7 down to 0, feeding each bit through `writeBit()`. This is used for writing the tree header's literal leaf characters.


* **Padding & Flushing (`writePadding`)**:
* Ensures that if the final compressed byte is incomplete (i.e., `ctr > 0`), remaining bit slots are padded with zeros and safely pushed out to disk so no trailing data is trapped in memory.
---


### 2. `EncoderModule`: File Compression Orchestrator

The `EncoderModule` class coordinates the overall compression lifecycle in `compressFile(...)`:

#### Step 1: Sentinel Header Generation

* Before writing any structural or compressed data, it takes the `totalChars` count (an 8-byte `unsigned long long`) from the `HuffmanTree`, casts its memory address to a `char*`, and streams all 8 physical bytes directly to the beginning of the output file using `io.writeByte()`. This establishes the strict termination boundary for decoders.

#### Step 2: Tree Header Serialization (`generateTreeHeaderString`)

* Recursively walks the Huffman tree using a pre-order traversal pattern:
* Emits `"0"` for internal nodes.
* Emits `"1"` followed by the raw character byte for leaf nodes.


* This string representation is serialized bit-by-bit into the output stream, allowing the decoder to perfectly reconstruct the exact shape of the Huffman tree without needing a lookup table.

#### Step 3: Payload Stream Compression

* Re-opens the source plaintext file in binary mode (`ios::binary`).
* Iterates through every character, fetches its pre-calculated variable-length prefix code from `hTree.codes`, and streams its individual bits (`0` and `1`) into the `writeModule` bit packer.

#### Step 4: Finalization

* Closes the plaintext file handle and invokes `io.writePadding()` to flush any remaining bits in the rolling buffer.

---
### 🔓 `DecoderModule.hpp`: Bit-Stream Reader & Forensic Integrity Suite

`DecoderModule.hpp` is the heavy-lifting extraction and verification engine of **Huffpress!**. It encapsulates the bit-level reader module (`readModule`), tree reconstruction logic, file decompression handlers, and the exhaustive forensic diagnostic system used to guarantee bit-for-bit file integrity.

---

### 1. Diagnostics Infrastructure (`IntegrityStatus` & `IntegrityReport`)

To track and categorize failures during validation testing against massive datasets like `rockyou.txt`, the module defines a robust error framework:

* **`enum IntegrityStatus`**: Maps structural and verification outcomes into clear error categories (`MATCH_OK`, `ERROR_FILE_NOT_FOUND`, `ERROR_TREE_BUILD_FAILED`, `ERROR_LENGTH_MISMATCH`, `ERROR_BYTE_MISMATCH`, `ERROR_BITSTREAM_CORRUPT`).
* **`struct IntegrityReport`**: Packs comprehensive forensic metadata when a test fails—tracking `totalBytesVerified`, `totalBitsProcessed`, the exact `mismatchIndex` offset, and a side-by-side comparison of the `expectedChar` versus the `actualChar`.

---

### 2. `readModule`: Sentinel Ingestion & Bit-Buffer Management

`readModule` handles reading compressed binary streams bit-by-bit while automatically processing the file headers:

* **Automatic Sentinel Consumption (`totalChars`)**:
* In its constructor, `readModule` immediately reads the first 8 bytes of the compressed file stream into `unsigned long long totalChars`. This eliminates header boilerplate across all decoders and diagnostic routines.


* **Low-Level Bit Streaming (`readBit`)**:
* Manages an 8-bit rolling buffer (`workingBuf`). When bits are exhausted (`remBits == 0`), it pulls a fresh byte from the file via `file->get()`, returning individual bits (`0` or `1`) or `-1` on EOF.


* **Literal Byte Reconstruction (`readByte`)**:
* Pulls 8 consecutive bits from the stream and packs them back into an 8-bit value. This is critical during tree reconstruction when reading literal characters belonging to leaf nodes.



---

### 3. `DecoderModule` Core Pipeline

The `DecoderModule` class orchestrates tree deserialization, file writing, and stream verification:

#### Step 1: Recursive Tree Deserialization (`retrieveTree`)

* Rebuilds the exact topology of the Huffman tree directly from the compressed bitstream layout:
* Reads a bit: if it encounters a `1`, it reads the subsequent 8-bit literal character via `readByte()` and instantiates a leaf node.
* If it encounters a `0`, it creates an internal node and recursively builds its left and right children.



#### Step 2: Exact-Count Decompression (`decodeFileAndSave`)

* Initializes `readModule` (automatically consuming the 8-byte sentinel).
* Rebuilds the Huffman tree structure.
* Enters a precise decoding loop bounded strictly by `io.totalChars` (the sentinel limit), traversing down the tree based on incoming bits (`0` for left, `1` for right) and writing decoded characters out to disk. This completely avoids false EOF errors or padding interference.

#### Step 3: Forensic Integrity Suite (`verifyStreamIntegrity` & `runDetailedDiagnostics`)

* Opens both the original source plaintext file and the compressed file in binary mode (`ios::binary`).
* Simultaneously decodes the compressed bitstream while reading bytes sequentially from the source file.
* Uses **unsigned char casting** (`unsigned char ch`) to prevent sign-extension traps on high-bit binary characters (such as passwords in `rockyou.txt`), ensuring accurate byte-to-byte comparison and eliminating false mismatches.
* Tracks exact bit processing counts and byte offsets, returning granular diagnostic codes if length divergences, corruption, or content mismatches occur.

---
### 🎛️ `HuffmanManager.hpp`: Controller & UI Bridge Layer

`HuffmanManager.hpp` acts as the high-level orchestration controller and architectural bridge between the user interface and the underlying subsystems (`HuffmanTree`, `EncoderModule`, and `DecoderModule`). It encapsulates state management, file loading, action dispatching, and diagnostic reporting behind a clean, printless API.

---

### 1. State Tracking & Session Management

The manager maintains the current runtime context of active file pairs and validation states:

* **`activeTxt` & `activeHuf**`: Tracks the string identifiers of currently loaded plaintext files (stored in `.\Plaintexts\`) and compressed `.huf` archives (stored in `.\Compressed\`).
* **`txtLoaded` & `hufLoaded**`: Boolean flags used by the UI to lock or unlock restricted actions (e.g., preventing compression or integrity checks when assets are missing).
* **`currentTree`**: A pointer to the active `HuffmanTree` instance loaded into RAM, ensuring that analytical inspections operate on the exact frequency layout of the current dataset.
* **`integrityStatus` & `isChecked**`: Caches the results of the latest stream verification pairing.

---

### 2. Core Operational Modules

#### File Loading & Initialization (`loadPlaintext` & `loadCompressed`)

* **Plaintext Loading**: Verifies file accessibility, cleans up any existing in-memory tree instances to prevent memory leaks, and instantiates a new `HuffmanTree` object which automatically triggers frequency occurrence mapping and queue creation.
* **Compressed Loading**: Validates the existence and accessibility of compressed archive files before marking `hufLoaded` as true.

#### Polymorphic Inspectors (`displayText`, `displayFrequencies`, `displayTree`, `displayCodes`)

* Implements a polymorphic display switch:
* **Plaintext Mode (`isPlain == true`)**: Direct inspection of the in-memory `currentTree`.
* **Compressed Archive Mode (`isPlain == false`)**: Safely decodes or extracts the file structure from the `.huf` file on-the-fly (utilizing temporary files or stream header reading via `DecoderModule` and `readModule`) to display heatmaps, frequencies, trees, or codes directly from the compressed archive.



#### Compression & Decompression Pipelines (`executeCompression` & `executeDecompression`)

* Wraps `EncoderModule` and `DecoderModule` calls, automatically formatting standard paths and destination suffixes (with custom override support or default naming based on `activeTxt` / `activeHuf`).

---

### 3. Analytics & Forensic Diagnostics

* **Compression Metrics (`getCompressionStats`)**:
* Leverages binary file positioning (`ios::binary | ios::ate`) to instantly query raw file sizes (`origSize` and `compSize`) from disk pointers without reading whole payloads into memory, calculating precise storage savings percentages.


* **Integrity Validation (`verifyPairs` & `runIntegrityReport`)**:
* Dispatches stream validation and detailed diagnostic tracing via `DecoderModule`, caching the output state to drive UI feedback screens.

---
### 🖥️ `Main.cpp`: Terminal User Interface & Application Entry Point

`Main.cpp` provides an interactive, color-coded ANSI terminal interface for **Huffpress!**. It coordinates user actions, enforces dynamic option locking based on loaded session states, and formats forensic diagnostic reports into readable, professional output blocks.

---

### 1. Terminal Styling & Color Macros

The file defines a rich set of ANSI escape sequence macros (`RESET`, `RED`, `GREEN`, `YELLOW`, `BLUE`, `MAGENTA`, `CYAN`, `WHITE`, `BOLD`, and `GRAY`) to deliver a modern, polished command-line user experience.

---

### 2. UI State Controller (`getStatusString` & `displayMenu`)

* **`getStatusString(...)`**: Evaluates whether both source and compressed files are active, mapping the underlying `IntegrityStatus` enum values (`MATCH_OK`, `ERROR_LENGTH_MISMATCH`, `ERROR_BYTE_MISMATCH`, `ERROR_BITSTREAM_CORRUPT`) into clean, color-coded status banners.
* **`displayMenu(...)`**:
* Clears the screen (`\033[H\033[2J`) and prints the main menu header along with the active `.txt` and `.huf` file tracking status.
* Implements **dynamic option graying/locking**: menu choices that require loaded files (such as payload inspections, compression, and integrity checks) are automatically dimmed using ANSI `GRAY` when prerequisites are unmet, guiding the user intuitively through the workflow.



---

### 3. Main Event Loop & Action Dispatcher (`main`)

The `main` function runs an infinite orchestration loop that continuously verifies session pairs (`app.verifyPairs()`), displays the menu, and routes inputs through a robust `switch` statement:

* **Choices 1 & 2 (File Loading)**: Prompts the user for filenames and loads plaintext files into memory (building occurrence maps and Huffman trees) or attaches compressed archives.
* **Choices 3–6 (Payload Inspection & Analysis)**: Handles polymorphic requests to inspect raw text, run character frequency analysis, view ASCII Huffman trees, or output prefix codes. If both `.txt` and `.huf` files are loaded, it prompts the user to choose whether to inspect the source file or decode the `.huf` archive on-the-fly.
* **Choices 7 & 8 (Compression & Decompression)**: Triggers core file operations with support for custom output naming or default source-derived naming (`NONE`).
* **Choice 9 (Compression Statistics)**: Queries raw binary file sizes on disk to calculate and display exact storage footprints, space-saving percentages, and compression ratios.
* **Choice 10 (Deep Content Integrity Check)**: Dispatches `runIntegrityReport()` to execute a forensic stream check, printing out detailed verification statistics, total bits parsed, and precise mismatch indices or expected-vs-actual character comparisons if an error occurs.
* **Choice 0 (Exit)**: Cleanly terminates the program session.
---

## 🛠️ Getting Started

### Prerequisites

* A modern C++ compiler supporting **C++11** or higher (e.g., GCC, Clang, MSVC).

### Compilation

Inside the Splits folder either compile Main.cpp or the One-Shot file using your preferred toolchain:

```bash
g++ -O3 -std=c++11 Main.cpp -o huffpress

```

---

## Menu Preview

Upon launching the application, you are greeted with an interactive terminal manager:

```
========================================
    Huffpress! - Encode & Decode Suite    
========================================
 Active .txt: rockyou.txt
 Active .huf: rockyou.huf
 Status:      [Ready / Verified]
----------------------------------------
 [1] Load Plaintext (.txt)
 [2] Load Compressed (.huf)
----------------------------------------
    Payload / Content Preview & Stats    
----------------------------------------
 [3] Read Loaded Text (Inspect in RAM)
 [4] Run Character Frequency Analysis
 [5] View Huffman Tree
 [6] View Generated Codes
----------------------------------------
          Save As .txt / .huf            
----------------------------------------
 [7] Compress File (Load .txt first)
 [8] Decompress File (Load .huf first)
----------------------------------------
          Statistics & Integrity         
----------------------------------------
 [9]  View Compression Ratio & Stats
 [10] Run Content Integrity Check
----------------------------------------
 [0] Exit
========================================

```

---

## Error Codes

The integrated integrity checker reports back with clear diagnostic states:

* `MATCH_OK` (0): File streams match source files with absolute bit-for-bit precision.
* `ERROR_FILE_NOT_FOUND`: Target files could not be accessed.
* `ERROR_TREE_BUILD_FAILED`: Tree header reconstruction failed due to bitstream truncation.
* `ERROR_LENGTH_MISMATCH`: Premature EOF or unexpected trailing data detected.
* `ERROR_BYTE_MISMATCH`: Decoded character differs from the raw source byte at a specific index.
* `ERROR_BITSTREAM_CORRUPT`: Structural traversal failure mid-stream.

---

## 📝 License

This project is open-source and free to use for educational purposes.

# *Created by Renosanceprog*
