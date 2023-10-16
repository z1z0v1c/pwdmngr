# PWDMNGR - A password manager

## Description

A simple password management tool written in the C programming language that uses an SQLite database to store data.

Requires user registration and login.

Provides options for:

      * Password generation
      * Save, edit, delete accounts
      * List of all user accounts

To generate a password, the user can choose several options:

      * Password length
      * Type of characters (uppercase and lowercase letters, numbers and/or special characters) - at least one character type must be included

Account is used to store information about user accounts across the web. Contains:

      * Site name
      * Username
      * Password

## Files

The application includes the following source files and their corresponding header files:

      * main.c - contains main method that provides option selection
      * application.c - handles selected options
      * database.c - database manipulation
      * helper.c - helper functions mainly for getting user input
      * user.c - user information
      * account.c - account information

It also includes the database.sql file for creating the database and the Makefile for compilation.

## Requirements and installation

      * Install gcc and GNU Make if not already installed
      * Make sure openssl/evp.h is present on the system
      * Install sqlite3 and libsqlite3-dev and compile sqlite3.h
      * Run 'sqlite3 pwdmngr.db' -> run '.read database.sql'
