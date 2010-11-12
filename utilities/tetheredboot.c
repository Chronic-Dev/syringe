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
#include <stdlib.h>

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

void usage()
{
	printf("Usage: tetheredboot <ibss> <kernelcache>\n");
	exit(0);
}

int main(int argc, char* argv[]) {
	int result = 0;
	if (argc != 3) {
		usage();
	}
	const char* ibssFile = argv[1];
	const char* kernelcacheFile = argv[2];
	
	pois0n_init();
	pois0n_set_callback(&print_progress, NULL);

	info("Waiting for device to enter DFU mode\n");
	while(pois0n_is_ready()) {
		sleep(1);
	}

	info("Found device in DFU mode\n");
	result = pois0n_is_compatible();
	if (result < 0) {
		error("Your device in incompatible with this exploit!\n");
		return result;
	}
	
	result = pois0n_injectonly();
	if (result < 0) {
		error("Exploit injection failed!\n");
		return result;
	}
	
	debug("Uploading %s to device\n", ibssFile);
	irecv_error_t error = irecv_send_file(client, ibssFile, 1);
	if(error != IRECV_E_SUCCESS) {
		error("Unable to upload iBSS\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	
	client = irecv_reconnect(client, 10);
	
	debug("Uploading %s to device\n", kernelcacheFile);
	error = irecv_send_file(client, kernelcacheFile, 1);
	if(error != IRECV_E_SUCCESS) {
		error("Unable to upload kernelcache\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	
	error = irecv_send_command(client, "bootx");
	if(error != IRECV_E_SUCCESS) {
		error("Unable send the bootx command\n");
		return -1;
	}
	
	pois0n_exit();
	return 0;
}
