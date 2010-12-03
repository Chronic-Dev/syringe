#pragma once

typedef unsigned __int64 uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef size_t off64_t;

        /* PRINT FORMAT MACROS */
#define PRId8     "hhd"
#define PRId16    "hd"
#define PRId32    "ld"
#define PRId64    "lld"

#define PRIdFAST8    "hhd"
#define PRIdFAST16    "hd"
#define PRIdFAST32    "ld"
#define PRIdFAST64   "lld"

#define PRIdLEAST8   "hhd"
#define PRIdLEAST16  "hd"
#define PRIdLEAST32  "ld"
#define PRIdLEAST64  "lld"

#define PRIdMAX   "lld"
#define PRIdPTR   "lld"

#define PRIi8     "hhi"
#define PRIi16    "hi"
#define PRIi32    "li"
#define PRIi64    "lli"

#define PRIiFAST8    "hhi"
#define PRIiFAST16    "hi"
#define PRIiFAST32    "li"
#define PRIiFAST64    "lli"

#define PRIiLEAST8   "hhi"
#define PRIiLEAST16  "hi"
#define PRIiLEAST32  "li"
#define PRIiLEAST64  "lli"

#define PRIiMAX   "lli"
#define PRIiPTR   "lli"

#define PRIo8     "hho"
#define PRIo16    "ho"
#define PRIo32    "lo"
#define PRIo64    "llo"

#define PRIoFAST8    "hho"
#define PRIoFAST16    "ho"
#define PRIoFAST32    "lo"
#define PRIoFAST64   "llo"

#define PRIoLEAST8   "hho"
#define PRIoLEAST16  "ho"
#define PRIoLEAST32  "lo"
#define PRIoLEAST64  "llo"

#define PRIoMAX   "llo"
#define PRIoPTR   "llo"

#define PRIu8     "hhu"
#define PRIu16    "hu"
#define PRIu32    "lu"
#define PRIu64    "llu"

#define PRIuFAST8    "hhu"
#define PRIuFAST16    "hu"
#define PRIuFAST32    "lu"
#define PRIuFAST64   "llu"

#define PRIuLEAST8   "hhu"
#define PRIuLEAST16  "hu"
#define PRIuLEAST32  "lu"
#define PRIuLEAST64  "llu"

#define PRIuMAX   "llu"
#define PRIuPTR   "llu"

#define PRIx8     "hhx"
#define PRIx16    "hx"
#define PRIx32    "lx"
#define PRIx64    "llx"

#define PRIxFAST8    "hhx"
#define PRIxFAST16    "hx"
#define PRIxFAST32    "lx"
#define PRIxFAST64   "llx"

#define PRIxLEAST8   "hhx"
#define PRIxLEAST16  "hx"
#define PRIxLEAST32  "lx"
#define PRIxLEAST64  "llx"

#define PRIxMAX   "llx"
#define PRIxPTR   "llx"

#define PRIX8     "hhX"
#define PRIX16    "hX"
#define PRIX32    "lX"
#define PRIX64    "llX"

#define PRIXFAST8    "hhX"
#define PRIXFAST16    "hX"
#define PRIXFAST32    "lX"
#define PRIXFAST64   "llX"

#define PRIXLEAST8   "hhX"
#define PRIXLEAST16  "hX"
#define PRIXLEAST32  "lX"
#define PRIXLEAST64  "llX"

#define PRIXMAX   "llX"
#define PRIXPTR   "llX"