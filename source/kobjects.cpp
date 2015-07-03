#include "kobjects.h"

#include <cstring>

#include "constants.h"

#define PAGE_SIZE 0x1000

KCodeSet* FindTitleCodeSet(u64 title_id)
{
    for (unsigned kproc_index = 0; kproc_index < kproc_num; kproc_index++) {
        KCodeSet* curr_codeset = *(KCodeSet**)(kproc_start + kproc_size * kproc_index + kproc_codeset_offset);
        if (curr_codeset != nullptr && curr_codeset->title_id == title_id)
            return curr_codeset;
    }
    return nullptr;
}

KCodeSet* FindTitleCodeSet(const char* title_name, size_t title_len)
{
    for (unsigned kproc_index = 0; kproc_index < kproc_num; kproc_index++) {
        KCodeSet* curr_codeset = *(KCodeSet**)(kproc_start + kproc_size * kproc_index + kproc_codeset_offset);
        if (curr_codeset != nullptr && memcmp(curr_codeset->title_name, title_name, title_len) == 0)
            return curr_codeset;
    }
    return nullptr;
}

u32 FindCodeOffsetKAddr(KCodeSet* code_set, u32 code_offset)
{
    u32 curr_offset = 0;

    if (code_set == nullptr)
        return 0;

    KLinkedListNode* node = code_set->text_info.first_node;

    // Iterate through all the blocks in the codeset
    for (; node != nullptr; node = node->next) {
        KBlockInfo* block_info = (KBlockInfo*)node->data;
        u32 block_size = block_info->page_count * PAGE_SIZE;

        // The code offset is within this block
        if (code_offset > curr_offset && code_offset < curr_offset + block_size)
            return block_info->mem_section_start + (code_offset - curr_offset);

        // Current offset: amount of bytes searched so far
        curr_offset += block_size;

        if (node == code_set->text_info.last_node)
            break;
    };

    return 0;
}
