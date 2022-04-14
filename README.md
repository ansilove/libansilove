# libansilove

libansilove is a library to convert ANSi and artscene related file formats
into PNG images.

# Dependencies

libansilove uses the `CMake` build system and requires the `GD` library and
header files.

# Installing dependencies

- OpenBSD: `pkg_add -r cmake gd`
- NetBSD: `pkgin install cmake gd`
- FreeBSD: `pkg install cmake gd`
- macOS: `brew install cmake gd`
- Alpine Linux: `apk add cmake gcc make musl-dev gd-dev`
- Debian / Ubuntu / Mint: `apt-get install build-essential cmake libgd-dev`
- Fedora: `dnf install cmake gcc make gd-devel`
- Solus: `eopkg install -c system.devel` and `eopkg install gd-devel`

# Compiling

	mkdir build
	cd build
	cmake ..
	make

# Installation

	make install

# Packages

Packages are available for the following operating systems:

- [OpenBSD][1]
- [NetBSD][2]
- [FreeBSD][3]
- [Debian][4]
- [Ubuntu][5]
- [openSUSE][6]
- [Solus][7]
- [Gentoo][8]

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

# Bindings

Bindings for other programming languages:

- [Racket][12]

# License

libansilove is released under the BSD 2-Clause license. See the file `LICENSE` for details.

# Authors

libansilove is developed by Stefan Vogt ([@ByteProject][9]), Brian Cassidy
([@bricas][10]) and Frederic Cambus ([@fcambus][11]).

# Resources

Project homepage: https://www.ansilove.org

GitHub: https://github.com/ansilove/libansilove

[1]: https://cvsweb.openbsd.org/cgi-bin/cvsweb/ports/graphics/libansilove
[2]: https://pkgsrc.se/graphics/libansilove
[3]: https://www.freshports.org/graphics/libansilove/
[4]: https://packages.debian.org/search?keywords=libansilove
[5]: https://packages.ubuntu.com/search?keywords=libansilove
[6]: https://software.opensuse.org/package/libansilove
[7]: https://dev.getsol.us/source/libansilove/
[8]: https://packages.gentoo.org/packages/dev-libs/libansilove
[9]: https://github.com/ByteProject
[10]: https://github.com/bricas
[11]: https://github.com/fcambus
[12]: https://gitlab.com/xgqt/racket-libansilove
