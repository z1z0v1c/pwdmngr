# PVDMNGR - Password manager

## Video Demo: <URL HERE>

## Description

A simple password management tool written in the C programming language that uses an SQLite database for storage.

Requires user registration and login.

Provides options for:

      * Password generation
      * Save, edit, delete "accounts"
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

      * main - two loops used to select options
      * application - handling of selected options
      * database - database manipulation
      * helper - helper functions mainly for getting user input
      * user - user information
      * account - account information

It also includes a database.sql file for creating the database and a Makefile for compilation.

## Requirements and installation

      * Install gcc and GNU Make if they are not already installed
      * Make sure openssl/evp.h is present on the system
      * Install sqlite3 and libsqlite3-dev and compile sqlite3.h
      * Run 'sqlite3 pwdmngr.db' -> run '.read database.sql'
