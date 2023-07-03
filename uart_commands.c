#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "fsl_debug_console.h"
#include "common.h"
#include "i2c_task.h"

#define words_max_count 32
const char delimiter[] = " ";

static char *words[words_max_count];
static int words_count = 0;
static int parameters[words_max_count];
static int parameters_count = 0;

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

	if (strcmp("leds", command) == 0)
	{
		if (parameters_count != 3)
		{
			strcpy(result, "error");
			return;
		}

		led_r = parameters[0];
		led_g = parameters[1];
		led_b = parameters[2];
		led_mode = LED_MODE_MANUAL;
		sprintf(result, "%d %d %d", parameters[0], parameters[1], parameters[2]);
		return;
	}

	if (strcmp("leds_mode", command) == 0)
	{
		if (parameters_count != 1)
		{
			strcpy(result, "error");
			return;
		}

		led_mode = (enum led_mode_t)parameters[0];
		sprintf(result, "auto");
		return;
	}

	if (strcmp("leds_status", command) == 0)
	{
		if (parameters_count != 0)
		{
			strcpy(result, "error");
			return;
		}

		sprintf(result, "leds_status %d %d %d %d", led_r, led_g, led_b, 
				(led_mode == LED_MODE_AUTO) ? 1 : 0);
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
		// debug_string(word, 100);
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
	// debug_string(result, 100);

	free_memory();

	return true;
}
