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

# WebAssembly

A unified WebAssembly wrapper now lives in `npm/packages/libansilove`. Use the flake-backed
development shell so Emscripten and Bun are on `PATH`:

	nix develop --command bash -lc 'cd npm/packages/libansilove && bun run build'

The build performs a fresh `emcc` compile, emits TypeScript shims, and stages the publishable
artefacts under `npm/packages/libansilove/dist/`. Consumers load the module with:

```
import { load } from 'libansilove';

const lib = await load();
const { png } = lib.renderAnsi('Hello from libansilove!\r\n');
```

Overwrite the default `locateFile` if your bundler serves the `.wasm` payload from a custom URL:

```
await load({ locateFile: (path) => new URL(`/static/${path}`, window.location.href).toString() });
```

`bun run verify` executes a smoke test against the packaged output and ensures the PNG pipeline
still works. Use `npm pack` after a build to inspect the final publishing tarball.

# npm package

The `npm/packages/libansilove` workspace contains everything required for publishing. The
important commands:

```
bun install
bun run build
bun test
bun run verify
```

The build step regenerates the WebAssembly artefacts (stored in
`npm/packages/libansilove/generated/`) and copies them into `dist/` alongside the compiled
JavaScript and declaration files.


# Packages

libansilove packages are available for:

- [OpenBSD][1]
- [NetBSD][2]
- [FreeBSD][3]
- [Debian][4]
- [Ubuntu][5]
- [openSUSE][6]
- [Solus][7]
- [Gentoo][8]
- [Homebrew][9]
- [Guix][10]
- [Nix][11]

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

# Projects using libansilove

- [Ansilove][12] - ASCII / ANSI art to PNG converter, also supporting several artscene file formats
- [ANSi View (PS3)][13] - ANSi and artscene related file formats viewer for the PlayStation 3
- [Abydos][14] - Image loading library which handles both raster and vector graphics
- [PixelView][15] - An image viewer optimized for low-res pixel graphics

# Bindings

Bindings for other programming languages:

- [Racket][16]

# License

libansilove is released under the BSD 2-Clause license. See the file `LICENSE` for details.

# Authors

libansilove is developed by Stefan Vogt ([@ByteProject][17]), Brian Cassidy
([@bricas][18]) and Frederic Cambus ([@fcambus][19]).

# Resources

Project homepage: https://www.ansilove.org

GitHub: https://github.com/ansilove/libansilove

[1]: https://openports.pl/path/graphics/libansilove
[2]: https://pkgsrc.se/graphics/libansilove
[3]: https://www.freshports.org/graphics/libansilove/
[4]: https://packages.debian.org/search?keywords=libansilove
[5]: https://packages.ubuntu.com/search?keywords=libansilove
[6]: https://software.opensuse.org/package/libansilove
[7]: https://dev.getsol.us/source/libansilove/
[8]: https://packages.gentoo.org/packages/dev-libs/libansilove
[9]: https://formulae.brew.sh/formula/libansilove
[10]: https://packages.guix.gnu.org/packages/libansilove/
[11]: https://github.com/NixOS/nixpkgs/tree/master/pkgs/by-name/li/libansilove
[12]: https://github.com/ansilove/ansilove
[13]: https://github.com/bucanero/ansiview-ps3
[14]: https://snisurset.net/code/abydos/
[15]: https://github.com/kajott/PixelView
[16]: https://gitlab.com/xgqt/racket-libansilove
[17]: https://github.com/ByteProject
[18]: https://github.com/bricas
[19]: https://github.com/fcambus
