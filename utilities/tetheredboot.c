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
#include <getopt.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "libpois0n.h"


bool file_exists(const char* fileName)
{
   struct stat buf;
   return !stat(fileName, &buf);
}

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
	printf("Usage: tetheredboot -i <ibss> -k <kernelcache> [-r <ramdisk>] [-b <bgcolor>] [-l <bootlogo.img3>]\n");
	exit(0);
}

bool g_verbose = false;

int main(int argc, char* argv[]) {
	int result = 0;
	irecv_error_t ir_error = IRECV_E_SUCCESS;

	//int index;
	const char 
		*ibssFile = NULL,
		*kernelcacheFile = NULL,
		*ramdiskFile = NULL,
		*bgcolor = NULL,
		*bootlogo = NULL;
	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "vhi:k:r:l:b:")) != -1)
		switch (c)
	{
		case 'v':
			g_verbose = true;
			break;
		case 'h':
			usage();
			break;
		case 'i':
			if (!file_exists(optarg)) {
				error("Cannot open iBSS file '%s'\n", optarg);
				return -1;
			}
			ibssFile = optarg;
			break;
		case 'k':
			if (!file_exists(optarg)) {
				error("Cannot open kernelcache file '%s'\n", optarg);
				return -1;
			}
			kernelcacheFile = optarg;
			break;
		case 'r':
			if (!file_exists(optarg)) {
				error("Cannot open ramdisk file '%s'\n", optarg);
				return -1;
			}
			ramdiskFile = optarg;
			break;
		case 'l':
			if (!file_exists(optarg)) {
				error("Cannot open bootlogo file '%s'\n", optarg);
				return -1;
			}
			bootlogo = optarg;
			break;
		case 'b':
			bgcolor = optarg;
			break;
		default:
			usage();
	}

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

	if (ibssFile != NULL) {
		debug("Uploading %s to device\n", ibssFile);
		ir_error = irecv_send_file(client, ibssFile, 1);
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable to upload iBSS\n");
			debug("%s\n", irecv_strerror(ir_error));
			return -1;
		}
	} else {
		return 0;
	}

	client = irecv_reconnect(client, 10);

	if (ramdiskFile != NULL) {
		debug("Uploading %s to device\n", ramdiskFile);
		ir_error = irecv_send_file(client, ramdiskFile, 1);
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable to upload ramdisk\n");
			debug("%s\n", irecv_strerror(ir_error));
			return -1;
		}

		sleep(5);

		ir_error = irecv_send_command(client, "ramdisk");
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable send the bootx command\n");
			return -1;
		}	
	}

	if (bootlogo != NULL) {
	        debug("Uploading %s to device\n", bootlogo);
		ir_error = irecv_send_file(client, bootlogo, 1);
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable to upload bootlogo\n");
			debug("%s\n", irecv_strerror(ir_error));
			return -1;
		}

		ir_error = irecv_send_command(client, "setpicture 1");
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable to set picture\n");
			return -1;
		}

                ir_error = irecv_send_command(client, "bgcolor 0 0 0");
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable to set picture\n");
                        return -1;
                }
	}

	if (bgcolor != NULL) {
		char finalbgcolor[255];
		sprintf(finalbgcolor, "bgcolor %s", bgcolor);
		ir_error = irecv_send_command(client, finalbgcolor);
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable set bgcolor\n");
			return -1;
		}
	}
	
	if (kernelcacheFile != NULL) {
		debug("Uploading %s to device\n", kernelcacheFile);
		ir_error = irecv_send_file(client, kernelcacheFile, 1);
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable to upload kernelcache\n");
			debug("%s\n", irecv_strerror(ir_error));
			return -1;
		}

		ir_error = irecv_send_command(client, "bootx");
		if(ir_error != IRECV_E_SUCCESS) {
			error("Unable send the bootx command\n");
			return -1;
		}
	}

	pois0n_exit();
	return 0;
}
