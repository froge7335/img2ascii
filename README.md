# img2ascii

Command-line tool that converts raster images to ASCII or braille text on stdout

## Build

Requires g++ 15 or newer, produces `converter` (`converter.exe` on Windows) and a `gcm.cache/` directory
From the project directory:

```
g++ -std=c++23 -O2 -fmodules -fsearch-include-path bits/std.cc charsets.cpp converter.cpp decoders.cpp -o converter -lstdc++exp
```

## Usage

```
.\converter <flags(optional)> <standard|braille|custom "<ramp>"> <low|med|high|xhigh> <image>
```

| Argument | Values |
|---|---|
| Ramp | `standard` = `" .:-=+*#%@"`, `braille` = `⠀⠁⠃⠇⡇⣇⣧⣷⣿`, or `custom "<ramp>"` with any UTF-8 string; glyphs go empty to full |
| Resolution | `low` 40, `med` 80, `high` 160, `xhigh` 240 columns, capped at the image width |

Rows keep the aspect ratio, halved because terminal cells are about twice as tall as they are wide. Brighter pixels map to fuller glyphs (made for dark terminals), transparent pixels map to the first glyph. Errors print a message and the usage line to stderr and exit with code 1

The --invert flag is optional and will invert the glyphs used. Transparent pixels will still map to the first glyph however. 

The --colour (or --color, for the americans) flag is optional and prints each glyph in the average colour of its pixels using 24-bit ANSI codes. --invert changes only the glyphs, not the colours. Needs a terminal with 24-bit colour support.

```
.\converter standard med photo.jpg
.\converter braille high logo.png
.\converter custom " .oO@" low cat.webp
.\converter standard xhigh pic.png > out.txt
.\converter --invert braille med example.jpg
.\converter --colour standard med photo.jpg
.\converter --invert --color braille high logo.png > out.txt
```

## Formats

- JPG, PNG, GIF, BMP, and more via stb_image
- WebP via simplewebp
- The format is detected from file contents, not the extension

## Limitations / Coming soon

- Animated GIFs: only the first frame is used. Animated WebP is not supported
- Vector formats (SVG) are not supported yet

## Third-party

- `third_party/stb_image.h` v2.30: MIT or public domain, https://github.com/nothings/stb
- `third_party/simplewebp.h`: BSD-3-Clause, https://github.com/MikuAuahDark/simplewebp
