//
//  artworx.c
//  AnsiLove/C
//
//  Copyright (C) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "../ansilove.h"

void artworx(struct input *inputFile, struct output *outputFile)
{
	const unsigned char *font_data;
	unsigned char *font_data_adf;

	// libgd image pointers
	gdImagePtr canvas;

	// create ADF instance
	canvas = gdImageCreate(640, (((inputFile->size - 192 - 4096 -1) / 2) / 80) * 16);

	// error output
	if (!canvas) {
		perror("Can't allocate buffer image memory");
		exit(7);
	}

	// ADF color palette array
	uint32_t adf_colors[16] = { 0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63 };

	uint32_t loop;
	uint32_t index;

	// process ADF font
	font_data_adf = (unsigned char *)malloc(sizeof (unsigned char)*4096);
	if (font_data_adf == NULL) {
		perror("Memory error");
		exit(7);
	}
	memcpy(font_data_adf, inputFile->data+193, 4096);

	font_data = font_data_adf;

	// process ADF palette
	for (loop = 0; loop < 16; loop++)
	{
		index = (adf_colors[loop] * 3) + 1;
		gdImageColorAllocate(canvas, (inputFile->data[index] << 2 | inputFile->data[index] >> 4),
		    (inputFile->data[index + 1] << 2 | inputFile->data[index + 1] >> 4),
		    (inputFile->data[index + 2] << 2 | inputFile->data[index + 2] >> 4));
	}

	gdImageColorAllocate(canvas, 0, 0, 0);

	// process ADF
	uint32_t column = 0, row = 0;
	uint32_t character, attribute, foreground, background;
	loop = 192 + 4096 + 1;

	while (loop < inputFile->size)
	{
		if (column == 80)
		{
			column = 0;
			row++;
		}

		character = inputFile->data[loop];
		attribute = inputFile->data[loop+1];

		background = (attribute & 240) >> 4;
		foreground = attribute & 15;

		drawchar(canvas, font_data, 8, 16, column, row, background, foreground, character);

		column++;
		loop += 2;
	}

	// create output file
	output(canvas, outputFile->fileName, outputFile->retina, outputFile->retinaScaleFactor);

	// nuke garbage
	free(font_data_adf);
}
