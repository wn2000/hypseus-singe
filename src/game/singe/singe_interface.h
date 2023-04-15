#ifndef SINGE_INTERFACE_H
#define SINGE_INTERFACE_H

#include "../../video/overlay.h"

// increase this number every time you change something in this file!!!
#define SINGE_INTERFACE_API_VERSION 6

#define SINGE_ERROR_INIT      0xA0
#define SINGE_ERROR_RUNTIME   0xA1
#define SINGE_OW              0x140

// info provided to Singe from Hypseus
struct singe_in_info
{
	// the API version (THIS MUST COME FIRST!)
	unsigned int uVersion;

	// FUNCTIONS:

	// shuts down hypseus
	void (*set_quitflag)();

	// print a line to the debug console, and the log file (and to stdout on some platforms)
	void (*printline)(const char *, ...);
	
	// notifies hypseus of the last error that singe got (so hypseus can display it)
	void (*set_last_error)(const char *);

	// From video/video.h
	Uint16 (*get_video_width)();
	Uint16 (*get_video_height)();

	void (*draw_string)(const char*, int, int, Overlay *);
	
	// From sound/samples.h
	int (*samples_play_sample)(Uint8 *pu8Buf, unsigned int uLength, unsigned int uChannels, int iSlot, void (*finishedCallback)(Uint8 *pu8Buf, unsigned int uSlot));
	bool (*samples_set_state)  (unsigned int, bool);
	bool (*samples_is_playing) (unsigned int);
	bool (*samples_end_early) (unsigned int);
	void (*samples_flush_queue)();

	// Laserdisc Control Functions
	void (*enable_audio1)();
	void (*enable_audio2)();
	void (*disable_audio1)();
	void (*disable_audio2)();
	void (*request_screenshot)();
	void (*set_search_blanking)(bool enabled);
	void (*set_skip_blanking)(bool enabled);
	bool (*pre_change_speed)(unsigned int uNumerator, unsigned int uDenominator);
	uint32_t (*get_current_frame)();
	void (*pre_play)();
	void (*pre_pause)();
	void (*pre_stop)();
	bool (*pre_search)(const char *, bool block_until_search_finished);
	void (*framenum_to_frame)(Uint32, char *);
	bool (*pre_skip_forward)(Uint32);
	bool (*pre_skip_backward)(Uint32);
	void (*pre_step_forward)();
	void (*pre_step_backward)();

	double (*cfm_get_xratio)(void *);
	double (*cfm_get_yratio)(void *);

	//by RDG2010
	void *pSingeInstance;
	void (*cfm_set_keyboard_mode)(void *, int);
	int  (*cfm_get_keyboard_mode)(void *);	
	int  (*get_status)();
	double (*get_singe_version)(void);
	void (*set_ldp_verbose)(bool);	

	int  (*cfm_get_number_of_mice)(void *);

	bool (*get_retro_path)();
	void (*set_singe_errors)(short);

	// VARIABLES:
	
	// VLDP Interface
	struct vldp_in_info        *g_local_info;
	const struct vldp_out_info *g_vldp_info;
	
};

// info provided from Singe to Hypseus
struct singe_out_info
{
	// the API version (THIS MUST COME FIRST!)
	unsigned int uVersion;

	// FUNCTIONS:
	void (*sep_call_lua)(const char *func, const char *sig, ...);
	void (*sep_do_blit)(Overlay *dest);
	void (*sep_do_mouse_move)(Uint16 x, Uint16 y, Sint16 xrel, Sint16 yrel, Sint8 mouseID);
	void (*sep_error)(const char *fmt, ...);
	void (*sep_print)(const char *fmt, ...);
	void (*sep_set_static_pointers)(double *m_disc_fps, unsigned int *m_uDiscFPKS);
	void (*sep_set_surface)(int width, int height);
	void (*sep_shutdown)(void);
	void (*sep_startup)(const char *script);
	void (*sep_set_retropath)(void);
	void (*sep_alter_lua_clock)(bool);
	void (*sep_mute_vldp_init)(void);
	void (*sep_no_crosshair)(void);
	void (*sep_upgrade_overlay)(void);
	void (*sep_overlay_resize)(void);
	
	////////////////////////////////////////////////////////////
};

// if you want to build singe as a DLL, then define SINGE_DLL in your DLL project's preprocessor defs
#ifdef SINGE_DLL
#define SINGE_EXPORT __declspec(dllexport)
#else
// otherwise SINGE_EXPORT is blank
#define SINGE_EXPORT
#endif

extern "C"
{
SINGE_EXPORT const struct singe_out_info *singeproxy_init(const struct singe_in_info *in_info);
}

#endif // SINGE_INTERFACE_H
