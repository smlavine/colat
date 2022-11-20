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
 * @file err.c
 * @version 2.0.0
 * @brief Main source code file
 * @details This file contains definitions and declarations of globally
 * available functions and variables.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* With glibc and perhaps some other environments, this variable is
 * declared in errno.h. But as it doesn't seem to cause any issues
 * compiling, We declare it again here without a preprocessor guard for
 * maximum portability. */
char *program_invocation_name;

/**
 * @details Prints program_invocation_name, ": ", and the
 * printf(3)-like-formatted error message.
 */
void
vwarn(const char *fmt, va_list ap)
{
	fprintf(stderr, "%s: ", program_invocation_name);
	vfprintf(stderr, fmt, ap);
}

/**
 * @details Calls vwarn(), then prints ": ", strerror(errno), and a newline.
 */
void
vewarn(const char *fmt, va_list ap)
{
	vwarn(fmt, ap);
	if (errno != 0) {
		/* To avoid two colons being printed, like
		 * "program_invocation_name: : No such file or directory" */
		if (fmt != NULL && fmt[0] != '\0') {
			fputs(": ", stderr);
		}
		fprintf(stderr, "%s", strerror(errno));
	}
	fputc('\n', stderr);
}

/**
 * @details Calls vewarn() and exits the program with the provided code.
 */
void
verrc(const int code, const char *fmt, va_list ap)
{
	vewarn(fmt, ap);

	exit(code);
}

/**
 * @details Calls verrc() with EXIT_FAILURE.
 */
void
verr(const char *fmt, va_list ap)
{
	verrc(EXIT_FAILURE, fmt, ap);
}

/**
 * @details Variadic wrapper for vwarn().
 */
void
warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vwarn(fmt, ap);
	va_end(ap);
}

/**
 * @details Variadic wrapper for vewarn().
 */
void
ewarn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vewarn(fmt, ap);
	va_end(ap);
}

/**
 * @details Variadic wrapper for verrc().
 */
void
errc(const int code, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verrc(code, fmt, ap);
	va_end(ap);
}

/**
 * @details Variadic wrapper for verr().
 */
void
err(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verr(fmt, ap);
	va_end(ap);
}
