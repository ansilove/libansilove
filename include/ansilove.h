//
//  ansilove.h
//  AnsiLove/C
//
//  Copyright (C) 2011-2017 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#ifndef ansilove_h
#define ansilove_h

void ansi(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, char *font, int32_t bits, char *mode, bool icecolors, char *fext, int retinaScaleFactor);
void artworx(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, int retinaScaleFactor);
void binary(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, int32_t columns, char *font, int32_t bits, bool icecolors, int retinaScaleFactor);
void icedraw(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, int retinaScaleFactor);
void pcboard(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, char *font, int32_t bits, int retinaScaleFactor);
void tundra(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, char *font, int32_t bits, int retinaScaleFactor);
void xbin(unsigned char *inputFileBuffer, int32_t inputFileSize, char *outputFile, char *retinaout, int retinaScaleFactor);

#endif
