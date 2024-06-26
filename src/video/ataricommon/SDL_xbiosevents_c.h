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
 *	Xbios mouse & joystick vectors
 *
 *	Patrice Mandin
 */

#ifndef _SDL_ATARI_XBIOSEVENTS_H_
#define _SDL_ATARI_XBIOSEVENTS_H_

#include "../SDL_sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

#define ATARI_XBIOS_MOUSEEVENTS (1<<0)
#define ATARI_XBIOS_JOYSTICKEVENTS (1<<1)
#define ATARI_XBIOS_KEYBOARDEVENTS (1<<2)

extern SDL_bool SDL_AtariXbios_enabled;

extern void AtariXbios_InitOSKeymap(_THIS);
extern void AtariXbios_PumpEvents(_THIS);
extern void AtariXbios_ShutdownEvents(void);

extern void SDL_AtariXbios_InstallVectors(int vectors_mask);
extern void SDL_AtariXbios_ExclusiveVectors(int vectors_mask);
extern void SDL_AtariXbios_RestoreVectors(void);
extern void SDL_AtariXbios_PostMouseEvents(_THIS, SDL_bool buttonEvents);
extern void SDL_AtariXbios_LockMousePosition(SDL_bool lockPosition);
extern void SDL_AtariXbios_PostKeyboardEvents(_THIS);

extern SDL_bool SDL_AtariXbios_IsKeyboardVectorSupported();

#endif /* _SDL_XBIOSEVENTS_H_ */
