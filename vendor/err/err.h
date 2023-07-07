/*
 * err - Small error-printing library for C
 * Copyright (C) 2021-2022 Sebastian LaVine <mail@smlavine.com>
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**
 * @file err.h
 * @version 2.0.0
 * @brief Header file
 * @details #include this to use err in your project.
 */

#include <stdarg.h>

/**
 * @brief Global value for the program's name.
 * @details This must be set before any err functions are called.
 */
extern char *program_invocation_name;

/**
 * @brief Prints a formatted warning message to stderr.
 * @pre program_invocation_name is set
 * @param fmt format string
 * @param ap va_list of arguments for the format string
 */
void vwarn(const char *, va_list);

/**
 * @brief Prints a formatted error message to stderr.
 * @pre program_invocation_name is set
 * @param fmt format string
 * @param ap va_list of arguments for the format string
 */
void vewarn(const char *, va_list);

/**
 * @brief Prints a formatted error message to stderr and exits.
 * @pre program_invocation_name is set
 * @param fmt format string
 * @param ap va_list of arguments for the format string
 */
void verr(const char *, va_list);

/**
 * @brief Prints a formatted error message to stderr and exits with the given
 * exit code.
 * @pre program_invocation_name is set
 * @param code exit code
 * @param fmt format string
 * @param ap va_list of arguments for the format string
 */
void verrc(const int, const char *, va_list);

/**
 * @brief Prints a formatted warning message to stderr.
 * @pre program_invocation_name is set
 * @param fmt format string
 * @param ... arguments for the format string
 */
void warn(const char *, ...);

/**
 * @brief Prints a formatted error message to stderr.
 * @pre program_invocation_name is set
 * @param fmt format string
 * @param ... arguments for the format string
 */
void ewarn(const char *, ...);

/**
 * @brief Prints a formatted error message to stderr and exits.
 * @pre program_invocation_name is set
 * @param fmt format string
 * @param ... arguments for the format string
 */
void err(const char *, ...);

/**
 * @brief Prints a formatted error message to stderr and exits with the given
 * exit code.
 * @pre program_invocation_name is set
 * @param code exit code
 * @param fmt format string
 * @param ... arguments for the format string
 */
void errc(const int, const char *, ...);
