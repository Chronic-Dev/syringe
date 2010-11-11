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

#define CONNECT_ATTEMPTS 30
#define CONNECT_SLEEP 1000

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

int progress_cb(irecv_client_t client, const irecv_event_t* event) {
	if (event->type == IRECV_PROGRESS) {
		print_progress(event->progress);
	}
	return 0;
}

int main(int argc, char* argv[]) {
	if(argc != 2)
	{
		printf("Usage: %s <file>\n"
			"\n"
			"\tLoads a file to a device in recovery mode and jumps to it.\n", argv[0]);
		return 0;
	}

	irecv_client_t client;
	irecv_error_t error;
	int i;

	for(i = 0; i < CONNECT_ATTEMPTS; i++)
	{
		error = irecv_open(&client);
		if(error == IRECV_E_SUCCESS)
			break;

		sleep(CONNECT_SLEEP);
	}

	if(i == CONNECT_ATTEMPTS)
	{
		fprintf(stderr, "Failed to connect to iBoot, error %d.\n", error);
		return -error;
	}
	
	irecv_event_subscribe(client, IRECV_PROGRESS, &progress_cb, NULL);
	
	error = irecv_send_file(client, argv[1], 0);
	if(error != IRECV_E_SUCCESS)
	{
		fprintf(stderr, "Failed to upload '%s', error %d.\n", argv[1], error);
		return 2;
	}

	error = irecv_send_command(client, "go");
	if(error != IRECV_E_SUCCESS)
	{
		fprintf(stderr, "Failed to jump to uploaded file, error %d.\n", argv[1], error);
		return 3;
	}

	return 0;
}
