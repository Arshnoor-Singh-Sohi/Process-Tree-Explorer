# Process Tree Explorer

<div align="center">
  <img src="https://raw.githubusercontent.com/username/Process-Tree-Explorer/main/img/logo.png" alt="Process Tree Explorer Logo" width="200"/>
  
  ![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
  ![Language: C](https://img.shields.io/badge/Language-C-blue.svg)
  ![Platform: Linux](https://img.shields.io/badge/Platform-Linux-orange.svg)
  ![Version: 1.0](https://img.shields.io/badge/Version-1.0-purple.svg)
  
  **A powerful Linux process tree navigation and management utility**
</div>

## 📋 Table of Contents

- [Overview](#-overview)
- [Process Tree Concepts](#-process-tree-concepts)
- [Features](#-features)
- [System Requirements](#-system-requirements)
- [Installation](#-installation)
- [Usage](#-usage)
  - [Basic Syntax](#basic-syntax)
  - [Command Options](#command-options)
  - [Examples](#examples)
- [Technical Implementation](#-technical-implementation)
  - [Process Identification](#process-identification)
  - [Tree Traversal](#tree-traversal)
  - [Process Status Check](#process-status-check)
  - [Signal Handling](#signal-handling)
- [Code Structure](#-code-structure)
- [Key Algorithms](#-key-algorithms)
- [Visualizations](#-visualizations)
- [Detailed Flow Diagrams](#-detailed-flow-diagrams)
- [Safety Considerations](#-safety-considerations)
- [Troubleshooting](#-troubleshooting)
- [Future Enhancements](#-future-enhancements)
- [License](#-license)

## 🔍 Overview

Process Tree Explorer is a Linux command-line utility designed to navigate, analyze, and manipulate process hierarchies in Unix/Linux systems. It provides a comprehensive set of tools to explore parent-child relationships between processes, identify defunct (zombie) processes, and perform operations like process termination and signal delivery.

This tool is particularly useful for system administrators, developers, and anyone interested in understanding and managing process relationships on Linux systems. By leveraging the `/proc` filesystem, it provides insights that aren't easily accessible through standard Linux utilities.

## 🌳 Process Tree Concepts

In Linux, processes are organized in a hierarchical tree structure where each process (except the initial process, PID 1) has a parent process. Understanding these relationships is crucial for effective system management.

### Key Process Relationships

```
                        ┌────────┐
                        │  Init  │
                        │ (PID 1)│
                        └────┬───┘
                             │
           ┌─────────────────┼──────────────────┐
           │                 │                  │
      ┌────▼────┐       ┌────▼────┐        ┌────▼────┐
      │ Process │       │ Process │        │ Process │
      │    A    │       │    B    │        │    C    │
      └────┬────┘       └────┬────┘        └────┬────┘
           │                 │                  │
      ┌────▼────┐       ┌────▼────┐        ┌────▼────┐
      │ Process │       │ Process │        │ Process │
      │   A-1   │       │   B-1   │        │   C-1   │
      └────┬────┘       └────┬────┘        └─────────┘
           │                 │
      ┌────▼────┐       ┌────▼────┐
      │ Process │       │ Process │
      │   A-2   │       │   B-2   │
      └─────────┘       └─────────┘
```

In this diagram:
- **Root Process (Init)**: The first process started by the kernel, with PID 1
- **Parent Process**: Any process that has created child processes
- **Child Process**: A process created by another process (its parent)
- **Siblings**: Processes that share the same parent
- **Descendants**: All processes in the subtree below a given process
- **Ancestors**: All processes in the path from a given process to the root

### Special Process States

- **Defunct/Zombie Process**: A process that has completed execution but still has an entry in the process table. Marked with a `Z` state in Linux.
- **Orphan Process**: A process whose parent has terminated, adopted by the Init process (PID 1).
- **Stopped Process**: A process that has been paused using SIGSTOP or similar signals.

## ✨ Features

Process Tree Explorer offers a wide range of capabilities:

| Feature Type | Description | Options |
|--------------|-------------|---------|
| **Information Gathering** | View process relationships and details | Default, `-id`, `-ds`, `-lg`, `-gc` |
| **Status Checking** | Check process states | `-do`, `-lz`, `-df`, `-dc` |
| **Process Management** | Send signals to control processes | `--pz`, `-sk`, `-st`, `-dt`, `-rp` |

### Detailed Feature List

1. **Process Tree Navigation**
   - Check if a process belongs to a specific process tree
   - List immediate descendants (children) of a process
   - Find non-direct descendants (grandchildren, etc.)
   - Identify siblings of a given process
   - List grandchildren of a process

2. **Zombie Process Management**
   - Count defunct processes in a tree
   - List all defunct processes in a tree
   - List defunct sibling processes
   - Check if a specific process is defunct
   - Kill parents of zombie processes to allow cleanup

3. **Process Control Operations**
   - Send SIGKILL to terminate processes
   - Send SIGSTOP to pause processes
   - Send SIGCONT to resume stopped processes
   - Kill the root process of a tree

## 💻 System Requirements

- Linux operating system
- GCC compiler
- Access to the `/proc` filesystem
- Appropriate permissions to view and manipulate processes

## 🔧 Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Arshnoor-Singh-Sohi/Process-Tree-Explorer.git
   cd Process-Tree-Explorer
   ```

2. Compile the source code:
   ```bash
   gcc -o prct prct.c
   ```

3. (Optional) Make the executable available system-wide:
   ```bash
   sudo cp prct /usr/local/bin/
   ```

## 🚀 Usage

### Basic Syntax

The general syntax for running Process Tree Explorer is:

```
prct [root_process] [process_id] [Option]
```

Where:
- `root_process`: The PID of the process that is the root of the tree you want to explore
- `process_id`: The PID of the specific process you want to analyze
- `Option`: (Optional) The specific operation you want to perform

### Command Options

The available command options are divided into several categories:

#### Information Gathering Options

| Option | Description | Example Output |
|--------|-------------|----------------|
| (none) | Show basic process information | `1234 5678` (Shows PID and PPID) |
| `-id` | List immediate descendants | `1235, 1236, 1237` |
| `-ds` | List non-direct descendants | `1238, 1239, 1240` |
| `-lg` | List sibling processes | `1241, 1242, 1243` |
| `-gc` | List grandchildren processes | `1244, 1245, 1246` |

#### Status Checking Options

| Option | Description | Example Output |
|--------|-------------|----------------|
| `-do` | Check if process is defunct | `Defunct` or `Not defunct` |
| `-lz` | List defunct siblings | `1247, 1248` |
| `-df` | List defunct descendants | `1249, 1250, 1251` |
| `-dc` | Count defunct descendants | `3` |

#### Process Management Options

| Option | Description | Example Output |
|--------|-------------|----------------|
| `--pz` | Kill parents of zombie processes | `Killed parent 1252 of zombie 1253` |
| `-sk` | Kill all descendants | `Killed process: 1254` |
| `-st` | Stop all descendants | `Stopped process: 1255` |
| `-dt` | Continue stopped descendants | `Resumed process: 1256` |
| `-rp` | Kill the root process | `Killed process: 1257` |

### Examples

Here are some practical examples of using Process Tree Explorer:

#### Example 1: Basic Process Information

```bash
$ prct 1004 1009
1009 1005
```
This shows that process 1009 exists in the tree rooted at 1004, and its parent is 1005.

#### Example 2: Counting Defunct Processes

```bash
$ prct 1004 1005 -dc
3
```
This indicates that there are 3 defunct (zombie) processes among the descendants of process 1005.

#### Example 3: Listing Process Descendants

```bash
$ prct 1004 1005 -id
1008
1009
1029
```
This lists all immediate children of process 1005.

#### Example 4: Handling Process Trees Outside the Hierarchy

```bash
$ prct 1007 1005
Does not belong to the process tree
```
This indicates that process 1005 is not a descendant of process 1007.

#### Example 5: Checking Process State

```bash
$ prct 1008 1090 -do
Defunct
```
This shows that process 1090 is in a defunct state.

## 🔬 Technical Implementation

Process Tree Explorer leverages several Linux system programming techniques to provide its functionality:

### Process Identification

The program primarily uses the `/proc` filesystem to gather information about processes. This virtual filesystem provides interfaces to kernel data structures.

```c
// Function to check if process exists
int does_process_exist(int pid) {
    char file_path[100];
    sprintf(file_path, "/proc/%d/status", pid);
    
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        return 0; // Process doesn't exist
    }
    
    fclose(file);
    return 1; // Process exists
}
```

This function checks if a process exists by attempting to open its status file in the `/proc` filesystem. If the file opens successfully, the process exists.

### Tree Traversal

To determine relationships between processes, the program traverses the process tree using parent-child relationships:

```c
int is_process_in_tree(int root_process, int process_id) {
    // Check if both processes exist
    if (!does_process_exist(root_process) || !does_process_exist(process_id)) {
        return 0;
    }

    // Start with the process we want to check
    int current_pid = process_id;

    // Keep going up the tree until we either find root_process or reach init
    while (current_pid > 1) { // 1 is the init process
        // If we found the root_process, we're done!
        if (current_pid == root_process) {
            return 1;
        }

        // Get the parent of current process
        int parent_pid = get_parent_pid(current_pid);
        
        // If we couldn't get parent PID, something went wrong
        if (parent_pid == -1) {
            return 0;
        }

        // Move up to the parent
        current_pid = parent_pid;
    }

    // If we got here, we reached init without finding root_process
    return 0;
}
```

This function determines if a process is in a specific tree by starting at the target process and repeatedly finding its parent until either reaching the root process (success) or PID 1 (failure).

### Process Status Check

The program can check various process states, including detecting zombie processes:

```c
int is_defunct(int pid) {
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL) {
        return 0;
    }

    char line[256];
    int is_defunct = 0;

    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "State:", 6) == 0) {
            if (strchr(line, 'Z') != NULL) {
                is_defunct = 1;
            }
            break;
        }
    }
    fclose(status_file);
    return is_defunct;
}
```

This function checks if a process is defunct (zombie) by examining its State field in the status file. If the state contains 'Z', the process is a zombie.

### Signal Handling

For process management operations, the program uses signals like SIGKILL, SIGSTOP, and SIGCONT:

```c
void kill_all_descendants(int process_id) {
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);
    
    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL) {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0) {
        // Recursively kill descendants of this child
        kill_all_descendants(child_pid);

        // Kill this child process
        if (kill(child_pid, SIGKILL) == -1) {
            printf("Failed to kill process");
        } else {
            printf("Killed process: %d\n", child_pid);
        }
    }

    fclose(child_file);
}
```

This recursive function sends SIGKILL to all descendants of a given process, effectively terminating the entire subtree.

## 📂 Code Structure

The code is organized into several functional groups:

```
prct.c
│
├── Helper Functions
│   ├── does_process_exist()
│   ├── get_parent_pid()
│   ├── is_process_in_tree()
│   ├── is_root_process()
│   ├── is_defunct()
│   └── is_orphan()
│
├── Information Gathering
│   ├── list_immediate_descendants()
│   ├── list_non_direct_descendants()
│   ├── list_siblings()
│   └── list_grandchildren()
│
├── Status Checking
│   ├── check_if_defunct()
│   ├── list_defunct_siblings()
│   ├── count_defunct_descendants()
│   └── list_defunct_descendants()
│
├── Process Management
│   ├── kill_parents_of_zombies()
│   ├── kill_all_descendants()
│   ├── stop_all_descendants()
│   ├── continue_all_paused_descendants()
│   └── kill_root_process()
│
└── main()
```

The program follows a modular design where each function handles a specific task, making the code more maintainable and easier to understand.

## 🧩 Key Algorithms

### Recursive Tree Traversal

One of the most important algorithms in the program is the recursive tree traversal used to process all descendants of a given process:

```c
void check_descendants_recursive(int pid, int *count, int print_pids) {
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", pid, pid);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL) {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0) {
        // Check if this child is defunct
        if (is_defunct(child_pid)) {
            (*count)++; // Increment counter
            if (print_pids) { // Only print if -df option
                if (*count == 1) { // Print header only once
                    printf("Defunct descendants:\n");
                }
                printf("%d\n", child_pid);
            }
        }
        // Recursively check this child's descendants
        check_descendants_recursive(child_pid, count, print_pids);
    }

    fclose(child_file);
}
```

This algorithm:
1. Opens the children file for the current process
2. Reads each child PID
3. Processes the child (in this case, checking if it's defunct)
4. Recursively calls itself for each child
5. This continues until all descendants have been processed

### Process Tree Verification

Another important algorithm is the one used to verify if a process belongs to a specific tree:

```
START
  IF process_id doesn't exist OR root_process doesn't exist THEN
    RETURN FALSE
  END IF
  
  SET current_pid = process_id
  
  WHILE current_pid > 1 DO
    IF current_pid == root_process THEN
      RETURN TRUE
    END IF
    
    SET current_pid = parent of current_pid
    IF we couldn't get parent THEN
      RETURN FALSE
    END IF
  END WHILE
  
  RETURN FALSE (reached init without finding root_process)
END
```

This algorithm starts at the target process and traverses up the process tree until either finding the root process (success) or reaching PID 1 (failure).

## 📊 Visualizations

### Process Tree Example

The following diagram illustrates a sample process tree that can be navigated using Process Tree Explorer:

```
                    ┌──────────┐
                    │   Bash   │
                    │ PID=9999 │
                    └────┬─────┘
                         │
                         │
                    ┌────▼─────┐
                    │ PID=1004 │
                    └──┬──┬──┬─┘
                       │  │  │
           ┌───────────┘  │  └───────────┐
           │              │              │
      ┌────▼────┐    ┌────▼─────┐   ┌────▼────┐
      │ PID=1005│    │[PID=1006]│   │ PID=1007│
      └──┬──┬──┬┘    └──────────┘   └────┬────┘
         │  │  │                         │
         │  │  │                    ┌────▼────┐
  ┌──────┘  │  └─────────┐          │ PID=1071│
  │         │            │          └────┬────┘
┌─▼──┐   ┌──▼───┐  ┌─────▼───┐           │
│1008│   │[1009]│  │ [1029]  │      ┌────▼────┐
└─┬──┘   └──────┘  └─────────┘      │ PID=1072│
  │                                 └─────────┘
┌─▼──┐
│1010│
└─┬──┘
  │
  │
┌─▼────┐  ┌──────────┐
│ 1030 │  │ [1090]   │
└──────┘  └──────────┘
```

In this diagram:
- Processes in square brackets [PID] are defunct (zombie) processes
- PID 9999 is a Bash shell process that serves as the ultimate ancestor
- PID 1004 is the root of the main process tree

### Sample Command Outputs

Here's a visualization of what happens when various commands are run:

1. **Basic Process Info**: `prct 1004 1009`
   ```
   1009 1005
   ```
   This shows process 1009 exists and its parent is 1005.

2. **Counting Defunct Processes**: `prct 1004 1005 -dc`
   ```
   3
   ```
   This indicates there are 3 defunct processes in the tree rooted at process 1005.

3. **Listing Immediate Descendants**: `prct 1004 1005 -id`
   ```
   1008
   1009
   1029
   ```
   This shows the immediate children of process 1005.

4. **Checking Process Status**: `prct 1008 1030 -do`
   ```
   Not defunct
   ```
   This indicates process 1030 is not in a defunct state.

## 🔄 Detailed Flow Diagrams

### Main Program Flow

```
┌─────────────────┐
│  Parse Command  │
│  Line Arguments │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     No     ┌──────────────┐
│ Enough Arguments├──────────►│  Print Error  │
└────────┬────────┘            └──────────────┘
         │ Yes
         ▼
┌─────────────────┐     No     ┌──────────────┐
│ Process IDs > 0 ├──────────►│  Print Error  │
└────────┬────────┘            └──────────────┘
         │ Yes
         ▼
┌─────────────────┐     No     ┌──────────────┐
│Process Exists?  ├──────────►│  Print Error  │
└────────┬────────┘            └──────────────┘
         │ Yes
         ▼
┌─────────────────┐     No
│  Option Given?  ├──────────────┐
└────────┬────────┘              │
         │ Yes                   │
         ▼                       ▼
┌─────────────────┐     ┌──────────────────┐
│ Process Command │     │ Print Process &  │
│    Option       │     │   Parent Info    │
└────────┬────────┘     └──────────────────┘
         │
         ▼
┌─────────────────┐
│  Return Result  │
└─────────────────┘
```

### Process Relationship Verification Flow

```
┌─────────────────┐
│  Check Process  │
│ Tree Membership │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     No     ┌──────────────┐
│Both Processes   ├──────────►│  Return False │
│   Exist?        │            └──────────────┘
└────────┬────────┘
         │ Yes
         ▼
┌─────────────────┐
│ current_pid =   │
│   process_id    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     Yes    ┌──────────────┐
│ current_pid == 1├──────────►│  Return False │
└────────┬────────┘            └──────────────┘
         │ No
         ▼
┌─────────────────┐     Yes    ┌──────────────┐
│ current_pid ==  ├──────────►│  Return True  │
│  root_process?  │            └──────────────┘
└────────┬────────┘
         │ No
         ▼
┌─────────────────┐
│ current_pid =   │
│ parent of       │
│ current_pid     │
└────────┬────────┘
         │
         └─────────────┐
                       ▼
                  (repeat loop)
```

### Recursive Descendant Processing Flow

```
┌─────────────────┐
│  Start Process  │
│  (Parent PID)   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     No     ┌──────────────┐
│Can Open Children├──────────►│    Return     │
│    File?        │            └──────────────┘
└────────┬────────┘
         │ Yes
         ▼
┌─────────────────┐
│  Read Child PID │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     No     ┌──────────────┐
│  More Children? ├──────────►│ Close File &  │
└────────┬────────┘            │    Return     │
         │ Yes                 └──────────────┘
         ▼
┌─────────────────┐
│ Process Current │
│  Child Process  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   Recursively   │
│ Process Child's │
│  Descendants    │
└────────┬────────┘
         │
         └─────────────┐
                       ▼
                  (next child)
```

## ⚠️ Safety Considerations

When using Process Tree Explorer, especially for operations that manipulate processes, there are several important safety considerations:

1. **Preventing Fork Bombs**: When testing with process creation, be careful to avoid creating fork bombs that can overwhelm system resources. Always use the following command periodically:
   ```bash
   killall -u username
   ```

2. **Privilege Requirements**: Some operations may require elevated privileges depending on which processes you're trying to manipulate.

3. **System Stability**: Be cautious when killing processes as it might affect system stability if critical processes are terminated.

4. **Zombie Process Handling**: When dealing with zombie processes, remember that they can only be cleaned up by sending signals to their parent processes or waiting for the parent to terminate.

## 🔧 Troubleshooting

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| "Process doesn't exist" | Process terminated | Verify the process ID is still active |
| "Does not belong to the process tree" | Incorrect hierarchy | Check the process relationship with `ps -ef \| grep <PID>` |
| Permission denied errors | Insufficient privileges | Run with appropriate permissions for the target processes |
| No defunct processes found | No zombies in tree | This is normal if all processes are properly terminated |
| Unable to kill process | Process in uninterruptible state | Try different signals or check if process is in 'D' state |

## 🚀 Future Enhancements

Potential improvements for future versions of Process Tree Explorer include:

1. **Graphical Visualization**: Add the ability to generate visual representations of process trees
2. **Extended Process Information**: Include more details about each process (memory usage, CPU usage, etc.)
3. **Interactive Mode**: Create an interactive shell for navigating and managing process trees
4. **Remote System Support**: Add the capability to explore process trees on remote systems
5. **Process Monitoring**: Implement continuous monitoring of process states and relationships
6. **Configuration Options**: Allow customization of output formats and behavior

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

```
MIT License

Copyright (c) 2025 Arshnoor Singh Sohi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
