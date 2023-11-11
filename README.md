# PWDMNGR - A password manager

## Description

A simple password management tool written in the C programming language. Stores data in an SQLite database. Runs on Linux.
Written for learning purposes.

Requires user registration and login. If the user provides incorrect credentials for login it'll be notified which one is incorrect.

Provides options for:

      * Password generation
      * Save, edit, delete accounts
      * List of all user accounts

User can choose whether he wants to use a generated password for creating and editing an account or to provide one.
If the user tries to use a generated password that hasn't been generated yet, it will be notified and the changes won't be saved.
Memory management is also handled in this situation.

To generate a password, the user can choose several options:

      * Password length
      * Type of characters (uppercase and lowercase letters, numbers, and/or special characters)
        - at least one character type must be included

If the user tries to edit an unexisting account it will be notified.
If the user tries to delete an unexisting account it will fail silently.

An account is used to store information about user accounts across the web. Contains:

      * Site name
      * Username
      * Password

After the application is started, valgrind-out.txt will be created, and after closing the application the memory information will be available.

## Files

The application includes the following source files and their corresponding header files:

      * main.c - contains the main function that handles database connection,
        and provides option selection for register/login and account manipulation.
      * application.c and application.h - contain functions for handling selected options
      * database.c and database.h - contain functions for the database manipulation
      * helper.c and helper.h - contain helper functions for getting user input, freeing memory, and hashing a password
      * user.c and user.h - contains user information data
      * account.c and account.c - account information

It also includes the database.sql file for creating the database and the Makefile for compilation.

## Requirements and installation

      * Install gcc and GNU Make if not already installed
      * Make sure openssl/evp.h is present on the system
      * Install sqlite3 and libsqlite3-dev and compile sqlite3.h
      * Run 'sqlite3 pwdmngr.db' -> run '.read database.sql'
      * For checking memory leaks and errors run:
           valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./main -lsqlite3
