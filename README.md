# libansilove [![Build Status][1]][2]

libansilove is a library to converts ANSi and artscene related file formats
into PNG images.

# Dependencies

libansilove uses the `CMake` build system and requires the `GD` library and
header files.

# Installing dependencies

- OpenBSD: `pkg_add -r cmake gd`
- NetBSD: `pkgin install cmake gd`
- FreeBSD: `pkg install cmake gd`
- Mac OS X: `brew install cmake gd`
- Alpine Linux: `apk add cmake gcc make musl-dev gd-dev`
- Debian / Ubuntu / Mint: `apt-get install cmake libgd-dev`
- Solus: `eopkg install -c system.devel` and `eopkg install gd-devel`

# Compiling

    cmake .
    make

# Installation

    make install

# Features

The following formats are supported:

- .ANS - ANSi (ANSI escape sequences: ANSI X3.64 standard)
- .PCB - PCBoard Bulletin Board System (BBS) own file format
- .BIN - Binary format (raw memory copy of text mode video memory)
- .ADF - Artworx format, supporting custom character sets and palettes
- .IDF - iCE Draw format, supporting custom character sets and palettes
- .TND - TundraDraw format, supporting 24-bit color mode
- .XB - The eXtended Binary XBin format, supporting custom character sets and palettes

# Documentation

# Usage

See the `example` directory for an example showing how to use the library to
convert a file to PNG.

# Who pulls the strings

libansilove is developed by Stefan Vogt ([@ByteProject](https://github.com/ByteProject)), Brian Cassidy ([@bricas](https://github.com/bricas)) and Frederic Cambus ([@fcambus](https://github.com/fcambus)).

# License

libansilove is released under the BSD 2-Clause License. See the file `LICENSE` for details.

# Resources

GitHub: https://github.com/ansilove/libansilove

[1]: https://api.travis-ci.org/ansilove/libansilove.png?branch=master
[2]: https://travis-ci.org/ansilove/libansilove
