/**
 * @example files.c
 * This example demonstrates when and how the ewarn() function is used.
 *
 * err() is called when a fatal error occurs. ewarn() is called when a nonfatal
 * error occurs.
 *
 * Compare this example with hello.c, which demonstrates warn(). In hello.c,
 * warn() is called after an unusual but managable situation with the program
 * arguments is found. In this example, ewarn() is called after a necessary
 * library function fails. We will get a more detailed error message with
 * ewarn() because it looks at errno when writing its message.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../err.h"

int
main(int argc, char *argv[])
{
	int i;

	program_invocation_name = argv[0];

	if (argc < 2)
		err("no files provided");

	for (i = 1; i < argc; i++) {
		FILE *fp;
		int c;

		if ((fp = fopen(argv[i], "r")) == NULL) {
			ewarn("%s", argv[i]);
			continue;
		}

		while ((c = fgetc(fp)) != EOF)
			putchar(c);

		fclose(fp);
	}

	return EXIT_SUCCESS;
}
