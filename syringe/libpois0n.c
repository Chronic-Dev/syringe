/**
 * GreenPois0n Syringe - libpois0n.c
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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "libpois0n.h"
#include "libpartial.h"
#include "libirecovery.h"

#include "common.h"
#include "ramdisk.h"
#include "exploits.h"
#include "payloads.h"

#define LIMERA1N
#define STEAKS4UCE
//#define PWNAGE2

static pois0n_callback progress_callback = NULL;
static void* user_object = NULL;

int recovery_callback(irecv_client_t client, const irecv_event_t* event) {
	if(progress_callback)
		progress_callback(event->progress, user_object);
	return 0;
}

void download_callback(ZipInfo* info, CDFile* file, size_t progress) {
	if(progress_callback)
		progress_callback(progress, user_object);
}

int send_command(char* command) {
	unsigned int ret = 0;
	irecv_error_t error = IRECV_E_SUCCESS;
	error = irecv_send_command(client, command);
	if (error != IRECV_E_SUCCESS) {
		printf("Unable to send command\n");
		return -1;
	}

	error = irecv_getret(client, &ret);
	if (error != IRECV_E_SUCCESS) {
		printf("Unable to send command\n");
		return -1;
	}

	return ret;
}

int fetch_image(const char* path, const char* output) {
	debug("Fetching %s...\n", path);
	if (download_file_from_zip(device->url, path, output, &download_callback)
			!= 0) {
		error("Unable to fetch %s\n", path);
		return -1;
	}

	return 0;
}

int fetch_dfu_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.RELEASE.dfu", type, device->model);
	snprintf(path, 254, "Firmware/dfu/%s", name);

	debug("Preparing to fetch DFU image from Apple's servers\n");
	if (fetch_image(path, output) < 0) {
		error("Unable to fetch DFU image from Apple's servers\n");
		return -1;
	}

	return 0;
}

int fetch_firmware_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.img3", type, device->model);
	snprintf(path, 254, "Firmware/all_flash/all_flash.%s.production/%s", device->model, name);

	debug("Preparing to fetch firmware image from Apple's servers\n");
	if (fetch_image(path, output) < 0) {
		error("Unable to fetch firmware image from Apple's servers\n");
	}

	return 0;
}

int upload_dfu_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s", type, device->model);

	debug("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_dfu_image(type, image) < 0) {
			error("Unable to upload DFU image\n");
			return -1;
		}
	}

	if (client->mode != kDfuMode) {
		debug("Resetting device counters\n");
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			debug("%s\n", irecv_strerror(error));
			return -1;
		}
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int upload_firmware_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s", type, device->model);

	debug("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_firmware_image(type, image) < 0) {
			error("Unable to upload firmware image\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int upload_firmware_payload(const char* type) {
	int size = 0;
	const unsigned char* payload = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;

	switch (device->index) {
	case DEVICE_APPLETV2:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_k66ap;
			size = sizeof(iBSS_k66ap);
			debug("Loaded payload for iBSS on k66ap\n0");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_k66ap;
		 size = sizeof(iBEC_k66ap);
		 debug("Loaded payload for iBEC on k66ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_k66ap;
			size = sizeof(iBoot_k66ap);
			debug("Loaded payload for iBoot on k66ap\n");
		}
		break;

	case DEVICE_IPAD1G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_k48ap;
			size = sizeof(iBSS_k48ap);
			debug("Loaded payload for iBSS on k48ap\n0");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_k48ap;
		 size = sizeof(iBEC_k48ap);
		 debug("Loaded payload for iBEC on k48ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_k48ap;
			size = sizeof(iBoot_k48ap);
			debug("Loaded payload for iBoot on k48ap\n");
		}
		break;

	case DEVICE_IPHONE3GS:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n88ap;
			size = sizeof(iBSS_n88ap);
			debug("Loaded payload for iBSS on n88ap\n");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_n88ap;
		 size = sizeof(iBEC_n88ap);
		 debug("Loaded payload for iBEC on n88ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_n88ap;
			size = sizeof(iBoot_n88ap);
			debug("Loaded payload for iBoot on n88ap\n");
		}
		break;

	case DEVICE_IPHONE4:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n90ap;
			size = sizeof(iBSS_n90ap);
			debug("Loaded payload for iBSS on n90ap\n");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_n90ap;
		 size = sizeof(iBEC_n90ap);
		 debug("Loaded payload for iBEC on n90ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_n90ap;
			size = sizeof(iBoot_n90ap);
			debug("Loaded payload for iBoot on n90ap\n");
		}
		break;

	case DEVICE_IPOD2G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n72ap;
			size = sizeof(iBSS_n72ap);
			debug("Loaded payload for iBSS on n72ap\n");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_n72ap;
		 size = sizeof(iBEC_n72ap);
		 debug("Loaded payload for iBEC on n72ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_n72ap;
			size = sizeof(iBoot_n72ap);
			debug("Loaded payload for iBoot on n72ap\n");
		}
		break;

	case DEVICE_IPOD3G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n18ap;
			size = sizeof(iBSS_n18ap);
			debug("Loaded payload for iBSS on n18ap\n");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_n18ap;
		 size = sizeof(iBEC_n18ap);
		 debug("Loaded payload for iBEC on n18ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_n18ap;
			size = sizeof(iBoot_n18ap);
			debug("Loaded payload for iBoot on n18ap\n");
		}
		break;

	case DEVICE_IPOD4G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n81ap;
			size = sizeof(iBSS_n81ap);
			debug("Loaded payload for iBSS on n81ap\n");
		}
		/*
		 if(!strcmp(type, "iBEC")) {
		 payload = iBEC_n81ap;
		 size = sizeof(iBEC_n81ap);
		 debug("Loaded payload for iBEC on n81ap\n");
		 }
		 */
		if (!strcmp(type, "iBoot")) {
			payload = iBoot_n81ap;
			size = sizeof(iBoot_n81ap);
			debug("Loaded payload for iBoot on n81ap\n");
		}
		break;
	}

	if (payload == NULL) {
		error("Unable to upload firmware payload\n");
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware payload\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s payload\n", type);
	error = irecv_send_buffer(client, (unsigned char*) payload, size, 1);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload %s payload\n", type);
		return -1;
	}

	return 0;
}

