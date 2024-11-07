# WELCOME TO THE EPIDRIVER KERNEL MODULE!

## General Introduction

This document provides instructions for setting up and running the Epidriver kernel module. 

The Epidriver module allows users to generate an infinite sequence of random bytes through a device file. After installation, this device file, named `epidriver`, can be found at `/dev/epidriver`.

The module includes four distinct levels, each described below.

## Description of the Existing Levels

- **Level 1:** Returns an infinite sequence of random numbers.
- **Level 2:** Returns an infinite sequence of random ASCII characters (including extended ASCII).
- **Level 3:** Returns an infinite sequence of random bytes, similar to `/dev/urandom`. This level uses the cryptographically secure `get_random_bytes()` function.
- **Level 4:** Returns an infinite sequence of random elements from a user-defined alphabet. The alphabet can be specified by setting the `alphabet` module parameter to a string of desired characters.

By default, the module operates at **Level 3** (or **Level 4** if an alphabet is provided). You can select **Level 1** or **Level 2** by setting the `level` module parameter accordingly.

## How to Install and Run the Module

1. **Build the Module:**
   - Run `make` in the folder containing the module files to generate the loadable kernel module object file (`epidriver.ko`) and its dependencies.

2. **Load the Module:**
   - Use the command `sudo insmod epidriver.ko` to load the module into the kernel.
   - After loading, a device file will automatically be created in `/dev`, which can be used to communicate with the driver (e.g., using `cat`).

   - **Setting Custom Alphabet:** To set a custom alphabet, use the `alphabet` parameter with `insmod`:
     ```bash
     sudo insmod epidriver.ko alphabet='abcdef'
     ```

   - **Selecting Levels:** To use **Level 1** or **Level 2**, specify the level with `insmod`:
     ```bash
     sudo insmod epidriver.ko level=1
     ```

3. **Interacting with the Driver:**
   - Use commands like:
     ```bash
     cat /dev/epidriver
     ```
     or `dd` to limit the number of bytes returned to the user.

4. **Unload the Module:**
   - Unload the module with:
     ```bash
     sudo rmmod epidriver
     ```
   - This will automatically delete the device file associated with the module.

5. **Clean Up:**
   - Use the `clean` target in the Makefile to delete all kernel object files from the directory.

## Simplifying Tests with `make`

The `make` command includes targets for testing all module use cases:

- **Default Level Test:** Run `make test` to load the module at its default level (Level 3).
- **Level-Specific Tests:** Use `make test_levelX` (replace `X` with a number between 1 and 4) to load the module at a specific level.

Each `make` test target first unloads the module (if loaded) and then reloads it. Recent kernel messages are displayed to provide information on the module's status.

---

Enjoy using the Epidriver module!
