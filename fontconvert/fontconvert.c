/*
ProffieOS font converter, based on:
TrueType to Adafruit_GFX font converter.  Derived from Peter Jakobs'
Adafruit_ftGFX fork & makefont tool, and Paul Kourany's Adafruit_mfGFX.

NOT AN ARDUINO SKETCH.  This is a command-line tool for preprocessing
fonts to be used with the Adafruit_GFX Arduino library.

For UNIX-like systems.  Outputs to stdout; redirect to header file, e.g.:
  ./fontconvert ~/Library/Fonts/FreeSans.ttf 18 > FreeSans18pt7b.h

REQUIRES FREETYPE LIBRARY.  www.freetype.org

Currently this only extracts the printable 7-bit ASCII chars of a font.
Will eventually extend with some int'l chars a la ftGFX, not there yet.
Keep 7-bit fonts around as an option in that case, more compact.

*/

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_GLYPH_H

typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;

#define DPI 141 // Approximate res. of Adafruit 2.8" TFT

int main(int argc, char *argv[]) {
  int                i, j, err, size, first=' ', last='~',
    bitmapOffset = 0, x, y, byte;
  char              *fontName, c, *ptr;
  FT_Library         library;
  FT_Face            face;
  FT_Glyph           glyph;
  FT_Bitmap         *bitmap;
  FT_BitmapGlyphRec *g;
  GFXglyph          *table;
  uint8_t            bit;
  
  // Parse command line.  Valid syntaxes are:
  //   fontconvert [filename] [size]
  //   fontconvert [filename] [size] [last char]
  //   fontconvert [filename] [size] [first char] [last char]
  // Unless overridden, default first and last chars are
  // ' ' (space) and '~', respectively
  
  if(argc < 3) {
    fprintf(stderr, "Usage: %s fontfile size [first] [last]\n",
	    argv[0]);
    return 1;
  }
  
  size = atoi(argv[2]);
  
  if(argc == 4) {
    last  = atoi(argv[3]);
  } else if(argc == 5) {
    first = atoi(argv[3]);
    last  = atoi(argv[4]);
  }
  
  if(last < first) {
    i     = first;
    first = last;
    last  = i;
  }
  
  ptr = strrchr(argv[1], '/'); // Find last slash in filename
  if(ptr) ptr++;         // First character of filename (path stripped)
  else    ptr = argv[1]; // No path; font in local dir.
  
  // Allocate space for font name and glyph table
  if((!(fontName = malloc(strlen(ptr) + 20))) ||
     (!(table = (GFXglyph *)malloc((last - first + 1) *
				   sizeof(GFXglyph))))) {
    fprintf(stderr, "Malloc error\n");
    return 1;
  }
  
  // Derive font table names from filename.  Period (filename
  // extension) is truncated and replaced with the font size & bits.
  strcpy(fontName, ptr);
  ptr = strrchr(fontName, '.'); // Find last period (file ext)
  if(!ptr) ptr = &fontName[strlen(fontName)]; // If none, append
  // Insert font size and 7/8 bit.  fontName was alloc'd w/extra
  // space to allow this, we're not sprintfing into Forbidden Zone.
  sprintf(ptr, "%dpt%db", size, (last > 127) ? 8 : 7);
  // Space and punctuation chars in name replaced w/ underscores.  
  for(i=0; (c=fontName[i]); i++) {
    if(isspace(c) || ispunct(c)) fontName[i] = '_';
  }
  
  // Init FreeType lib, load font
  if((err = FT_Init_FreeType(&library))) {
    fprintf(stderr, "FreeType init error: %d", err);
    return err;
  }
  if((err = FT_New_Face(library, argv[1], 0, &face))) {
    fprintf(stderr, "Font load error: %d", err);
    FT_Done_FreeType(library);
    return err;
  }
  
  // << 6 because '26dot6' fixed-point format
  FT_Set_Char_Size(face, size << 6, 0, DPI, 0);
  
  // Currently all symbols from 'first' to 'last' are processed.
  // Fonts may contain WAY more glyphs than that, but this code
  // will need to handle encoding stuff to deal with extracting
  // the right symbols, and that's not done yet.
  // fprintf(stderr, "%ld glyphs\n", face->num_glyphs);
  
  // Process glyphs and output glyph arrays.
  int bytes_used = 0;
  for(i=first, j=0; i<=last; i++, j++) {
    // MONO renderer provides clean image with perfect crop
    // (no wasted pixels) via bitmap struct.
    if((err = FT_Load_Char(face, i, FT_LOAD_TARGET_MONO))) {
      fprintf(stderr, "Error %d loading char '%c'\n",
	      err, i);
      continue;
    }
    
    if((err = FT_Render_Glyph(face->glyph,
			      FT_RENDER_MODE_MONO))) {
      fprintf(stderr, "Error %d rendering char '%c'\n",
	      err, i);
      continue;
    }
    
    if((err = FT_Get_Glyph(face->glyph, &glyph))) {
      fprintf(stderr, "Error %d getting glyph '%c'\n",
	      err, i);
      continue;
    }
    
    bitmap = &face->glyph->bitmap;
    g      = (FT_BitmapGlyphRec *)glyph;
    
    // Glyps are stored as 8/16/32-bit arrays to save space.
    table[j].bitmapOffset = bitmapOffset;
    table[j].width        = bitmap->width;
    table[j].height       = bitmap->rows;
    table[j].xAdvance     = face->glyph->advance.x >> 6;
    table[j].xOffset      = g->left;
    table[j].yOffset      = 1 - g->top;

    int type_width = (bitmap->rows + 7) / 8;
    if (type_width == 0) type_width = 1;
    if (type_width == 3) type_width = 4;
    printf("const uint%d_t %sChar%d[] = {\n", type_width * 8, fontName, j);
    for(x=0;x < bitmap->width; x++) {
      bytes_used += type_width;
      printf("  0b");
      for(y=bitmap->rows-1; y>=0; y--) {
	byte = x / 8;
	bit  = 0x80 >> (x & 7);
	if (bitmap->buffer[y * bitmap->pitch + byte] & bit) {
          printf("1");
        } else {
          printf("0");
        }
      }
      printf("UL,\n");
    }
    printf("};\n");
    
    bitmapOffset += (bitmap->width * bitmap->rows + 7) / 8;
    
    FT_Done_Glyph(glyph);
  }

  fprintf(stderr, "  %d bytes used for bitmaps.\n", bytes_used);
  
  // Output glyph attributes table (one per character)
  printf("const Glyph %sGlyphs[] = {\n", fontName);
  for(i=first, j=0; i<=last; i++, j++) {
    printf("  { %3d, %4d, %4d, GLYPHDATA(%sChar%d) }",
	   table[j].xAdvance,
	   table[j].xOffset,
	   table[j].yOffset,
	   fontName,
	   j);
    if(i < last) {
      printf(",   // 0x%02X", i);
      if((i >= ' ') && (i <= '~')) {
	printf(" '%c'", i);
      }
      putchar('\n');
    }
  }
  printf(" }; // 0x%02X", last);
  if((last >= ' ') && (last <= '~')) printf(" '%c'", last);
  printf("\n\n");
  
  FT_Done_FreeType(library);

  return 0;
}

