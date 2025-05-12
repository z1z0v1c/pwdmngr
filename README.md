# pwdmngr

![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/z1z0v1c/pwdmngr/makefile.yml?branch=main)
![Valgrind](https://img.shields.io/badge/valgrind-%20checked-success)
![Last Commit](https://img.shields.io/github/last-commit/z1z0v1c/pwdmngr)

A simple password management tool written in **C** that uses **SQLite** for data storage. Developed for **Linux** systems as a learning project.

[![C](https://img.shields.io/badge/language-C-darkgrey.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![SQLite](https://img.shields.io/badge/database-SQLite-blue.svg)](https://www.sqlite.org/)
[![Linux](https://img.shields.io/badge/platform-Linux-red.svg)](https://www.linux.org/)

## Features

- User registration and login system with credential validation
- Password generation with customizable options:
  - Configurable password length
  - Character type selection
- Account management:
  - Save new accounts
  - Edit existing accounts
  - Delete accounts
  - List all stored accounts
- Option to use generated passwords or custom ones
- Memory leak detection through Valgrind integration

## Project Structure

The application includes the following files:

| Source Files | Header Files | Description |
|--------------|--------------|-------------|
| main.c       | -            | Main program entry point, handles database connection and option selection |
| application.c | application.h | Functions for handling user-selected options |
| database.c   | database.h   | Database manipulation functions |
| helper.c     | helper.h     | Utility functions for user input, memory management, and password hashing |
| user.c       | user.h       | User information data structure and operations |
| account.c    | account.h    | Account information data structure and operations |

Additional files:
- `database.sql`: SQL schema for database creation
- `Makefile`: Builds the project

## Error Handling

- Provides clear notifications for login errors, specifying which credential is incorrect
- Attempting to use an ungenerated password will notify the user and cancel changes
- Trying to edit a non-existent account will show a warning
- Deleting a non-existent account fails silently
- Memory is managed properly in all edge cases

## Requirements & Installation

1. Install required dependencies:
   - `gcc`, `make`
   - `openssl` (check for `openssl/evp.h`)
   - `sqlite3`, `libsqlite3-dev`

2. Initialize the database:

   ```bash
   sqlite3 pwdmngr.db < database.sql
   ```
   
   Alternatively:
   ```bash
   sqlite3 pwdmngr.db
   .read database.sql
   .exit
   ```

3. Build and run the application:

   ```bash
   make
   ./main
   ```

4. (Optional) Run with Valgrind for memory leak detection:
   ```bash
   valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind.txt ./main -lsqlite3
   ```
   
   Results will be available in `valgrind.txt` after exiting the application.
