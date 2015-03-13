/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// vid.h -- video driver defs

typedef uchar pixel_t;

typedef struct vrect_t vrect_t;
typedef struct viddef_t viddef_t;

struct vrect_t
{
	int	x, y, width, height;
	vrect_t	*pnext;
};

struct viddef_t
{
	int	width;          
	int	height;
	pixel_t	*buffer;                // invisible buffer
	pixel_t	*colormap;              // 256 * VID_GRADES size
	pixel_t	*alphamap;              // 256 * 256 translucency map
	int	rowbytes;               // may be > width if displayed in a window
					// can be negative for stupid dibs
};
extern viddef_t vid;

// Video module initialisation etc
void	VID_Init (void);
void	VID_Shutdown (void);
void	VID_CheckChanges (void);

void	VID_MenuInit( void );
void	VID_MenuDraw( void );
const char *VID_MenuKey( int );
