/*-------------------------------------------------------------------------
  Author (C) 2008, Charles Wang <charlesw123456@gmail.com>

  This program is free software; you can redistribute it and/or modify it 
  under the terms of the GNU General Public License as published by the 
  Free Software Foundation; either version 2, or (at your option) any 
  later version.

  This program is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
  for more details.

  You should have received a copy of the GNU General Public License along 
  with this program; if not, write to the Free Software Foundation, Inc., 
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

  As an exception, it is allowed to write an extension of Coco/R that is
  used as a plugin in non-free software.

  If not otherwise stated, any source code generated by Coco/R (other than 
  Coco/R itself) does not fall under the GNU General Public License.
-------------------------------------------------------------------------*/
#include  "CDefs.h"

void *
_CcsMalloc_(size_t size, const char * fname, int line)
{
    return malloc(size);
}

void *
_CcsRealloc_(void * ptr, size_t size, const char * fname, int line)
{
    return realloc(ptr, size);
}

void
_CcsFree_(void * ptr, const char * fname, int line)
{
    free(ptr);
}

char *
_CcsStrdup_(const char * str, const char * fname, int line)
{
    return strdup(str);
}

int
CcsUTF8GetCh(const char ** str)
{
    int ch, c1, c2, c3, c4;
    const char * cur = *str;
    ch = *cur++;
    if (ch >= 128 && ((ch & 0xC0) != 0xC0) && (ch != EoF)) {
	fprintf(stderr, "Inside UTF-8 character!\n");
	exit(-1);
    }
    if (ch < 128 || ch == EoF) return ch;
    if ((ch & 0xF0) == 0xF0) {
	/* 1110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
	c1 = ch & 0x07;
	ch = *cur++; if (ch == 0) goto broken;
	c2 = ch & 0x3F;
	ch = *cur++; if (ch == 0) goto broken;
	c3 = ch & 0x3F;
	ch = *cur++; if (ch == 0) goto broken;
	c4 = ch & 0x3F;
	*str = cur;
	return (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
    }
    if ((ch & 0xE0) == 0xE0) {
	/* 1110xxxx 10xxxxxx 10xxxxxx */
	c1 = ch & 0x0F;
	ch = *cur++; if (ch == 0) goto broken;
	c2 = ch & 0x3F;
	ch = *cur++; if (ch == 0) goto broken;
	c3 = ch & 0x3F;
	*str = cur;
	return (((c1 << 6) | c2) << 6) | c3;
    }
    /* (ch & 0xC0) == 0xC0 */
    /* 110xxxxx 10xxxxxx */
    c1 = ch & 0x1F;
    ch = *cur++; if (ch == 0) goto broken;
    c2 = ch & 0x3F;
    *str = cur;
    return (c1 << 6) | c2;
 broken:
    fprintf(stderr, "Broken in UTF8 character.\n");
    exit(-1);
}