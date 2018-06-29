//
//  icedraw.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "../ansilove.h"

void icedraw(struct input *inputFile, struct output *outputFile)
{
	const unsigned char *font_data;
	unsigned char *font_data_idf;

	// extract relevant part of the IDF header, 16-bit endian unsigned short
	int32_t x2 = (inputFile->data[9] << 8) + inputFile->data[8];

	// libgd image pointers
	gdImagePtr canvas;

	uint32_t loop = 12;
	int32_t index;
	int32_t colors[16];

	// process IDF font
	font_data_idf = (unsigned char *)malloc(4096);
	if (font_data_idf == NULL) {
		perror("Memory error");
		exit(7);
	}
	memcpy(font_data_idf, inputFile->data+(inputFile->size - 48 - 4096), 4096);

	font_data = font_data_idf;

	// process IDF
	uint32_t idf_sequence_length, idf_sequence_loop, i = 0;

	// dynamically allocated memory buffer for IDF data
	unsigned char *idf_buffer, *temp;
	idf_buffer = malloc(sizeof (unsigned char));

	int16_t idf_data, idf_data_length;

	while (loop < inputFile->size - 4096 - 48)
	{
		memcpy(&idf_data, inputFile->data+loop, 2);

		// RLE compressed data
		if (idf_data == 1)
		{
			memcpy(&idf_data_length, inputFile->data+loop+2, 2);

			idf_sequence_length = idf_data_length & 255;

			for (idf_sequence_loop = 0; idf_sequence_loop < idf_sequence_length; idf_sequence_loop++)
			{
				// reallocate IDF buffer memory
				temp = realloc(idf_buffer, i + 2);
				if (idf_buffer != NULL) {
					idf_buffer = temp;
				}
				else {
					perror("Error allocating IDF buffer memory");
					exit(7);
				}

				idf_buffer[i] = inputFile->data[loop + 4];
				idf_buffer[i+1] = inputFile->data[loop + 5];
				i += 2;
			}
			loop += 4;
		}
		else {
			// reallocate IDF buffer memory
			temp = realloc(idf_buffer, i + 2);
			if (idf_buffer != NULL) {
				idf_buffer = temp;
			}
			else {
				perror("Error allocating IDF buffer memory");
				exit(8);
			}

			// normal character
			idf_buffer[i] = inputFile->data[loop];
			idf_buffer[i+1] = inputFile->data[loop + 1];
			i += 2;
		}
		loop += 2;
	}

	// create IDF instance
	canvas = gdImageCreate((x2 + 1) * 8, i / 2 / 80 * 16);

	// error output
	if (!canvas) {
		perror("Can't allocate buffer image memory");
		exit(9);
	}
	gdImageColorAllocate(canvas, 0, 0, 0);

	// process IDF palette
	for (loop = 0; loop < 16; loop++)
	{
		index = (loop * 3) + inputFile->size - 48;
		colors[loop] = gdImageColorAllocate(canvas, (inputFile->data[index] << 2 | inputFile->data[index] >> 4),
		    (inputFile->data[index + 1] << 2 | inputFile->data[index + 1] >> 4),
		    (inputFile->data[index + 2] << 2 | inputFile->data[index + 2] >> 4));
	}

	// render IDF
	int32_t column = 0, row = 0;
	int32_t character, attribute, foreground, background;

	for (loop = 0; loop < i; loop += 2)
	{
		if (column == x2 + 1)
		{
			column = 0;
			row++;
		}

		character = idf_buffer[loop];
		attribute = idf_buffer[loop+1];

		background = (attribute & 240) >> 4;
		foreground = attribute & 15;

		drawchar(canvas, font_data, 8, 16, column, row, colors[background], colors[foreground], character);

		column++;
	}

	// create output file
	output(canvas, outputFile->fileName, outputFile->retina, outputFile->retinaScaleFactor);

	// free memory
	free(font_data_idf);
	free(idf_buffer);
}
