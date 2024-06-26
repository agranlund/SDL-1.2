/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/*
 *	XBIOS mouse & joystick vectors
 *
 *	Patrice Mandin
 */

#include <mint/osbind.h>

#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_atarisuper.h"
#include "SDL_atarikeys.h"
#include "SDL_atarievents_c.h"
#include "SDL_xbiosevents_c.h"
#include "SDL_xbiosinterrupt_s.h"


#define KEY_PRESSED		0xff
#define KEY_UNDEFINED	0x80
#define KEY_RELEASED	0x00

/* Variables */

SDL_bool SDL_AtariXbios_enabled=SDL_FALSE;

/* Local variables */

static Uint16 atari_prevmouseb;	/* buttons */

/* Functions */

SDL_bool SDL_AtariXbios_IsKeyboardVectorSupported_Super()
{
	uint16_t* oshdr = *((uint16_t**)(0x4f2));
	return (oshdr[1] >= 0x0200) ? SDL_TRUE : SDL_FALSE;
}

SDL_bool SDL_AtariXbios_IsKeyboardVectorSupported()
{
	return Supexec(SDL_AtariXbios_IsKeyboardVectorSupported_Super);
}

void AtariXbios_InitOSKeymap(_THIS)
{
	int vectors_mask;
	vectors_mask  = ATARI_XBIOS_JOYSTICKEVENTS;	/* XBIOS joystick events */
	vectors_mask |= ATARI_XBIOS_MOUSEEVENTS;	/* XBIOS mouse events */
	if (SDL_AtariXbios_IsKeyboardVectorSupported()) {
		vectors_mask |= ATARI_XBIOS_KEYBOARDEVENTS;	/* XBIOS keyboard events */
	} else {
		SDL_SetError("Xbios keyboard vector is not supported on TOS 1.x");
		return 0;
	}

	SDL_AtariXbios_InstallVectors(vectors_mask);
	SDL_AtariXbios_ExclusiveVectors(vectors_mask);
}

void AtariXbios_PumpEvents(_THIS)
{
	SDL_AtariXbios_PostKeyboardEvents(this);
	SDL_AtariXbios_PostMouseEvents(this, SDL_TRUE);
}

void AtariXbios_ShutdownEvents(void)
{
	SDL_AtariXbios_RestoreVectors();
}

void SDL_AtariXbios_InstallVectors(int vectors_mask)
{
	/* Clear variables */
	SDL_AtariXbios_mouselock =
		SDL_AtariXbios_mouseb =
		SDL_AtariXbios_mousex =
		SDL_AtariXbios_mousey =
		SDL_AtariXbios_joystick =
		atari_prevmouseb = 0;

	SDL_memset((void*)SDL_AtariXbios_keyboard, KEY_UNDEFINED, 128);

	if (vectors_mask==0) {
		SDL_AtariXbios_enabled=SDL_FALSE;
		return;
	}

	/* Install our vectors */
	SDL_AtariXbios_exclusivemousevector = 0;
	SDL_AtariXbios_exclusivejoystickvector = 0;
	SDL_AtariXbios_exclusivekeyboardvector = 0;
	SDL_AtariXbios_installmousevector = (vectors_mask & ATARI_XBIOS_MOUSEEVENTS) != 0;
	SDL_AtariXbios_installjoystickvector = (vectors_mask & ATARI_XBIOS_JOYSTICKEVENTS) != 0;
	SDL_AtariXbios_installkeyboardvector = (vectors_mask & ATARI_XBIOS_KEYBOARDEVENTS) != 0;

	Supexec(SDL_AtariXbios_Install);
	/* SDL_AtariXbios_Restore() doesn't need SDL_AtariXbios_enabled */
	Setexc(VEC_PROCTERM, SDL_AtariXbios_Restore);

	SDL_AtariXbios_enabled=SDL_TRUE;
}

