/**
  * GreenPois0n Syringe - injectpois0n.c
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

#include <stdio.h>
#include <unistd.h>
#include "libpois0n.h"

void print_progress(double progress, void* data) {
	int i = 0;
	if(progress < 0) {
		return;
	}

	if(progress > 100) {
		progress = 100;
	}

	printf("\r[");
	for(i = 0; i < 50; i++) {
		if(i < progress / 2) {
			printf("=");
		} else {
			printf(" ");
		}
	}

	printf("] %3.1f%%", progress);
	if(progress == 100) {
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	if (argc > 1 && (!strcmp(argv[1],"--help") || !strcmp(argv[1],"-h"))) {
		printf("injectpois0n\n\t-t  Teather Boot\n\t-r  Boot ramdisk\n\t-s  Only execute iBSS payload\n\t-o  Only execute iBSS payload and boot iBoot\n\nUsage: %s [-r|-s|-o]\n", argv[0]);
		return 0;
	}
	pois0n_init();
	pois0n_set_callback(&print_progress, NULL);

	info("Waiting for device to enter DFU mode\n");
	while(pois0n_is_ready()) {
		sleep(1);
	}

	info("Found device in DFU mode\n");
	if(!pois0n_is_compatible()) {
		if (argc > 1) {
			if (!strcmp(argv[1],"-t"))
				pois0n_inject("1");
			else if (!strcmp(argv[1],"-r"))
				pois0n_inject("0");
			else if (!strcmp(argv[1],"-s"))
				pois0n_inject("2");
			else if (!strcmp(argv[1],"-o"))
				pois0n_inject("3");
		} else {
			pois0n_inject(NULL);
		}
	}

	pois0n_exit();
	return 0;
}
