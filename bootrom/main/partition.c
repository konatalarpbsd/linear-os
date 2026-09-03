/*
* Copyright (c) 2026. All rights reserved
*
* written by KonataBSD
*
* This code is licensed under the GNU GPL v2 or later
*/


// under development, not completed

#include <utils.h>
#include <partition.h>


void partition_map_init(struct partition_hdr *hdr) {

hdr->magic = LINEAR_PARTITION_MAGIC;
hdr->ver = LINEAR_PARTITION_VER;
hdr->partition_limit = LINEAR_PARTITION_MAX;

}


void create_partition(const char *name, uint32_t fs, uint32_t type, uint32_t flags, uint64_t lba, uint64_t count) {


struct partition_name p = {
.fs_type = fs,
.type = type,
.flags = flags,
.first_lba = lba,
.sector_count = count
};

strncpy(p.name, name, sizeof(p.name) - 1);
p.name[sizeof(p.name) - 1] = '\0';

}
