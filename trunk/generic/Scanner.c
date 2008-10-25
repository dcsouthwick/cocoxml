/*---- open(Scanner.c) S ----*/
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
/*---- enable ----*/
#include  <stdlib.h>
#include  <string.h>
#include  "Scanner.h"

static int utf8get(char ** str);

Token_t *
Token(Token_t * self)
{
    if (!self && !(self = malloc(sizeof(Token_t)))) return NULL;
    self->kind = 0;
    self->pos = 0;
    self->col = 0;
    self->line = 0;
    self->val = NULL;
    self->next = NULL;
    return self;
}

void
Token_Destruct(Token_t * self)
{
    if (self->val) free(self->val);
}

typedef struct {
    int key, val;
}  Char2State_t;

static const Char2State_t c2sArr[] = {
    /*---- chars2states ----*/
    /*---- enable ----*/
};
static const int c2sNum = sizeof(c2sArr) / sizeof(c2sArr[0]);

static int
c2sCmp(const void * key, const void * c2s)
{
    return *((const int *)key) - ((const Char2State_t *)c2s)->key;
}
static int
Char2State(int chr)
{
    Char2State_t * c2s;

    c2s = bsearch(&chr, c2sArr, c2sNum, sizeof(Char2State_t), c2sCmp);
    return c2s ? c2s->val : 0;
}

typedef struct {
    const char * key;
    int val;
}  Identifier2KWKind_t;

static const Identifier2KWKind_t i2kArr[] = {
    /*---- identifiers2keywordkinds ----*/
    /*---- enable ----*/
};
static const int i2kNum = sizeof(i2kArr) / sizeof(i2kArr[0]);

static int
i2kCmp(const void * key, const void * i2k)
{
    return strcmp((const char *)key, ((const Identifier2KWKind_t *)i2k)->key);
}

static int
Identifier2KWKind(const char * key, int defaultVal)
{
    Identifier2KWKind_t * i2k;

    i2k = bsearch(key, i2kArr, i2kNum, sizeof(Identifier2KWKind_t), i2kCmp);
    return i2k ? i2k->val : defaultVal;
}

/* Now we support seekable file for simplicity. */
Buffer_t *
Buffer(Buffer_t * self, FILE * fp)
{
    long len;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (!(self->buf = malloc(len))) return NULL;
    self->cur = self->buf;
    self->last = self->buf + len;
    return self;
}

void
Buffer_Destruct(Buffer_t * self)
{
    free(self->buf);
}

int
Buffer_Read(Buffer_t * self)
{
    return self->cur < self->last ? utf8get(&self->cur) : EoF;
}

int
Buffer_Peek(Buffer_t * self)
{
    char * cur = self->cur;
    return cur < self->last ? utf8get(&cur) : EoF;
}

const char *
Buffer_GetString(Buffer_t * self, int start)
{
    return self->buf + start;
}

int
Buffer_GetPos(Buffer_t * self)
{
    return self->cur - self->buf;
}

void
Buffer_SetPos(Buffer_t * self, int pos)
{
    self->cur = self->buf + pos;
}

static int
utf8get(char ** str)
{
    int ch, c1, c2, c3, c4;
    char * cur = *str;
    ch = *cur++;
    if (ch >= 128 && ((ch & 0xC0) != 0xC0)) {
	fprintf(stderr, "Inside UTF-8 character!\n");
	exit(-1);
    }
    if (ch < 128) return ch;
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