int upload_ibss() {
	if (upload_dfu_image("iBSS") < 0) {
		error("Unable upload iBSS\n");
		return -1;
	}
	return 0;
}

int upload_iboot() {
	if (upload_firmware_image("iBoot") < 0) {
		error("Unable upload iBoot\n");
		return -1;
	}
	return 0;
}

int upload_devicetree() {
	if (upload_firmware_image("DeviceTree") < 0) {
		error("Unable upload DeviceTree\n");
		return -1;
	}
	return 0;
}

int upload_ramdisk() {
	if (irecv_send_buffer(client, (unsigned char*) ramdisk, sizeof(ramdisk), 0)
			< 0) {
		error("Unable upload ramdisk\n");
		return -1;
	}
	return 0;
}

int upload_kernelcache() {
	struct stat buf;
	char kernelcache[255];
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(kernelcache, '\0', 255);
	memset(&buf, '\0', sizeof(buf));
	snprintf(kernelcache, 254, "kernelcache.release.%c%c%c", device->model[0], device->model[1], device->model[2]);
	debug("Checking if kernelcache already exists\n");
	if (stat(kernelcache, &buf) != 0) {
		if (fetch_image(kernelcache, kernelcache) < 0) {
			error("Unable to upload kernelcache\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload kernelcache\n");
		return -1;
	}

	error = irecv_send_file(client, kernelcache, 1);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload kernelcache\n");
		return -1;
	}

	return 0;
}

int upload_ibss_payload() {
	if (upload_firmware_payload("iBSS") < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}
	return 0;
}

int upload_ibec_payload() {
	if (upload_firmware_payload("iBEC") < 0) {
		error("Unable to upload iBEC payload\n");
		return -1;
	}
	return 0;
}

