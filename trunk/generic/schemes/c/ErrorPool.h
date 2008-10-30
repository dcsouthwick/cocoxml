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
#ifndef  COCO_ERRORPOOL_H
#define  COCO_ERRORPOOL_H

#ifndef   COCO_CDEFS_H
#include  "Defs.h"
#endif

EXTC_BEGIN

struct CcsErrorPool_s {
    FILE * fp;
    int    warningCount;
    int    errorCount;
};

CcsErrorPool_t * CcsErrorPool(CcsErrorPool_t * self, FILE * fp);
void CcsErrorPool_Destruct(CcsErrorPool_t * self);

void CcsErrorPool_Info(CcsErrorPool_t *, const char * format, ...);
void CcsErrorPool_Warning(CcsErrorPool_t * self, int line, int col,
			  const char * format, ...);
void CcsErrorPool_Error(CcsErrorPool_t * self, int line, int col,
			const char * format, ...);
void CcsErrorPool_Fatal(CcsErrorPool_t * self, int line, int col,
			const char * format, ...);

EXTC_END

#endif /* COCO_ERRORPOOL_H */
