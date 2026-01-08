// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Kent Gibson <warthog618@gmail.com>

#include <errno.h>
#include <gpiod.h>
#include <stdio.h>

/* The following helper functions were pulled mostly verbatim from the libgpiod
 * examples. More information on each function is included in its header.
 */

/* Request a single line as an input.
 * Returns struct gpiod_line_request* on success or NULL on failure.
 * Unfortunately most libgpiod functions don't set errno on failure.
 *
 * Pulled from the v2.2.x branch:
 * https://github.com/brgl/libgpiod/blob/v2.2.x/examples/get_line_value.c
 */
struct gpiod_line_request *request_input_line(const char *chip_path,
					      unsigned int offset,
					      const char *consumer)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
						  settings);
	if (ret)
		goto free_line_config;

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}


/* Request a single line as an output.
 * Returns struct gpiod_line_request* on success or NULL on failure.
 * Unfortunately most libgpiod functions don't set errno on failure.
 *
 * Pulled from the v2.2.x branch:
 * https://github.com/brgl/libgpiod/blob/v2.2.x/examples/toggle_line_value.c
 */
struct gpiod_line_request *
request_output_line(const char *chip_path, unsigned int offset,
		    enum gpiod_line_value value, const char *consumer)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings,
					  GPIOD_LINE_DIRECTION_OUTPUT);
	gpiod_line_settings_set_output_value(settings, value);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
						  settings);
	if (ret)
		goto free_line_config;

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}


/* Request a single line as a rising edge event
 * Returns struct gpiod_line_request* on success or NULL on failure.
 * Unfortunately most libgpiod functions don't set errno on failure.
 *
 * Pulled from the v2.2.x branch:
 * https://github.com/brgl/libgpiod/blob/v2.2.x/examples/watch_line_rising.c
 */
struct gpiod_line_request *request_input_line_rising(const char *chip_path,
						     unsigned int offset,
						     const char *consumer)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
	gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_RISING);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
						  settings);
	if (ret)
		goto free_line_config;

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}

/* Request multiple lines as output
 * Returns struct gpiod_line_request* on success or NULL on failure.
 * Unfortunately most libgpiod functions don't set errno on failure.
 *
 * Pulled from the v2.2.x branch:
 * https://github.com/brgl/libgpiod/blob/v2.2.x/examples/toggle_multiple_line_values.c
 */
struct gpiod_line_request *
request_output_lines(const char *chip_path, const unsigned int *offsets,
		     enum gpiod_line_value *values, unsigned int num_lines,
		     const char *consumer)
{
	struct gpiod_request_config *rconfig = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *lconfig;
	struct gpiod_chip *chip;
	unsigned int i;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings,
					  GPIOD_LINE_DIRECTION_OUTPUT);

	lconfig = gpiod_line_config_new();
	if (!lconfig)
		goto free_settings;

	for (i = 0; i < num_lines; i++) {
		ret = gpiod_line_config_add_line_settings(lconfig, &offsets[i],
							  1, settings);
		if (ret)
			goto free_line_config;
	}
	gpiod_line_config_set_output_values(lconfig, values, num_lines);

	if (consumer) {
		rconfig = gpiod_request_config_new();
		if (!rconfig)
			goto free_line_config;

		gpiod_request_config_set_consumer(rconfig, consumer);
	}

	request = gpiod_chip_request_lines(chip, rconfig, lconfig);

	gpiod_request_config_free(rconfig);

free_line_config:
	gpiod_line_config_free(lconfig);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}

/* Request multiple lines as input
 * Returns struct gpiod_line_request* on success or NULL on failure.
 * Unfortunately most libgpiod functions don't set errno on failure.
 *
 * Pulled from the v2.2.x branch:
 * https://github.com/brgl/libgpiod/blob/v2.2.x/examples/get_multiple_line_values.c
 */
struct gpiod_line_request *
request_input_lines(const char *chip_path, const unsigned int *offsets,
		    unsigned int num_lines, const char *consumer)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	unsigned int i;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	for (i = 0; i < num_lines; i++) {
		ret = gpiod_line_config_add_line_settings(line_cfg, &offsets[i],
							  1, settings);
		if (ret)
			goto free_line_config;
	}

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}