int boot_ramdisk() {
	irecv_error_t error = IRECV_E_SUCCESS;

	// Add an exception for this since it's very different
	debug("Preparing to upload ramdisk\n");
	if (upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		return -1;
	}

	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "ramdisk");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to execute ramdisk command\n");
		return -1;
	}

	debug("Setting kernel bootargs\n");
	error = irecv_send_command(client,
			"go kernel bootargs rd=md0 -v keepsyms=1");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to set kernel bootargs\n");
		return -1;
	}

	debug("Preparing to upload kernelcache\n");
	if (upload_kernelcache() < 0) {
		error("Unable to upload kernelcache\n");
		return -1;
	}

	error = irecv_send_command(client, "bootx");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to move iBoot into memory\n");
		return -1;
	}

	return 0;
}

int boot_tethered() {
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	// Add an exception for this since it's very different
	if (device->index == DEVICE_APPLETV2) {
		debug("Preparing to upload kernelcache\n");
		if (upload_kernelcache() < 0) {
			error("Unable to upload kernelcache\n");
			return -1;
		}

		debug("Hooking jump_to command\n");
		error = irecv_send_command(client, "go rdboot");
		if (error != IRECV_E_SUCCESS) {
			error("Unable to hook jump_to\n");
			return -1;
		}

		debug("Booting kernel\n");
		error = irecv_send_command(client, "bootx");
		if (error != IRECV_E_SUCCESS) {
			error("Unable to boot kernel\n");
			return -1;
		}

		return 0;
	}

	debug("Preparing to boot iBoot\n");
	if (boot_iboot() < 0) {
		error("Unable to boot iBoot\n");
		return -1;
	}

	debug("Preparing to upload ramdisk\n");
	if (upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		return -1;
	}

	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "go ramdisk 1 1");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to execute ramdisk command\n");
		return -1;
	}

	debug("Setting kernel bootargs\n");
	error = irecv_send_command(client,
			"go kernel bootargs rd=disk0s1 -v keepsyms=1");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to set kernel bootargs\n");
		return -1;
	}

	irecv_setenv(client, "boot-args", "0");
	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	error = irecv_send_command(client, "go fsboot");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to fsboot\n");
		return -1;
	}

	return 0;
}

