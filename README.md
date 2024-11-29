# Custom Memory Allocation Library

## Project Overview

This project implements a custom memory allocation library with two distinct allocation strategies: Best Fit and First Fit. The implementation demonstrates low-level memory management techniques by replacing the standard `malloc()` and `free()` functions.

## Key Features

- Custom memory allocation strategies
- Memory pool management using `mmap()`
- Dynamic memory chunk tracking
- Metadata-based memory management
- Configurable allocation policies

## Implementation Architecture

### Memory Management Structure

```c
typedef struct block {
    size_t          size;       // Memory chunk size
    int             free;       // Allocation status
    struct block    *prev;      // Previous chunk reference
    struct block    *next;      // Next chunk reference
} Block;
```

### Allocation Strategies

#### Best Fit Strategy
- Selects the smallest free chunk that accommodates the requested memory
- Minimizes internal memory fragmentation
- Performs comprehensive chunk size comparison

#### First Fit Strategy
- Allocates the first available chunk that meets size requirements
- Faster allocation process
- Simplifies memory search algorithm

## Core Functions

### `malloc(size_t ask_size)`
- Initializes 20,000-byte memory pool on first allocation
- Rounds requested size to 32-byte multiples
- Implements chunk selection and splitting
- Handles zero-size requests by reporting maximum free chunk

### `free(char* ptr)`
- Marks memory chunk as free
- Supports adjacent chunk merging
- Reduces memory fragmentation
- Maintains linked list integrity

## Memory Pool Management

- Pre-allocates 20,000 bytes using `mmap()`
- Uses 32-byte metadata headers
- Supports dynamic chunk splitting
- Implements coalescing of free adjacent chunks

## Compilation and Usage

### Compilation
```bash
gcc -shared -fPIC bf.c -o bf.so  # Best Fit
gcc -shared -fPIC ff.c -o ff.so  # First Fit
```

### Execution
```bash
LD_PRELOAD=/path/to/bf.so ./main
LD_PRELOAD=/path/to/ff.so ./main
```

## Limitations

- Fixed 20,000-byte memory pool
- Requires manual memory management
- Not recommended for production environments
- Specific to Linux systems

## System Requirements

- Linux environment (Tested on Ubuntu 18.04)
- GCC compiler
- Basic understanding of memory management