void SDL_AtariXbios_ExclusiveVectors(int vectors_mask)
{
	SDL_AtariXbios_exclusivemousevector = (vectors_mask & ATARI_XBIOS_MOUSEEVENTS) != 0;
	SDL_AtariXbios_exclusivejoystickvector = (vectors_mask & ATARI_XBIOS_JOYSTICKEVENTS) != 0;
	SDL_AtariXbios_exclusivekeyboardvector = (vectors_mask & ATARI_XBIOS_KEYBOARDEVENTS) != 0;
}

void SDL_AtariXbios_RestoreVectors(void)
{
	if (!SDL_AtariXbios_enabled) {
		return;
	}

	/* Reinstall system vector */
	Supexec(SDL_AtariXbios_Restore);
}

static int atari_GetButton(int button)
{
	switch(button)
	{
		case 0:
			return SDL_BUTTON_RIGHT;
			break;
		case 1:
		default:
			return SDL_BUTTON_LEFT;
			break;
	}
}

void SDL_AtariXbios_PostMouseEvents(_THIS, SDL_bool buttonEvents)
{
	if (!SDL_AtariXbios_enabled) {
		return;
	}

	/* Mouse motion ? */
	if (SDL_AtariXbios_mousex || SDL_AtariXbios_mousey) {
		SDL_PrivateMouseMotion(0, 1, SDL_AtariXbios_mousex, SDL_AtariXbios_mousey);
		SDL_AtariXbios_mousex = SDL_AtariXbios_mousey = 0;
	}
	
	/* Mouse button ? */
	if (buttonEvents && (SDL_AtariXbios_mouseb != atari_prevmouseb)) {
		int i;

		for (i=0;i<2;i++) {
			int curbutton, prevbutton;

			curbutton = SDL_AtariXbios_mouseb & (1<<i);
			prevbutton = atari_prevmouseb & (1<<i);

			if (curbutton && !prevbutton) {
				SDL_PrivateMouseButton(SDL_PRESSED, atari_GetButton(i), 0, 0);
			}
			if (!curbutton && prevbutton) {
				SDL_PrivateMouseButton(SDL_RELEASED, atari_GetButton(i), 0, 0);
			}
		}
		atari_prevmouseb = SDL_AtariXbios_mouseb;
	}
}

void SDL_AtariXbios_LockMousePosition(SDL_bool lockPosition)
{
	SDL_AtariXbios_mouselock = lockPosition;
}

void SDL_AtariXbios_PostKeyboardEvents(_THIS)
{
	short i, kstate;
	SDL_keysym keysym;

	if (!SDL_AtariXbios_enabled) {
		return;
	}

	kstate  = (SDL_AtariXbios_keyboard[SCANCODE_LEFTSHIFT] == KEY_PRESSED) ? K_LSHIFT : 0;
	kstate |= (SDL_AtariXbios_keyboard[SCANCODE_RIGHTSHIFT] == KEY_PRESSED) ? K_RSHIFT : 0;
	kstate |= (SDL_AtariXbios_keyboard[SCANCODE_LEFTCONTROL] == KEY_PRESSED) ? K_CTRL : 0;
	kstate |= (SDL_AtariXbios_keyboard[SCANCODE_LEFTALT] == KEY_PRESSED) ? K_ALT : 0;

	for (i=0; i<sizeof(SDL_AtariXbios_keyboard); i++) {
		/* Key pressed ? */
		if (SDL_AtariXbios_keyboard[i]==KEY_PRESSED) {
			SDL_PrivateKeyboard(SDL_PRESSED,
				SDL_Atari_TranslateKey(i, &keysym, SDL_TRUE, kstate));
			SDL_AtariXbios_keyboard[i]=KEY_UNDEFINED;
		}
			
		/* Key released ? */
		if (SDL_AtariXbios_keyboard[i]==KEY_RELEASED) {
			SDL_PrivateKeyboard(SDL_RELEASED,
				SDL_Atari_TranslateKey(i, &keysym, SDL_FALSE, kstate));
			SDL_AtariXbios_keyboard[i]=KEY_UNDEFINED;
		}
	}
}