int boot_iboot() {
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("Loading iBoot\n");
	if (device->chip_id == 8720) {
		error
				= irecv_send_command(client,
						"go image load 0x69626F74 0x9000000");
	} else {
		error = irecv_send_command(client,
				"go image load 0x69626F74 0x41000000");
	}
	if (error != IRECV_E_SUCCESS) {
		error("Unable load iBoot to memory\n");
		return -1;
	}

	debug("Shifting iBoot\n");
	if (device->chip_id == 8720) {
		error = irecv_send_command(client,
				"go memory move 0x9000040 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client,
				"go memory move 0x41000040 0x41000000 0x48000");
	}
	if (error != IRECV_E_SUCCESS) {
		error("Unable to move iBoot into memory\n");
		return -1;
	}

	debug("Patching iBoot\n");
	if (device->chip_id == 8720) {
		error = irecv_send_command(client, "go patch 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go patch 0x41000000 0x48000");
	}
	if (error != IRECV_E_SUCCESS) {
		error("Unable to patch iBoot\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);

	debug("Jumping into iBoot\n");
	if (device->chip_id == 8720) {
		error = irecv_send_command(client, "go jump 0x9000000");
	} else {
		error = irecv_send_command(client, "go jump 0x41000000");
	}
	if (error != IRECV_E_SUCCESS) {
		error("Unable to jump into iBoot\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	if (upload_firmware_payload("iBoot") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	return 0;
}

//bootarg should only be passed IF we want to specify.. otherwise, it MUST BE NULL
int execute_ibss_payload(char *bootarg) {
	//int i = 0;
	char* bootargs = bootarg;
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("Initializing greenpois0n in iBSS\n");
	irecv_send_command(client, "go");

	if (bootargs == NULL) {
		// Code to detect whether to boot ramdisk or filesystem
		debug("Checking if device is already jailbroken\n");
		error = irecv_getenv(client, "boot-args", &bootargs);
		if (error != IRECV_E_SUCCESS) {
			debug("%s\n", irecv_strerror(error));
			error("Unable to read env var\n");
			return -1;
		}
	}

	// If boot-args hasn't been set then we've never been jailbroken
	if (!strcmp(bootargs, "") || !strcmp(bootargs, "0")) {
		debug("Booting jailbreak ramdisk\n");
		if (boot_ramdisk() < 0) {
			error("Unable to boot device into tethered mode\n");
			return -1;
		}
	}
	// If boot-args is 1 then boot device into tethered mode
	else if (!strcmp(bootargs, "1")) {
		debug("Booting tethered device\n");
		if (boot_tethered() < 0) {
			error("Unable to boot device into tethered mode\n");
			return -1;
		}
	}
	// If boot-args is 2, then don't boot kernel, just load iBSS payload
	else if (!strcmp(bootargs, "2")) {
		debug("Booting iBSS in payload mode\n");
		return 0;
	}
	// If boot-args is 3, then don't boot kernel, just load iBoot payload
	else if (!strcmp(bootargs, "3")) {
		debug("Booting device in verbose mode\n");
		if (boot_iboot() < 0) {
			error("Unable to boot device into verbose mode\n");
			return -1;
		}
	}

	return 0;
}

void pois0n_init() {
	irecv_init();
	irecv_set_debug_level(libpois0n_debug);
	debug("Initializing libpois0n\n");

#ifdef __APPLE__
	system("killall -9 iTunesHelper");
#endif

#ifdef _WIN32
	system("TASKKILL /F /IM iTunes.exe > NUL");
	system("TASKKILL /F /IM iTunesHelper.exe > NUL");
#endif
}

void pois0n_set_callback(pois0n_callback callback, void* object) {
	progress_callback = callback;
	user_object = object;
}

int pois0n_is_ready() {
	irecv_error_t error = IRECV_E_SUCCESS;

	//////////////////////////////////////
	// Begin
	// debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		debug("Device must be in DFU mode to continue\n");
		return -1;
	}
	irecv_event_subscribe(client, IRECV_PROGRESS, &recovery_callback, NULL);

	//////////////////////////////////////
	// Check device
	// debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}

	return 0;
}

int pois0n_is_compatible() {
	irecv_error_t error = IRECV_E_SUCCESS;
	info("Checking if device is compatible with this jailbreak\n");

	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}
	info("Identified device as %s\n", device->product);

	if (device->chip_id != 8930
#ifdef LIMERA1N
			&& device->chip_id != 8922 && device->chip_id != 8920
#endif
#ifdef STEAKS4UCE
			&& device->chip_id != 8720
#endif
	) {
		error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}

	return 0;
}

void pois0n_exit() {
	debug("Exiting libpois0n\n");
	irecv_close(client);
	irecv_exit();
}

int pois0n_injectonly() {
	//////////////////////////////////////
	// Send exploit
	if (device->chip_id == 8930) {

#ifdef LIMERA1N

		debug("Preparing to upload limera1n exploit\n");
		if (limera1n_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}

#else

		error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else if (device->chip_id == 8920 || device->chip_id == 8922) {

#ifdef LIMERA1N

		debug("Preparing to upload limera1n exploit\n");
		if (limera1n_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}

#else

		error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else if (device->chip_id == 8720) {

#ifdef STEAKS4UCE

		debug("Preparing to upload steaks4uce exploit\n");
		if (steaks4uce_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}

#else

		error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else if (device->chip_id == 8900) {

#ifdef PWNAGE2

		debug("Preparing to upload pwnage2 exploit\n");
		if(pwnage2_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}

#else

		error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else {
		error("Sorry, this device is not currently supported\n");
		return -1;
	}
	return 0;
}

//Bootargs must be NULL unless we want to specify the boot type
int pois0n_inject(char *bootargs) {
	int result = 0;
	result = pois0n_injectonly();
	if (result < 0) {
		error("DFU Exploit injection failed (%u)\n", result);
		return result;
	}
	//////////////////////////////////////
	// Send iBSS
	debug("Preparing to upload iBSS\n");
	if (upload_ibss() < 0) {
		//error("Unable to upload iBSS\n");
		//return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload iBSS payload\n");
	if (upload_ibss_payload() < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}

	debug("Executing iBSS payload\n");
	if (execute_ibss_payload(bootargs) < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	return 0;
}
