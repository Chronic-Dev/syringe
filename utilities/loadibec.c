/**
  * Syringe-base loadibec -- loadibec.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  * Copyright (C) 2010 Ricky Taylor
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
#include "libirecovery.h"
#include "libpois0n.h"

#ifdef COMMIT
#define STRINGIFY(x) #x
#define XSTRINGIFY(x) STRINGIFY(x)
#define COMMIT_STRING " (" XSTRINGIFY(COMMIT) ")"
#else
#define COMMIT_STRING ""
#endif

#define LOADIBEC_VERSION "2.00"

int upload_ibss();
int upload_ibss_payload();

void print_progress(double progress) {
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

void print_pois0n_progress(double progress, void* data) {
	print_progress(progress);
}

int progress_cb(irecv_client_t client, const irecv_event_t* event) {
	if (event->type == IRECV_PROGRESS) {
		print_progress(event->progress);
	}
	return 0;
}

int main(int argc, char* argv[]) 
{
	irecv_error_t error;
	unsigned int cpid;
	int can_ra1n = 0;

	printf("Loadibec " LOADIBEC_VERSION COMMIT_STRING ".\n");

	if(argc != 2)
	{
		printf("Usage: %s <file>\n"
			"\tLoads a file to an iDevice in recovery mode and jumps to it.\n", argv[0]);
		return 0;
	}


	irecv_init();

	printf("Connecting to iDevice...\n");

	error = irecv_open_attempts(&client, 10);
	if(error != IRECV_E_SUCCESS)
	{
		fprintf(stderr, "Failed to connect to iBoot, error %d.\n", error);
		return -error;
	}
	
	if(irecv_get_cpid(client, &cpid) == IRECV_E_SUCCESS)
	{
		if(cpid > 8900)
			can_ra1n = 1;
	}

	if(client->mode == kDfuMode && can_ra1n)
	{
		int ret;
		printf("linera1n compatible device detected, injecting limera1n.\n");
		irecv_close(client);
		irecv_exit();

		pois0n_init();
		pois0n_set_callback(&print_pois0n_progress, NULL);

		ret = pois0n_is_ready();
		if(ret < 0)
			return ret;

		ret = pois0n_is_compatible();
		if(ret < 0)
			return ret;

		pois0n_injectonly();

		printf("limera1ned, reconnecting...\n");

		client = irecv_reconnect(client, 10);
		if(!client)
		{
			fprintf(stderr, "Failed to reconnect.\n");
			return 4;
		}

		printf("uploading ibss...\n");
		if(upload_ibss() >= 0)
		{
			client = irecv_reconnect(client, 10);
			if(upload_ibss_payload() >= 0)
			{
				irecv_send_command(client, "go");
				printf("iBSS loaded...\n");

				client = irecv_reconnect(client, 10);
			}
		}
	}
	else
		can_ra1n = 0;

	printf("Starting transfer of '%s'.\n", argv[1]);

	irecv_event_subscribe(client, IRECV_PROGRESS, &progress_cb, NULL);
	
	error = irecv_send_file(client, argv[1], 0);
	if(error != IRECV_E_SUCCESS)
	{
		fprintf(stderr, "Failed to upload '%s', error %d.\n", argv[1], error);
		return 2;
	}

	if(can_ra1n)
		error = irecv_send_command(client, "go jump 0x41000000");
	else
		error = irecv_send_command(client, "go");
	if(error != IRECV_E_SUCCESS)
	{
		fprintf(stderr, "Failed to jump to uploaded file, error %d.\n", error);
		return 3;
	}

	printf("Uploaded Successfully.\n");

	irecv_exit();

	return 0;
}
