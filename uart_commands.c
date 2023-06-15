#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "fsl_debug_console.h"

#define words_max_count 32
const char delimiter[] = " ";

static char *words[words_max_count];
static int words_count = 0;
static int parameters[words_max_count];
static int parameters_count = 0;

// print debug info about string
void ds(char *s, int max_string)
{
	int i;
	for (i = 0; i < max_string; i++)
	{
		if (s[i] == 0)
		{
			PRINTF("[NULL](%d)\r\n", i);
			return;
		}
		PRINTF("%c", s[i]);
	}
	PRINTF("[-](%d)\r\n", i);
}

static void remove_last_r_or_n(char *word)
{
	int length = strlen(word);
	if (length > 0 && (word[length - 1] == '\r' || word[length - 1] == '\n'))
	{
		word[length - 1] = 0;
	}
}

static void remove_last_rn(char *word)
{
	remove_last_r_or_n(word);
	remove_last_r_or_n(word);
}

static bool parse_parameters(char *words[], int words_count)
{
	for (int i = 1; i < words_count; i++)
	{
		char *end;
		long l = strtol(words[i], &end, 10);
		if (end == words[i])
		{
			return false;
		}
		if (!(l >= INT_MIN && l <= INT_MAX))
		{
			return false;
		}
		parameters[parameters_count++] = (int)l;
	}

	return true;
}

static void execute_commands(const char *command, char *result)
{
	int command_length = strlen(command);
	if (command_length <= 0)
	{
		strcpy(result, "error");
		return;
	}

	if (strcmp("add", command) == 0)
	{
		int value = 0;
		for (int i = 0; i < parameters_count; i++)
		{
			value += parameters[i];
		}

		sprintf(result, "%d", value);
		return;
	}

	if (strcmp("subtract", command) == 0)
	{
		int value = 0;
		if (parameters_count >= 1)
		{
			value = parameters[0];
		}
		for (int i = 1; i < parameters_count; i++)
		{
			value -= parameters[i];
		}

		sprintf(result, "%d", value);
		return;
	}

	strcpy(result, "error");
}

static void free_memory(void)
{
	for (int i = 0; i < words_count; i++)
	{
		free(words[i]);
	}
}

bool parse_line(char *line, char *result)
{
	words_count = 0;
	parameters_count = 0;

	char *word = strtok(line, delimiter);
	while (word != NULL)
	{
		remove_last_rn(word);
		// ds(word, 100);
		if (words_count + 1 >= words_max_count)
		{
			free_memory();
			strcpy(result, "error");
			return false;
		}

		char *words_ = (char*)calloc(1, strlen(word) + 1);
		if (words_ == NULL)
		{
			free_memory();
			strcpy(result, "error");
			return false;
		}

		strncpy(words_, word, strlen(word));
		words[words_count++] = words_;

		word = strtok(NULL, delimiter);
	}

	if (words_count == 0 || !parse_parameters(words, words_count))
	{
		free_memory();
		strcpy(result, "error");
		return false;
	}

	// for (int i = 0; i < parameters_count; i++)
	// 	printf("%d\r\n", parameters[i]);

	execute_commands(words[0], result);
	// ds(result, 100);

	free_memory();

	return true;
}
