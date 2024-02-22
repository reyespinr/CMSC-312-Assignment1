# CMSC-312 Assignment 1

This repository contains a collection of programs demonstrating inter-process communication (IPC) methods, including message queues and socket programming.

## Structure

- `include/`: Header files for shared structures and utilities.
- `src/`: Source files for all IPC examples, including client-server socket programs and message queue examples.

## Building and Running

### Prerequisites

- CMake (version 3.13 or higher)
- A C++ compiler supporting C++11 or higher (e.g., g++ or clang)
- VSCode (optional, with CMake Tools extension for easy building and running)

### Using VSCode Extension

1. Open the project folder in VSCode.
2. Ensure CMake Tools extension is installed.
3. Configure the project by selecting a kit (compiler).
4. Build the project by clicking the build button (✓) in the bottom blue bar.
5. Run any executable by clicking on the play button next to the build button.

### Using Command Line

In the project root directory, run the following commands:

```bash
mkdir build && cd build      # Create and enter the build directory
cmake ..                     # Generate build system files
cmake --build .              # Compile the project
```
To run an executable, navigate to the build directory and execute the program:

```bash
./server                     # To run the server program
./client                     # To run the client program
```
Each program can be executed in separate terminal windows. Ensure the server is running before starting the client.

## Programs by Question

- **Question 1: Basic IPC using Shared Memory**
  - `ProcessA`: Initializes shared memory and waits for B and C processes.
  - `ProcessB`: Attaches and writes to shared memory managed by Process A.
  - `ProcessC`: Attaches and writes to shared memory managed by Process A.

- **Question 2: Process Forking**
  - `SeparateForks`: Demonstrates forking separate child processes for B and C from A.
  - `NestedForks`: Demonstrates nested forking with process C forked from within B.

- **Question 3: Message Queues**
  - `Spock`: Reads messages from a message queue.
  - `Kirk`: Writes messages to a message queue.

- **Question 4: Message Queue Modification**
  - `NewSpock`: Reads a sequence of numbers from a message queue, sorts them, and prints out.
  - `NewKirk`: Sends a sequence of numbers to be sorted to `NewSpock`.

- **Question 5: Socket Programming**
  - `Server`: Sorts numbers sent by clients over sockets and prints out sorted numbers.
  - `Client`: Sends a sequence of numbers to the server to be sorted.

For more detailed instructions on running individual examples, refer to the comments within each source file.
