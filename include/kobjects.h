#pragma once

#include <3ds.h>

//-----------------------------------------------------------------------------

#pragma pack(1)
typedef struct KBlockInfo {
    /* 00 */ u32 mem_section_start;
    /* 04 */ u32 page_count;
} KBlockInfo;

typedef struct KLinkedListNode {
    /* 00 */ struct KLinkedListNode* next;
    /* 04 */ struct KLinkedListNode* prev;
    /* 08 */ void* data;
} KLinkedListNode;

typedef struct MemSectionInfo {
    /* 00 */ u32 start_addr;
    /* 04 */ u32 total_pages;
    /* 08 */ u32 kblockinfo_count;
    /* 0C */ KLinkedListNode* first_node;
    /* 10 */ KLinkedListNode* last_node;
} MemSectionInfo;

typedef struct KCodeSet {
    /* 00 */ u8 padding0[0x08 - 0x00];
    /* 08 */ MemSectionInfo text_info;
    /* 1C */ u8 padding1[0x50 - 0x1C];
    /* 50 */ char title_name[8];
    /* 58 */ u8 padding2[0x5C - 0x58];
    /* 5C */ u64 title_id;
} KCodeSet;
#pragma pack(0)

static_assert(sizeof(KBlockInfo) == 0x8, "KBlockInfo is the wrong size!");
static_assert(sizeof(KLinkedListNode) == 0xC, "KLinkedListNode is the wrong size!");
static_assert(sizeof(MemSectionInfo) == 0x14, "MemSectionInfo is the wrong size!");
static_assert(sizeof(KCodeSet) == 0x64, "KCodeSet is the wrong size!");

//-----------------------------------------------------------------------------

/**
 * Searches kernel memory for the KCodeSet matching a certain titleid
 * @param title_id Target title for which to find the codeset
 * @return pointer to found KCodeSet
 */
KCodeSet* FindTitleCodeSet(u64 title_id);

/**
 * Searches kernel memory for the KCodeSet matching a certain title name
 * @param title_name Target title for which to find the codeset
 * @param title_len Length of title_name string
 * @return pointer to found KCodeSet
 */
KCodeSet* FindTitleCodeSet(const char* title_name, size_t title_len);

/**
 * Finds the KAddr corresponding to a title's usermode code offset
 * For example, if the title's usermode base address is 0x00100000, and the code
 * you want to look for is at 0x0010AF70, the offset would be 0xAF70.
 * @param code_set Pointer to the title's KCodeSet
 * @param code_offset The offset from the usermode base address to find the KAddr for
 * @return KAddr corresponding to code offset
 */
u32 FindCodeOffsetKAddr(KCodeSet* code_set, u32 code_offset);