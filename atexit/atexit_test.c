/*	$OpenBSD: atexit_test.c,v 1.0 2002/07/29 21:45:00 dhartmei Exp $ */

/*
 * Copyright (c) 2002 Daniel Hartmeier
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Effort sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F30602-01-2-0537.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "/usr/src/lib/libc/stdlib/atexit.h"

extern struct atexit *__atexit;
extern void (*__cleanup)();

void	handle_first();
void	handle_middle();
void	handle_last();
void	handle_invalid();
void	handle_cleanup();
void	handle_signal(int);

static int counter;

int
main(int argc, char *argv[])
{
	int i;

	if (argc != 2 || (strcmp(argv[1], "-valid") &&
	    strcmp(argv[1], "-invalid"))) {
		fprintf(stderr, "%s -valid/-invalid\n", argv[0]);
		return (1);
	}
	fprintf(stderr, "main()\n");
	if (atexit(handle_last)) {
		perror("atexit(handle_last) failed");
		return (1);
	}
	for (i = 0; i < 65535; ++i) {
		if (atexit(handle_middle)) {
			perror("atexit(handle_middle) failed");
			return (1);
		}
	}
	if (atexit(handle_first)) {
		perror("atexit(handle_first) failed");
		return (1);
	}
	/* this is supposed to segfault */
	if (strcmp(argv[1], "-valid")) {
		signal(SIGSEGV, handle_signal);
		__atexit->fns[0] = handle_invalid;
	}
	__cleanup = handle_cleanup;
	counter = 0;
	fprintf(stderr, "main() returns\n");
	return (0);
}

void
handle_first()
{
	fprintf(stderr, "handle_first() counter == %i\n", counter);
}

void
handle_middle()
{
	counter++;
}

void
handle_last()
{
	fprintf(stderr, "handle_last() counter == %i\n", counter);
}

void
handle_cleanup()
{
	fprintf(stderr, "handle_cleanup()\n");
}

void
handle_invalid()
{
	fprintf(stderr, "handle_invalid() THIS SHOULD HAVE SEGFAULTED INSTEAD!\n");
}

void
handle_signal(int sigraised)
{
	switch (sigraised) {
	case SIGSEGV:
		fprintf(stderr, "SIGSEGV\n");
		exit(0);
	default:
		fprintf(stderr, "unexpected signal caught\n");
		exit(1);
	}
}