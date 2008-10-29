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
#include  "ArrayList.h"

ArrayList_t *
ArrayList(ArrayList_t * self)
{
    self = AllocObject(self, sizeof(ArrayList_t));
    self->Count = 0;
    self->Capacity = 16;
    self->Data = CocoMalloc(sizeof(void *) * self->Capacity);
    return self;
}

void
ArrayList_Destruct(ArrayList_t * self)
{
    ArrayList_Clear(self);
    if (self->Data)  CocoFree(self->Data);
}

void
ArrayList_Add(ArrayList_t * self, void * value)
{
    int newCapacity;
    if (self->Count >= self->Capacity) {
	newCapacity = self->Capacity + self->Capacity;
	self->Data = CocoRealloc(self->Data, sizeof(void *) * newCapacity);
	self->Capacity = newCapacity;
    }
    self->Data[self->Count++] = value;
}

void
ArrayList_Remove(ArrayList_t * self, void * value)
{
    void ** cur;
    for (cur = self->Data; cur - self->Data < self->Count; ++cur)
	if (*cur == value) {
	    memmove(cur, cur + 1,
		    sizeof(void *) * (self->Count - (cur + 1 - self->Data)));
	    --self->Count;
	    break;
	}
}

void *
ArrayList_Get(ArrayList_t * self, int index)
{
    return (index >= 0 && index < self->Count) ? self->Data[index] : NULL;
}

void
ArrayList_Clear(ArrayList_t * self)
{
}
