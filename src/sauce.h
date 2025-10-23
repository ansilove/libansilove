/*
 * SAUCE - Standard Architecture for Universal Comment Extensions
 * Metadata format for ANSi, ASCII, and related art files
 * 
 * SAUCE record is 128 bytes and appears at the end of the file:
 * - Bytes 0-4: "SAUCE" magic (5 bytes)
 * - Byte 5: Version (0 for this spec)
 * - Bytes 6-36: Title (31 bytes)
 * - Bytes 37-67: Author (31 bytes)
 * - Bytes 68-98: Group/Company (31 bytes)
 * - Bytes 99-102: Date (YYYYMMDD, 4 bytes)
 * - Bytes 103-106: File size in bytes (4 bytes, little-endian)
 * - Byte 107: DataType (0-8)
 * - Byte 108: FileType (depends on DataType)
 * - Bytes 109-110: TInfo1 (width info for text, little-endian)
 * - Bytes 111-112: TInfo2 (height info for text, little-endian)
 * - Bytes 113-114: TInfo3 (font info)
 * - Bytes 115-127: Comments record count / reserved
 */

#ifndef SAUCE_H
#define SAUCE_H

#include <stdint.h>
#include <stdbool.h>

#define SAUCE_ID "SAUCE"
#define SAUCE_ID_LEN 5
#define SAUCE_RECORD_SIZE 128

typedef struct {
    char title[32];
    char author[32];
    char group[32];
    char date[9];
    uint32_t filesize;
    uint8_t datatype;
    uint8_t filetype;
    uint16_t tinfo1;     /* Width (columns) */
    uint16_t tinfo2;     /* Height (rows) */
    uint16_t tinfo3;     /* Font ID or other */
    bool valid;
} sauce_record_t;

/*
 * Read SAUCE record from a file buffer
 * Returns true if SAUCE record found and valid
 */
static inline bool
sauce_read(const uint8_t *buffer, size_t buflen, sauce_record_t *sauce)
{
    if (buflen < SAUCE_RECORD_SIZE) {
        return false;
    }
    
    /* SAUCE record should be at the very end */
    const uint8_t *record = buffer + buflen - SAUCE_RECORD_SIZE;
    
    /* Check magic */
    if (record[0] != 'S' || record[1] != 'A' || record[2] != 'U' ||
        record[3] != 'C' || record[4] != 'E') {
        return false;
    }
    
    /* Parse record */
    if (sauce) {
        int i;
        
        /* Copy title, author, group (trim trailing spaces) */
        for (i = 0; i < 31; i++) {
            sauce->title[i] = record[6 + i];
        }
        sauce->title[31] = '\0';
        
        for (i = 0; i < 31; i++) {
            sauce->author[i] = record[37 + i];
        }
        sauce->author[31] = '\0';
        
        for (i = 0; i < 31; i++) {
            sauce->group[i] = record[68 + i];
        }
        sauce->group[31] = '\0';
        
        /* Date YYYYMMDD */
        for (i = 0; i < 8; i++) {
            sauce->date[i] = record[99 + i];
        }
        sauce->date[8] = '\0';
        
        /* File size (little-endian) */
        sauce->filesize = (record[103] << 24) | (record[102] << 16) |
                          (record[101] << 8) | record[100];
        
        /* Data and file types */
        sauce->datatype = record[107];
        sauce->filetype = record[108];
        
        /* Text info (little-endian) */
        sauce->tinfo1 = record[109] | (record[110] << 8);  /* Columns */
        sauce->tinfo2 = record[111] | (record[112] << 8);  /* Rows */
        sauce->tinfo3 = record[113] | (record[114] << 8);  /* Font */
        
        sauce->valid = true;
    }
    
    return true;
}

#endif /* SAUCE_H */
