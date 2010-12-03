/**
  * GreenPois0n Syringe - libpois0n.h
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef INJECTPOIS0N_H
#define INJECTPOIS0N_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef void(*pois0n_callback)(double progress, void* object);
	
LIBSYRINGE_EXPORT void pois0n_init();
LIBSYRINGE_EXPORT void pois0n_set_callback(pois0n_callback callback, void* object);
LIBSYRINGE_EXPORT int pois0n_is_ready();
LIBSYRINGE_EXPORT int pois0n_is_compatible();
LIBSYRINGE_EXPORT int pois0n_inject();
LIBSYRINGE_EXPORT int pois0n_injectonly();
LIBSYRINGE_EXPORT void pois0n_exit();
LIBSYRINGE_EXPORT int boot_iboot();

#ifdef __cplusplus
}
#endif

#endif
