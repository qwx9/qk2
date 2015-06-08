// input.h -- external (non-keyboard) input devices

extern cvar_t *in_joystick;
extern cvar_t *lookspring;
extern cvar_t *lookstrafe;
extern cvar_t *sensitivity;
extern cvar_t *freelook;
extern cvar_t *m_pitch;

void IN_Init (void);

void IN_Shutdown (void);

void IN_Commands (void);
// oportunity for devices to stick commands on the script buffer

void IN_Frame (void);

void IN_Move (usercmd_t *cmd);
// add additional movement on top of the keyboard move cmd

void IN_Activate (qboolean active);

void IN_Grabm(int on);
