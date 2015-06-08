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
