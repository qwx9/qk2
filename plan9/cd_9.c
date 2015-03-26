#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "../q_shared.h"

qboolean cdValid = false;
qboolean playing = false;
qboolean wasPlaying = false;
qboolean initialized = false;
qboolean enabled = true;
qboolean playLooping = false;
float cdvolume;
byte remap[100];
byte playTrack;
byte maxTrack;
int cdfile = -1;

cvar_t	*cd_volume;
cvar_t *cd_nocd;
cvar_t *cd_dev;


void CDAudio_Eject(void)
{
	if (cdfile == -1 || !enabled)
		return;

	Com_DPrintf("CDAudio_Eject: PORTME\n");
	/*
	if ( ioctl(cdfile, CDROMEJECT) == -1 ) 
		Com_DPrintf("ioctl cdromeject failed\n");
	*/
}

void CDAudio_CloseDoor(void)
{
	if (cdfile == -1 || !enabled)
		return;

	Com_DPrintf("CDAudio_CloseDoor: PORTME\n");
	/*
	if ( ioctl(cdfile, CDROMCLOSETRAY) == -1 ) 
		Com_DPrintf("ioctl cdromclosetray failed\n");
	*/
}

int CDAudio_GetAudioDiskInfo(void)
{
	cdValid = false;
	Com_DPrintf("CDAudio_GetAudioDiskInfo: PORTME\n");
	return -1;

	/*
	struct cdrom_tochdr tochdr;

	if ( ioctl(cdfile, CDROMREADTOCHDR, &tochdr) == -1 ) 
	{
		Com_DPrintf("ioctl cdromreadtochdr failed\n");
		return -1;
	}

	if (tochdr.cdth_trk0 < 1)
	{
		Com_DPrintf("CDAudio: no music tracks\n");
		return -1;
	}

	cdValid = true;
	maxTrack = tochdr.cdth_trk1;
	return 0;
	*/
}

void CDAudio_Pause(void)
{
	if (cdfile == -1 || !enabled)
		return;
	if (!playing)
		return;

	Com_DPrintf("CDAudio_GetAudioDiskInfo: PORTME\n");

	/*
	if ( ioctl(cdfile, CDROMPAUSE) == -1 ) 
		Com_DPrintf("ioctl cdrompause failed\n");

	wasPlaying = playing;
	playing = false;
	*/
}

void CDAudio_Play(int track, qboolean looping)
{
	if (cdfile == -1 || !enabled)
		return;
	if (!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if (!cdValid)
			return;
	}

	track = remap[track];
	if (track < 1 || track > maxTrack)
	{
		Com_DPrintf("CDAudio: Bad track number %u.\n", track);
		return;
	}

	USED(looping);
	Com_DPrintf("CDAudio_Play: PORTME\n");

	/*
	struct cdrom_tocentry entry;
	struct cdrom_ti ti;

	// don't try to play a non-audio track
	entry.cdte_track = track;
	entry.cdte_format = CDROM_MSF;
	if ( ioctl(cdfile, CDROMREADTOCENTRY, &entry) == -1 )
	{
		Com_DPrintf("ioctl cdromreadtocentry failed\n");
		return;
	}
	if (entry.cdte_ctrl == CDROM_DATA_TRACK)
	{
		Com_Printf("CDAudio: track %i is not audio\n", track);
		return;
	}

	if (playing)
	{
		if (playTrack == track)
			return;
		CDAudio_Stop();
	}

	ti.cdti_trk0 = track;
	ti.cdti_trk1 = track;
	ti.cdti_ind0 = 1;
	ti.cdti_ind1 = 99;
	if ( ioctl(cdfile, CDROMPLAYTRKIND, &ti) == -1 ) 
	{
		Com_DPrintf("ioctl cdromplaytrkind failed\n");
		return;
	}
	if ( ioctl(cdfile, CDROMRESUME) == -1 ) 
		Com_DPrintf("ioctl cdromresume failed\n");

	playLooping = looping;
	playTrack = track;
	playing = true;

	if (cd_volume->value == 0.0)
		CDAudio_Pause ();
	*/
}


void CDAudio_Stop(void)
{
	if (cdfile == -1 || !enabled)
		return;
	if (!playing)
		return;

	Com_DPrintf("CDAudio_Stop: PORTME\n");

	/*
	if ( ioctl(cdfile, CDROMSTOP) == -1 )
		Com_DPrintf("ioctl cdromstop failed (%d)\n", errno);

	wasPlaying = false;
	playing = false;
	*/
}

void CDAudio_Resume(void)
{
	if (cdfile == -1 || !enabled)
		return;
	if (!cdValid)
		return;
	if (!wasPlaying)
		return;

	Com_DPrintf("CDAudio_Stop: PORTME\n");

	/*
	if ( ioctl(cdfile, CDROMRESUME) == -1 ) 
		Com_DPrintf("ioctl cdromresume failed\n");
	playing = true;
	*/
}

