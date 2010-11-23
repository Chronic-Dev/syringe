/**
  * GreenPois0n Syringe - libpartial.h
  * Copyright (C) 2010 David Wang
  *
  * Modified by:
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

#ifndef LIBPARTIAL_H
#define LIBPARTIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <curl/curl.h>

#ifdef _MSC_VER
#define STATIC_INLINE static __inline
#else
#define STATIC_INLINE static inline
#endif


#ifdef _WIN32
#define fseeko fseeko64
#define ftello ftello64
#define off_t off64_t
#define mkdir(x, y) mkdir(x)
#define PATH_SEPARATOR "\\"
#define sleep(n) Sleep(1000 * n)
#else
#define PATH_SEPARATOR "/"
#endif

#define TRUE 1
#define FALSE 0

#define FLIPENDIAN(x) flipEndian((unsigned char *)(&(x)), sizeof(x))
#define FLIPENDIANLE(x) flipEndianLE((unsigned char *)(&(x)), sizeof(x))

#define IS_BIG_ENDIAN      0
#define IS_LITTLE_ENDIAN   1

#define TIME_OFFSET_FROM_UNIX 2082844800L
#define APPLE_TO_UNIX_TIME(x) ((x) - TIME_OFFSET_FROM_UNIX)
#define UNIX_TO_APPLE_TIME(x) ((x) + TIME_OFFSET_FROM_UNIX)

#define ASSERT(x, m) if(!(x)) { fflush(stdout); fprintf(stderr, "error: %s\n", m); perror("error"); fflush(stderr); exit(1); }

extern char endianness;

STATIC_INLINE void flipEndian(unsigned char* x, int length) {
  int i;
  unsigned char tmp;

  if(endianness == IS_BIG_ENDIAN) {
    return;
  } else {
    for(i = 0; i < (length / 2); i++) {
      tmp = x[i];
      x[i] = x[length - i - 1];
      x[length - i - 1] = tmp;
    }
  }
}

STATIC_INLINE void flipEndianLE(unsigned char* x, int length) {
  int i;
  unsigned char tmp;

  if(endianness == IS_LITTLE_ENDIAN) {
    return;
  } else {
    for(i = 0; i < (length / 2); i++) {
      tmp = x[i];
      x[i] = x[length - i - 1];
      x[length - i - 1] = tmp;
    }
  }
}

STATIC_INLINE void hexToBytes(const char* hex, uint8_t** buffer, size_t* bytes) {
	size_t i;
	*bytes = strlen(hex) / 2;
	*buffer = (uint8_t*) malloc(*bytes);
	for(i = 0; i < *bytes; i++) {
		uint32_t byte;
		sscanf(hex, "%2x", &byte);
		(*buffer)[i] = byte;
		hex += 2;
	}
}

STATIC_INLINE void hexToInts(const char* hex, unsigned int** buffer, size_t* bytes) {
	size_t i;
	*bytes = strlen(hex) / 2;
	*buffer = (unsigned int*) malloc((*bytes) * sizeof(int));
	for(i = 0; i < *bytes; i++) {
		sscanf(hex, "%2x", &((*buffer)[i]));
		hex += 2;
	}
}

struct io_func_struct;

typedef int (*readFunc)(struct io_func_struct* io, off_t location, size_t size, void *buffer);
typedef int (*writeFunc)(struct io_func_struct* io, off_t location, size_t size, void *buffer);
typedef void (*closeFunc)(struct io_func_struct* io);

typedef struct io_func_struct {
  void* data;
  readFunc read;
  writeFunc write;
  closeFunc close;
} io_func;

#ifdef _MSC_VER
#define ATTRIBUTE_PACKED
#pragma pack(push)
#pragma pack(1)
#else
#define ATTRIBUTE_PACKED __attribute__ ((packed))
#endif

typedef struct EndOfCD {
	uint32_t signature;
	uint16_t diskNo;
	uint16_t CDDiskNo;
	uint16_t CDDiskEntries;
	uint16_t CDEntries;
	uint32_t CDSize;
	uint32_t CDOffset;
	uint16_t lenComment;
} ATTRIBUTE_PACKED EndOfCD;

typedef struct CDFile {
	uint32_t signature;
	uint16_t version;
	uint16_t versionExtract;
	uint16_t flags;
	uint16_t method;
	uint16_t modTime;
	uint16_t modDate;
	uint32_t crc32;
	uint32_t compressedSize;
	uint32_t size;
	uint16_t lenFileName;
	uint16_t lenExtra;
	uint16_t lenComment;
	uint16_t diskStart;
	uint16_t internalAttr;
	uint32_t externalAttr;
	uint32_t offset;
} ATTRIBUTE_PACKED CDFile;

typedef struct LocalFile {
	uint32_t signature;
	uint16_t versionExtract;
	uint16_t flags;
	uint16_t method;
	uint16_t modTime;
	uint16_t modDate;
	uint32_t crc32;
	uint32_t compressedSize;
	uint32_t size;
	uint16_t lenFileName;
	uint16_t lenExtra;
} ATTRIBUTE_PACKED LocalFile;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

typedef struct ZipInfo ZipInfo;

typedef void (*PartialZipProgressCallback)(ZipInfo* info, CDFile* file, size_t progress);

struct ZipInfo {
	char* url;
	uint64_t length;
	CURL* hIPSW;
	char* centralDirectory;
	size_t centralDirectoryRecvd;
	EndOfCD* centralDirectoryDesc;
	char centralDirectoryEnd[0xffff + sizeof(EndOfCD)];
	size_t centralDirectoryEndRecvd;
	PartialZipProgressCallback progressCallback;
};

int download_file_from_zip(const char* url, const char* path, const char* output, PartialZipProgressCallback progressCallback);
ZipInfo* PartialZipInit(const char* url);
CDFile* PartialZipFindFile(ZipInfo* info, const char* fileName);
CDFile* PartialZipListFiles(ZipInfo* info);
unsigned char* PartialZipGetFile(ZipInfo* info, CDFile* file);
void PartialZipRelease(ZipInfo* info);
void PartialZipSetProgressCallback(ZipInfo* info, PartialZipProgressCallback progressCallback);

#ifdef __cplusplus
}
#endif

#endif
