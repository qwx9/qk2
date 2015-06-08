void S_Init (void);
void S_Shutdown (void);

// if origin is NULL, the sound will be dynamically sourced from the entity
void S_StartSound (vec3_t origin, int entnum, int entchannel, sfx_t *sfx, float fvol,  float attenuation, float timeofs);
void S_StartLocalSound (char *s);

void S_RawSamples (int samples, int rate, int width, int channels, byte *data);

void S_StopAllSounds(void);
void S_Update (vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up);

void S_Activate (qboolean active);

void S_BeginRegistration (void);
sfx_t *S_RegisterSound (char *sample);
void S_EndRegistration (void);

// the sound code makes callbacks to the client for entitiy position
// information, so entities can be dynamically re-spatialized
void CL_GetEntitySoundOrigin (int ent, vec3_t org);
