//
//  tundra.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "../ansilove.h"

int tundra(struct input *inputFile, struct output *outputFile)
{
	// some type declarations
	struct fontStruct fontData;
	char tundra_version;
	char tundra_header[8];

	// font selection
	alSelectFont(&fontData, outputFile->font);

	// libgd image pointers
	gdImagePtr canvas;

	// extract tundra header
	tundra_version = inputFile->data[0];
	memcpy(&tundra_header, inputFile->data+1, 8);

	// need to add check for "TUNDRA24" string in the header
	if (tundra_version != 24) {
		fputs("\nInput file is not a TUNDRA file.\n\n", stderr);
		exit(4);
	}

	// read tundra file a first time to find the image size
	uint32_t character, background = 0, foreground = 0;
	uint32_t loop = 9, column = 0, row = 1;

	while (loop < inputFile->size)
	{
		if (column == 80)
		{
			column = 0;
			row++;
		}

		character = inputFile->data[loop];

		if (character == 1)
		{
			row =
			    (inputFile->data[loop + 1] << 24) + (inputFile->data[loop + 2] << 16) +
			    (inputFile->data[loop + 3] << 8) + inputFile->data[loop+4];

			column =
			    (inputFile->data[loop + 5] << 24) + (inputFile->data[loop + 6] << 16) +
			    (inputFile->data[loop + 7] << 8) + inputFile->data[loop+8];

			loop += 8;
		}

		if (character == 2)
		{
			character = inputFile->data[loop + 1];

			loop += 5;
		}

		if (character == 4)
		{
			character = inputFile->data[loop + 1];

			loop += 5;
		}

		if (character == 6)
		{
			character = inputFile->data[loop + 1];

			loop += 9;
		}

		if (character != 1 && character != 2 && character != 4 && character != 6)
			column++;

		loop++;
	}

	// allocate buffer image memory
	canvas = gdImageCreateTrueColor(80 * outputFile->bits, (row) * fontData.height);

	if (!canvas) {
		perror("Error, can't allocate buffer image memory");
		exit(6);
	}

	// process tundra
	column = 0;
	row = 0;

	loop = 9;

	while (loop < inputFile->size)
	{
		if (column == 80)
		{
			column = 0;
			row++;
		}

		character = inputFile->data[loop];

		if (character == 1)
		{
			row =
			    (inputFile->data[loop + 1] << 24) + (inputFile->data[loop + 2] << 16) +
			    (inputFile->data[loop + 3] << 8) + inputFile->data[loop + 4];

			column =
			    (inputFile->data[loop + 5] << 24) + (inputFile->data[loop + 6] << 16) +
			    (inputFile->data[loop + 7] << 8) + inputFile->data[loop + 8];

			loop += 8;
		}

		if (character == 2)
		{
			foreground =
			    (inputFile->data[loop + 3] << 16) + (inputFile->data[loop + 4] << 8) +
			    inputFile->data[loop + 5];

			character = inputFile->data[loop+1];

			loop += 5;
		}

		if (character == 4)
		{
			background = (inputFile->data[loop + 3] << 16) + (inputFile->data[loop + 4] << 8) +
			    inputFile->data[loop+5];

			character = inputFile->data[loop+1];

			loop += 5;
		}

		if (character == 6)
		{
			foreground =
			    (inputFile->data[loop + 3] << 16) + (inputFile->data[loop + 4] << 8) +
			    inputFile->data[loop+5];

			background =
			    (inputFile->data[loop + 7] << 16) + (inputFile->data[loop + 8] << 8) +
			    inputFile->data[loop+9];

			character = inputFile->data[loop+1];

			loop += 9;
		}

		if (character != 1 && character != 2 && character != 4 && character != 6)
		{
			drawchar(canvas, fontData.font_data, outputFile->bits, fontData.height,
			    column, row, background, foreground, character);

			column++;
		}

		loop++;
	}

	// create output image
	output(canvas, outputFile->fileName, outputFile->retina, outputFile->retinaScaleFactor);

	return 0;
}

