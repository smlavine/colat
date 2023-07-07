/**
 * @example weekday.c
 * This example demonstrates how the library makes it easy to concisely express
 * repetitive errors.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../err.h"

void
usage(void)
{
	fputs("usage: weekday YYYY-MM-DD\n", stderr);
}

int
main(int argc, char *argv[])
{
	struct tm tm = {0};
	char *temp;
	int year, month, day;
	char linebuf[64];

	program_invocation_name = argv[0];

	if (argc != 2) {
		usage();
		return EXIT_FAILURE;
	}

	/* Note that when errno == 0, strerror(errno) is not included in
	 * the output of err(). */

	if ((temp = strtok(argv[1], "-")) == NULL)
		err("bad date: no year");
	else if ((year = atoi(temp)) == 0)
		err("bad date: invalid year");

	if ((temp = strtok(NULL, "-")) == NULL)
		err("bad date: no month");
	else if ((month = atoi(temp)) == 0)
		err("bad date: invalid month");

	if ((temp = strtok(NULL, "-")) == NULL)
		err("bad date: no day");
	else if ((day = atoi(temp)) == 0)
		err("bad date: invalid day");

	/* See ctime(3) */
	tm.tm_year = year - 1900;
	tm.tm_mon  = month - 1;
	tm.tm_mday = day;

	if (mktime(&tm) == (time_t)-1)
		err("mktime failed");

	strftime(linebuf, sizeof(linebuf), "%Y-%m-%d: %A", &tm);
	puts(linebuf);

	return EXIT_SUCCESS;
}
