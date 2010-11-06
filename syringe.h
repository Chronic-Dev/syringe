/**
  * GreenPois0n Syringe - syringe.h
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

#ifndef SYRINGE_H
#define SYRINGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef void(*syringe_callback)(double progress, void* object);

void syringe_init();
void syringe_set_callback(syringe_callback callback, void* object);
int syringe_is_ready();
int syringe_is_compatible();
int syringe_inject();
void syringe_exit();

#ifdef __cplusplus
}
#endif

#endif
