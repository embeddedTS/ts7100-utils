/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019, Technologic Systems Inc. */

/* Due to the nature of the parsing, the following conditions must be met:
 *
 *   Each token to evaluate must be in the style of " var=val " The var is a
 *     string, while value can be in hex (prefixed with 0x), octal (prefixed
 *     with a 0), or decimal (all other numbers). There must be a non-number
 *     immediately after the end of the val.
 *   Each token to evaluate must be present once and only once. Multiple tokens
 *     of the same name will return the FIRST value in the cmdline.
 *     e.g. "... var=0xaa ... var=0x55 ..." Will return "var" as 0xaa.
 */

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *cmd_fd = NULL;
static char *cmd_str = NULL;

/* As /proc/cmdline is not a "real" file, its not possible to get the size
 * unless the whole thing is read until EOF.
 * Open /proc/cmdline, read until EOF while discarding the contents, then malloc
 * some memory that is the length+1 of cmdline so there is a trailing NULL, then
 * read the file in to memory from the file, close the file.
 *
 * Even though the dummy read is byte at a time, it is fast since it is a
 * kernel pipe rather than real IO. This only needs to be done once at runtime,
 * after that the cmdline in memory can be parsed multiple times.
 */
void eval_cmd_init(void)
{
	size_t sz, ret;
	char buf;

	if (cmd_fd != NULL) {
		return;
	}

	cmd_fd = fopen("/proc/cmdline", "r");
	if ((int32_t)cmd_fd == -1) {
		error(errno, errno, "Failed to open /proc/cmdline");
	}

	for (sz = 0; ; sz++) {
		ret = fread(&buf, sizeof(char), 1, cmd_fd);
		if (ret == 0) break;
	}

	rewind(cmd_fd);
	
	cmd_str = (char *)calloc(sz+1, sizeof(char));
	if (cmd_str == NULL) {
		fclose(cmd_fd);
		error(errno, errno, "Failed to allocate memory");
	}

	fread(cmd_str, sizeof(char), sz, cmd_fd);
	fclose(cmd_fd);
}

/* Perform the actual evaluation of variable value similar to bash eval 
 * Note that token must be "var" and not "var="
 *
 * Returns -1 if the token was not found in the kernel cmdline
 * Otherwise returns value of the token
 */
int32_t eval_cmd(const char *token)
{
	char *ptr;
	int32_t ret = -1;	

	assert(cmd_str != NULL);

	ptr = strstr(cmd_str, token);
	if (ptr != NULL) {
		/* strstr points us to start of token, add length of token +1 to
		 * now point at the start of value, after the '=' sign.
		 */
		ptr += (strlen(token) + 1);
		ret = (int32_t)strtoul(ptr, NULL, 0);
	}

	return ret;
}
