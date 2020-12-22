/**
 *  Virtual Memory Cart for Pseudo Saturn Kai
 *  Structure definitions by cafe-alpha.
 *
 *  This is licensed under WTFPL.
**/
/**
 *             DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                     Version 2, December 2004
 * 
 *  Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 * 
 *  Everyone is permitted to copy and distribute verbatim or modified
 *  copies of this license document, and changing it is allowed as long
 *  as the name is changed.
 * 
 *             DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *    TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 * 
 *   0. You just DO WHAT THE FUCK YOU WANT TO. 
**/

#ifndef _VMEM_DEFS_H_
#define _VMEM_DEFS_H_

#include <stdio.h>

/**
 * File Based Virtual Memory Device.
 * Backup data are read/written from/to "MEMCART" folder in SD card.
 *
 *
 * Example :
 * MEMCART (folder)
 *  + CONTENTS.IDX
 *  + DATA001.BUP
 *  + DATA002.BUP
 *  + README.TXT
 *
 * Each .BUP files contain following data :
 *  - Header data, including BupDir structure.
 *  - Save data itself.
 *
 * File naming follows the conditions below :
 *  - 8.3 file name
 *  - .BUP extension
 *
 * Maximum file count depends on RAM available to BUP
 * library, hence can't be increased easily.
 *
 *
 * Memory usage :
 * Refer to vmem_main.h file for details.
**/
#define CART_BUP_FOLDER "MEMCART"
#define BUP_FILEPREFIX  "DATA"
#define BUP_DATA_EXT    ".BUP"

/* Backup file example : "/MEMCART/DATA999.XYZ"
 * -> 20 characters
 * + null terminating character
 * => round to 24 characters
 */
#define BUP_FILE_NAMELEN 24



// GCC have alternative #pragma pack(N) syntax and old gcc version not
// support pack(push,N), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif


/**
 * Vmem usage statistics structure.
 * Statistics are reset on each vmem session, ie when Saturn is reset,
 * or when game calls BUP_Init function.
**/
typedef struct _vmem_bup_stats_t
{
    /* Number of times BUP_Dir function is called. */
    unsigned char dir_cnt;

    /* Number of times BUP_Read function is called. */
    unsigned char read_cnt;

    /* Number of times BUP_Write function is called. */
    unsigned char write_cnt;

    /* Number of times BUP_Verify function is called. */
    unsigned char verify_cnt;
} vmem_bup_stats_t;


/**
 * Backup data header.
**/
typedef struct _vmem_bup_header_t
{
    /* Magic string.
     * Used in order to verify that file is in vmem format.
     */
#define VMEM_MAGIC_STRING_LEN 4
#define VMEM_MAGIC_STRING "Vmem"
    char magic[VMEM_MAGIC_STRING_LEN];

    /* Save ID.
     * "Unique" ID for each save data file, the higher, the most recent.
     */
    unsigned long save_id;

    /* Vmem usage statistics. */
    vmem_bup_stats_t stats;

    /* Unused, kept for future use. */
    char unused1[8 - sizeof(vmem_bup_stats_t)];

    /* Backup Data Informations Record (34 bytes + 2 padding bytes). */
    BupDir dir;

    /* Date stamp, in Saturn's BUP library format.
     * Used in order to verify which save data is the most
     * recent one when rebuilding index data.
     * Note #1 : this information is already present in BupDir structure, 
     * but games are setting it, so it may be incorrect (typically, set 
     * to zero).
     * Note #2 : this date information is the date when Pseudo Saturn Kai
     * last started, not the time the save was saved, so if information in
     * dir structure is available, it is more accurate.
     */
    unsigned long date;

    /* Unused, kept for future use. */
    char unused2[8];
} vmem_bup_header_t;



/**
 * Maximum count for file index data.
 *
 * Note #1 : FAT16 maximum file count per directory is 512, so setting a maximum
 *           file count lower than this upper limit may be recommended
 * Note #2 : At least 16 bytes (backup name=16 characters, ID=4 bytes) are
 *           required per index, and as library RAM space is 16KB, there won't
 *           be more than 1024 saves per index.
**/
#define VMEM_CART_INDEX_MAX (1024)
#define VMEM_CART_INDEX_CNT (400)


/**
 * Index data header and status for each entries.
**/
typedef struct _vmem_idxheader_t
{
    /* Save counter.
     * Incremented before each data save
     * Used in order to verify which save is from the latest
     * revision, in order to keep old saves on SD card.
     */
    unsigned long savecntr;

    /* Vmem usage statistics. */
    vmem_bup_stats_t stats;

    /* Unused, kept for future use. */
    char unused1[8 - sizeof(vmem_bup_stats_t)];

    /* Magic string.
     * Used in order to verify that file is in vmem format.
     */
    char magic[VMEM_MAGIC_STRING_LEN];

    /* Bitfield to indicate if current file is used or not.
     * It may be smarter to set array size equal to
     * ((VMEM_CART_INDEX_CNT / 32) + 1)
     * But setting it at maximum size from start will allow
     * to increase index count in the future.
     */
    unsigned long file_used[VMEM_CART_INDEX_MAX / 32];
} vmem_idxheader_t;


/**
 * File index entry structure.
 * Associates SD card filename and backup filename
**/
typedef struct _vmem_idxentry_t
{
    /* Backup filename (null terminated), from BupDir structure. */
    Uint8 bup_filename[12];

    /* Save ID & date, the higher the most recent. */
    unsigned long save_id;
    unsigned long date;
} vmem_idxentry_t;


/**
 * File indexing structure.
 * This structure is read/saved from/to SD card.
**/
typedef struct _vmem_fileindex_t
{
    vmem_idxheader_t header;
    vmem_idxentry_t  entries[VMEM_CART_INDEX_CNT];
} vmem_fileindex_t;




/**
 * Misc global stuff for index data.
**/
typedef struct _vmem_idxglobal_t
{
    /* Buffer to store formatted backup file name. */
    char name_buff[BUP_FILE_NAMELEN];

    /* Buffer to store backup entry. */
    vmem_bup_header_t bup_hdr_buff;
    /* Padding for 4 bytes alignment. */
    unsigned char tb_pad[2];
} vmem_idxglobal_t;

// GCC have alternative #pragma pack() syntax and old gcc version not
// support pack(pop), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif // _VMEM_DEFS_H_
