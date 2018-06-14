//
//  binary.c
//  AnsiLove/C
//
//  Copyright (C) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "../ansilove.h"

void binary(struct input *inputFile, struct output *outputFile)
{
	// binary files must have an even size
	if (inputFile->size % 2) {
		fprintf(stderr, "\nBinary file is not valid.\n");
		exit(1);
	}

	// some type declarations
	struct fontStruct fontData;

	// font selection
	alSelectFont(&fontData, outputFile->font);

	// libgd image pointers
	gdImagePtr canvas;

	// allocate buffer image memory
	canvas = gdImageCreate(inputFile->columns * outputFile->bits,
	    ((inputFile->size / 2) / inputFile->columns * fontData.height));

	if (!canvas) {
		perror("Error, can't allocate buffer image memory");
		exit(6);
	}

	// allocate black color
	gdImageColorAllocate(canvas, 0, 0, 0);

	// allocate color palette
	int32_t colors[16];

	for (int i = 0; i < 16; i++) {
		colors[i] = gdImageColorAllocate(canvas, binary_palette[i*3],
		    binary_palette[i*3+1],
		    binary_palette[i*3+2]);
	}

	// process binary
	uint32_t character, attribute, background, foreground;
	uint32_t loop = 0, column = 0, row = 0;

	while (loop < inputFile->size)
	{
		if (column == inputFile->columns)
		{
			column = 0;
			row++;
		}

		character = inputFile->data[loop];
		attribute = inputFile->data[loop+1];

		background = (attribute & 240) >> 4;
		foreground = (attribute & 15);

		if (background > 8 && !outputFile->icecolors)
			background -= 8;

		drawchar(canvas, fontData.font_data, outputFile->bits, fontData.height,
		    column, row, colors[background], colors[foreground], character);

		column++;
		loop += 2;
	}

	// create output image
	output(canvas, outputFile->fileName, outputFile->retina, outputFile->retinaScaleFactor);
}