static void CD_f (void)
{
	char	*command;
	int n, ret;

	if (Cmd_Argc() < 2)
		return;

	command = Cmd_Argv (1);

	if (Q_strcasecmp(command, "on") == 0)
	{
		enabled = true;
		return;
	}

	if (Q_strcasecmp(command, "off") == 0)
	{
		if (playing)
			CDAudio_Stop();
		enabled = false;
		return;
	}

	if (Q_strcasecmp(command, "reset") == 0)
	{
		enabled = true;
		if (playing)
			CDAudio_Stop();
		for (n = 0; n < 100; n++)
			remap[n] = n;
		CDAudio_GetAudioDiskInfo();
		return;
	}

	if (Q_strcasecmp(command, "remap") == 0)
	{
		ret = Cmd_Argc() - 2;
		if (ret <= 0)
		{
			for (n = 1; n < 100; n++)
				if (remap[n] != n)
					Com_Printf("  %u -> %u\n", n, remap[n]);
			return;
		}
		for (n = 1; n <= ret; n++)
			remap[n] = atoi(Cmd_Argv (n+1));
		return;
	}

	if (Q_strcasecmp(command, "close") == 0)
	{
		CDAudio_CloseDoor();
		return;
	}

	if (!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if (!cdValid)
		{
			Com_Printf("No CD in player.\n");
			return;
		}
	}

	if (Q_strcasecmp(command, "play") == 0)
	{
		CDAudio_Play((byte)atoi(Cmd_Argv (2)), false);
		return;
	}

	if (Q_strcasecmp(command, "loop") == 0)
	{
		CDAudio_Play((byte)atoi(Cmd_Argv (2)), true);
		return;
	}

	if (Q_strcasecmp(command, "stop") == 0)
	{
		CDAudio_Stop();
		return;
	}

	if (Q_strcasecmp(command, "pause") == 0)
	{
		CDAudio_Pause();
		return;
	}

	if (Q_strcasecmp(command, "resume") == 0)
	{
		CDAudio_Resume();
		return;
	}

	if (Q_strcasecmp(command, "eject") == 0)
	{
		if (playing)
			CDAudio_Stop();
		CDAudio_Eject();
		cdValid = false;
		return;
	}

	if (Q_strcasecmp(command, "info") == 0)
	{
		Com_Printf("%u tracks\n", maxTrack);
		if (playing)
			Com_Printf("Currently %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		else if (wasPlaying)
			Com_Printf("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		Com_Printf("Volume is %f\n", cdvolume);
		return;
	}
}

void CDAudio_Update(void)
{
	if (cdfile == -1 || !enabled)
		return;
	if (cd_volume && cd_volume->value != cdvolume)
	{
		if (cdvolume)
		{
			Cvar_SetValue ("cd_volume", 0.0);
			cdvolume = cd_volume->value;
			CDAudio_Pause ();
		}
		else
		{
			Cvar_SetValue ("cd_volume", 1.0);
			cdvolume = cd_volume->value;
			CDAudio_Resume ();
		}
	}

	Com_DPrintf("CDAudio_Stop: PORTME\n");

	/*
	struct cdrom_subchnl subchnl;
	static time_t lastchk;

	if (playing && lastchk < time(NULL)) {
		lastchk = time(NULL) + 2; //two seconds between chks
		subchnl.cdsc_format = CDROM_MSF;
		if (ioctl(cdfile, CDROMSUBCHNL, &subchnl) == -1 ) {
			Com_DPrintf("ioctl cdromsubchnl failed\n");
			playing = false;
			return;
		}
		if (subchnl.cdsc_audiostatus != CDROM_AUDIO_PLAY &&
			subchnl.cdsc_audiostatus != CDROM_AUDIO_PAUSED) {
			playing = false;
			if (playLooping)
				CDAudio_Play(playTrack, true);
		}
	}
	*/
}

int CDAudio_Init(void)
{
	int i;
	cvar_t	*cv;

	cv = Cvar_Get ("nocdaudio", "0", CVAR_NOSET);
	if (cv->value)
		return -1;

	cd_nocd = Cvar_Get ("cd_nocd", "0", CVAR_ARCHIVE );
	if ( cd_nocd->value)
		return -1;

	cd_volume = Cvar_Get ("cd_volume", "1", CVAR_ARCHIVE);

	cd_dev = Cvar_Get("cd_dev", "/dev/cdrom", CVAR_ARCHIVE);

	if((cdfile = open(cd_dev->string, OREAD)) < 0){
		fprint(2, "CDAudio_Init: %r\n");
		Com_Printf("CDAudio_Init: failed to open \"%s\"\n", cd_dev->string);
		cdfile = -1;
		return -1;
	}

	for (i = 0; i < 100; i++)
		remap[i] = i;
	initialized = true;
	enabled = true;

	if (CDAudio_GetAudioDiskInfo())
	{
		Com_Printf("CDAudio_Init: No CD in player.\n");
		cdValid = false;
	}

	Cmd_AddCommand ("cd", CD_f);

	Com_Printf("CD Audio Initialized\n");

	return 0;
}

void CDAudio_Activate (qboolean active)
{
	if (active)
		CDAudio_Resume ();
	else
		CDAudio_Pause ();
}

void CDAudio_Shutdown(void)
{
	if (!initialized)
		return;
	CDAudio_Stop();
	close(cdfile);
	cdfile = -1;
}
