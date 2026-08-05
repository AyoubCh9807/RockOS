#pragma once

#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "block_manager.hpp"
#include "fs_types.hpp"
#include "inode.hpp"
#include "inode_manager.hpp"

constexpr u32 DIRECTORY_ENTRIES_PER_BLOCK =
    BLOCK_SIZE / sizeof(DirectoryEntry);

class DirectoryManager {

private:
    InodeManager& inode_manager;
    BlockManager& block_manager;

public:
    DirectoryManager(InodeManager& inode_manager,
                     BlockManager& block_manager)
        : inode_manager(inode_manager),
          block_manager(block_manager) {}

    bool add_entry(u32 directory_inode,
                   const char* name,
                   u32 inode_number)
    {
        if (directory_inode >= TOTAL_INODES)
            return false;

        if (inode_number >= TOTAL_INODES)
            return false;

        if (StringUtils::strlen(name) >= MAX_FILENAME_LENGTH)
            return false;

        Inode dir_inode;

        if (!inode_manager.read_inode(directory_inode, dir_inode))
            return false;

        if (!dir_inode.is_directory)
            return false;

        DirectoryEntry* first_free = nullptr;
        u8* first_free_buffer = nullptr;
        u32 first_free_block = INVALID_BLOCK;

        u8 buffers[DIRECT_BLOCKS][BLOCK_SIZE];

        //
        // Search every existing directory block
        //
        for (u32 i = 0; i < DIRECT_BLOCKS; i++)
        {
            u32 block = dir_inode.direct_blocks[i];

            if (block == INVALID_BLOCK)
                continue;

            if (!block_manager.read_block(block, buffers[i]))
                return false;

            DirectoryEntry* entries =
                reinterpret_cast<DirectoryEntry*>(buffers[i]);

            for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++)
            {
                if (entries[j].is_used)
                {
                    if (StringUtils::strcmp(entries[j].name, name) == 0)
                        return false;
                }
                else if (first_free == nullptr)
                {
                    first_free = &entries[j];
                    first_free_buffer = buffers[i];
                    first_free_block = block;
                }
            }
        }

        //
        // Reuse an existing free slot
        //
        if (first_free != nullptr)
        {
            StringUtils::strcpy(first_free->name, name);
            first_free->inode_number = inode_number;
            first_free->is_used = true;

            return block_manager.write_block(first_free_block,
                                             first_free_buffer);
        }

        //
        // Need a brand new directory block
        //
        u32 pointer_index = INVALID_BLOCK;

        for (u32 i = 0; i < DIRECT_BLOCKS; i++)
        {
            if (dir_inode.direct_blocks[i] == INVALID_BLOCK)
            {
                pointer_index = i;
                break;
            }
        }

        if (pointer_index == INVALID_BLOCK)
            return false;

        u32 new_block = block_manager.allocate_block();

        if (new_block == INVALID_BLOCK)
            return false;

        dir_inode.direct_blocks[pointer_index] = new_block;

        u8 buffer[BLOCK_SIZE];

        //
        // TODO:
        // Zero the entire buffer.
        //
        // memset(buffer,0,BLOCK_SIZE);

        DirectoryEntry* entries =
            reinterpret_cast<DirectoryEntry*>(buffer);

        entries[0].is_used = true;
        entries[0].inode_number = inode_number;
        StringUtils::strcpy(entries[0].name, name);

        if (!block_manager.write_block(new_block, buffer))
            return false;

        return inode_manager.write_inode(directory_inode,
                                         dir_inode);
    }

    bool remove_entry(u32 directory_inode,
                      const char* name)
    {
        if (directory_inode >= TOTAL_INODES)
            return false;

        Inode dir_inode;

        if (!inode_manager.read_inode(directory_inode, dir_inode))
            return false;

        if (!dir_inode.is_directory)
            return false;

        for (u32 i = 0; i < DIRECT_BLOCKS; i++)
        {
            u32 block = dir_inode.direct_blocks[i];

            if (block == INVALID_BLOCK)
                continue;

            u8 buffer[BLOCK_SIZE];

            if (!block_manager.read_block(block, buffer))
                return false;

            DirectoryEntry* entries =
                reinterpret_cast<DirectoryEntry*>(buffer);

            for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++)
            {
                if (!entries[j].is_used)
                    continue;

                if (StringUtils::strcmp(entries[j].name, name) == 0)
                {
                    entries[j].is_used = false;
                    entries[j].inode_number = INVALID_INODE;

                    // Optional:
                    // clear filename here

                    return block_manager.write_block(block,
                                                     buffer);
                }
            }
        }

        return false;
    }

    bool find_entry(u32 directory_inode,
                    const char* name,
                    DirectoryEntry& out)
    {
        if (directory_inode >= TOTAL_INODES)
            return false;

        Inode dir_inode;

        if (!inode_manager.read_inode(directory_inode,
                                      dir_inode))
            return false;

        if (!dir_inode.is_directory)
            return false;

        for (u32 i = 0; i < DIRECT_BLOCKS; i++)
        {
            u32 block = dir_inode.direct_blocks[i];

            if (block == INVALID_BLOCK)
                continue;

            u8 buffer[BLOCK_SIZE];

            if (!block_manager.read_block(block, buffer))
                return false;

            DirectoryEntry* entries =
                reinterpret_cast<DirectoryEntry*>(buffer);

            for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++)
            {
                if (!entries[j].is_used)
                    continue;

                if (StringUtils::strcmp(entries[j].name,
                                        name) == 0)
                {
                    out = entries[j];
                    return true;
                }
            }
        }

        return false;
    }

    bool list_directory(u32 directory_inode)
    {
        if (directory_inode >= TOTAL_INODES)
            return false;

        Inode dir_inode;

        if (!inode_manager.read_inode(directory_inode,
                                      dir_inode))
            return false;

        if (!dir_inode.is_directory)
            return false;

        for (u32 i = 0; i < DIRECT_BLOCKS; i++)
        {
            u32 block = dir_inode.direct_blocks[i];

            if (block == INVALID_BLOCK)
                continue;

            u8 buffer[BLOCK_SIZE];

            if (!block_manager.read_block(block, buffer))
                return false;

            DirectoryEntry* entries =
                reinterpret_cast<DirectoryEntry*>(buffer);

            for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++)
            {
                if (!entries[j].is_used)
                    continue;

                //
                // TODO:
                // Print entries[j].name
                // Print entries[j].inode_number
                //
            }
        }

        return true;
    }
};
