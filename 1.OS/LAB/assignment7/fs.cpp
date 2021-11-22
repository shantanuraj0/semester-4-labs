
// fs.cpp: File System

//including necessary header files 
#include "sfs/fs.h"
#include<bits/stdc++.h>
using namespace std;
 


// Debugging the  file  system 
void FileSystem::debug(Disk *dis) 
{
    Block blo;
    Block blo2;
    uint32_t indir;
    unsigned int inode_block_count;
    std::string direct;
    std::string indirect;

    //reading superblock
    dis->read(0, blo.Data);
 
    printf("SuperBlock:\n");
    if (blo.Super.MagicNumber == MAGIC_NUMBER)
    {
        printf("    magic number is valid\n");
    } 
    else
    {
        printf("    magic number is invalid\n");
    }

    printf("    %u blocks\n"         , blo.Super.Blocks);
    printf("    %u inode blocks\n"   , blo.Super.InodeBlocks);
    printf("    %u inodes\n"         , blo.Super.Inodes);
 
    
    //reading inode blocks
    inode_block_count = blo.Super.InodeBlocks;
 
    
    for (unsigned int k = 0; k < inode_block_count; k++) 
    {
        dis->read(1+k, blo.Data);

        for (unsigned int i = 0; i < INODES_PER_BLOCK; i++)
        {
            direct = "";
            indirect = "";

            if (!blo.Inodes[i].Valid)
            {
                continue;
            }

            for (unsigned int j = 0; j < POINTERS_PER_INODE; j++)
            {
                if (blo.Inodes[i].Direct[j] != 0) 
                {
                    direct += " ";
                    direct += std::to_string(blo.Inodes[i].Direct[j]);
                }
            }

            indir = blo.Inodes[i].Indirect;

            if (indir != 0)
             {
                dis->read(indir, blo2.Data);

                for (unsigned int l = 0; l < POINTERS_PER_BLOCK; l++) 
                {
                    if (blo2.Pointers[l] != 0)
                     {
                        indirect += " ";
                        indirect += std::to_string(blo2.Pointers[l]);
                    }
                }
            }

            printf("Inode %u:\n", i);
            printf("    size: %u bytes\n"    , blo.Inodes[i].Size);
            printf("    direct blocks:%s\n" , direct.c_str());

            if (indirect.length() > 0)
             {
                printf("    indirect block: %u\n", indir);
                printf("    indirect data blocks:%s\n", indirect.c_str());
            }
        }
    }
}
 


//formatting the file system 
bool FileSystem::format(Disk *dis)
 {

    //Checking if mounted or not
    if (dis->mounted()) 
    {
        return false;
    }


    //writing superblock
    Block blo;
    memset(blo.Data,0,dis->BLOCK_SIZE);

    blo.Super.MagicNumber = MAGIC_NUMBER;
    blo.Super.Blocks = dis->size();
    blo.Super.InodeBlocks = (size_t)(((float)dis->size()*0.1)+0.5);
    blo.Super.Inodes = INODES_PER_BLOCK*blo.Super.InodeBlocks;
    dis->write(0, blo.Data);
 
    
    //clearing all the other blocks
    char clea[BUFSIZ] = {0};
    for (size_t p=1; p<blo.Super.Blocks; p++) 
    {
        dis->write(p, clea);
    }
 
    return true;
}
 


//mounting the file system 
bool FileSystem::mount(Disk *disk) 
{
 
    //make user disk is not  mounted
    if (disk->mounted()) 
    {
        return false;
    }
 
    
    //reading superblock
    Block bloc;
    disk->read(0, bloc.Data);
 
    
    //making sure that inodes counts match
    if (bloc.Super.Inodes != bloc.Super.InodeBlocks * INODES_PER_BLOCK) 
    {
        return false;
    }
    

    //checking magic number
    if (bloc.Super.MagicNumber != MAGIC_NUMBER) 
    {
        return false;
    }
 
    
    //making sure that there are blocks present
    if (bloc.Super.Blocks < 0) 
    {
        return false;
    }
 
    
    //checking inode proportion
    if (bloc.Super.InodeBlocks != ceil(.1 * bloc.Super.Blocks)) 
    {
        return false;
    }
 
    
    disk->mount();
 
    
    // Copying the  metadata
    num_blocks = bloc.Super.Blocks;
    num_inode_blocks = bloc.Super.InodeBlocks;
    num_inodes = bloc.Super.Inodes;
    this->disk = disk;
 
    
    //allocating the free block bitmap
    free_bitmap = std::vector<int> (num_blocks,1);
 
    
    //setting all blocks to free initially
    for (uint32_t p = 0; p < num_blocks; p++)
     {
        free_bitmap[p] = 1;
    }
 
    
    //superblock isn't free
    free_bitmap[0] = 0;
 
    

    //inode blocks are'nt free
    for (unsigned int p = 0; p < num_inode_blocks; p++) 
    {
        free_bitmap[1+p] = 0;
    }
 
    
    //TODO: reading inodes to determine that which blocks are free
    for (uint32_t inode_block = 0; inode_block < num_inode_blocks; inode_block++)
     {
        Block b;
        disk->read(1+inode_block,b.Data);

        //reading each inode
        for (uint32_t inode = 0; inode < INODES_PER_BLOCK; inode++)
         {
            // if it is not valid that means it has no blocks
            if (!b.Inodes[inode].Valid) 
            {
                continue;
            }


            uint32_t n_blocks = (uint32_t)ceil(b.Inodes[inode].Size/(double)disk->BLOCK_SIZE);


            //reading all the direct blocks
            for (uint32_t point = 0; point < POINTERS_PER_INODE && point < n_blocks; point++) 
            {
                free_bitmap[b.Inodes[inode].Direct[point]] = 0;
            }
 
            
            //reading the indirect block if necessary
            if (n_blocks > POINTERS_PER_INODE) 
            {
                Block indirect;
                disk->read(b.Inodes[inode].Indirect,indirect.Data);
                free_bitmap[b.Inodes[inode].Indirect] = 0;

                for (uint32_t point = 0; point < n_blocks - POINTERS_PER_INODE; point++)
                {
                    free_bitmap[indirect.Pointers[point]] = 0;
                }
            }
        }
    }
 
    return true;
}
 





//Creating inode 
ssize_t FileSystem::create()
{
 
    //locating the free inode in inode table
    int in_d = -1;

    for (uint32_t inode_block = 0; inode_block < num_inode_blocks; inode_block++)
    {
        Block b;
        disk->read(1+inode_block,b.Data);

        //reading each inode
        for (uint32_t inod = 0; inod < INODES_PER_BLOCK; inod++)
        {
            
            //if it is not valid, it is free to be written
            if (!b.Inodes[inod].Valid) 
            {
                in_d = inod + INODES_PER_BLOCK*inode_block;
                break;
            }
        }

        
        if (in_d != -1) 
        {
            break;
        }
    }
 

    //recording the inode if found
    if (in_d == -1) 
    {
        return -1;
    }
    
    Inode q;
    q.Valid = true;
    q.Size = 0;
    
    for (unsigned int t = 0; t < POINTERS_PER_INODE; t++) 
    {
        q.Direct[t] = 0;
    }
    q.Indirect = 0;
    save_inode(in_d, &q);
 
    return in_d;
}
 


//removing the inode 
bool FileSystem::remove(size_t inum)
{
    Inode no_de;
 
    
    //loading the inode info
    if (!load_inode(inum, &no_de)) 
    {
        return false;
    }
    
    if (no_de.Valid == 0)
    {
        return false;
    }
 
    
    //free the direct blocks
    for (unsigned int p = 0; p < POINTERS_PER_INODE; p++) 
    {
        
        if (no_de.Direct[p] != 0)
        {
            free_bitmap[no_de.Direct[p]] = 1;
            no_de.Direct[p] = 0;
        }
    }
 
    
    //free the indirect blocks
    if (no_de.Indirect != 0) 
    {
        
        free_bitmap[no_de.Indirect] = 1;
        Block b;
        disk->read(no_de.Indirect,b.Data);

        //free the blocks pointed to indirectly
        for (unsigned int p = 0; p < POINTERS_PER_BLOCK; p++) 
        {
            if (b.Pointers[p] != 0) 
            {
                free_bitmap[b.Pointers[p]] = 1;
            }
        }
    }


    //clearing the inode in inode table
    no_de.Indirect = 0;
    no_de.Valid = 0;
    no_de.Size = 0;

    if (!save_inode(inum, &no_de)) 
    {
        return false;
    };
 
    return true;
}
 


//inode stat  
ssize_t FileSystem::stat(size_t inum) 
{
    
    //loading the inode info
    Inode p;

    if (!load_inode(inum,&p) || !p.Valid) 
    {
        return -1;
    }
 
    return p.Size;
}
 




//read from inode 
ssize_t FileSystem::read(size_t inumber, char *data, size_t length, size_t offset) 
{
    
    //loading the inode info
    Inode inode;

    if (!load_inode(inumber, &inode) || offset > inode.Size) 
    {
        return -1;
    }
 
    
    
    //adjusting the length
    length = std::min(length, inode.Size - offset);
 
    uint32_t start_block = offset / disk->BLOCK_SIZE;
 
    
    //reading the block and copying to data; using memcpy
 
    
    //getting the indirect block number if it will need it
    Block indirect;

    
    if ((offset + length) / disk->BLOCK_SIZE > POINTERS_PER_INODE)
    {
        
        //making sure that direct block is allocated
        if (inode.Indirect == 0) 
        {
            return -1;
        }

        disk->read(inode.Indirect,indirect.Data);
    }
 
    
    size_t read = 0;
    for (uint32_t block_num = start_block; read < length; block_num++)
     {
        
        
        //figure out that which block we are reading
        size_t block_to_read;

        if (block_num < POINTERS_PER_INODE) 
        {
            block_to_read = inode.Direct[block_num];
        }
         else
        {
            block_to_read = indirect.Pointers[block_num-POINTERS_PER_INODE];
        }
 
        
        //making sure that block is allocated or not
        if (block_to_read == 0) 
        {
            return -1;
        }
 
        
        //getting the block : from either direct or indirect
        Block b;
        disk->read(block_to_read,b.Data);
        size_t read_offset;
        size_t read_length;
 
        

        //if it is the first block read, have to start from an offset
        //and read either until the end of the block, or the whole request
        if (read == 0)
        {
            read_offset = offset % disk->BLOCK_SIZE;
            read_length = std::min(disk->BLOCK_SIZE - read_offset, length);
        }
         else
        {
            // otherwise, start from the beginning, and read
            // either the whole block or the rest of the request
            read_offset = 0;
            read_length = std::min(disk->BLOCK_SIZE-0, length-read);
        }

        memcpy(data + read, b.Data + read_offset, read_length);
        read += read_length;
    }

    return read;
}
 



//allocating the free block 
ssize_t FileSystem::allocate_free_block() 
{
    int blo = -1;
    for (unsigned int p = 0; p < num_blocks; p++)
     {
        if (free_bitmap[p])
        {
            free_bitmap[p] = 0;
            blo = p;
            break;
        }
    }
 
    
    // need to zero data block if we are allocating one
    if (blo != -1) 
    {
        char data[disk->BLOCK_SIZE];
        memset(data,0,disk->BLOCK_SIZE);
        disk->write(blo,(char*)data);
    }
 
    return blo;
}
 


//writing to inode 
ssize_t FileSystem::write(size_t inumber, char *data, size_t length, size_t offset) 
{
    
    //loading the inode
    Inode inode;

    if (!load_inode(inumber, &inode) || offset > inode.Size) 
    {
        return -1;
    }
 
    
    size_t MAX_FILE_SIZE = disk->BLOCK_SIZE * (POINTERS_PER_INODE*POINTERS_PER_BLOCK);
 
    
    //adjusting the length
    length = std::min(length, MAX_FILE_SIZE-offset);
    
    uint32_t start_block = offset / disk->BLOCK_SIZE;
    Block indirect;
    bool read_indirect = false;
 
    bool modified_inode = false;
    bool modified_indirect = false;
 
    
    //writing into block and copying the data
    size_t written = 0;
    for (uint32_t block_num = start_block; written < length && block_num < POINTERS_PER_INODE + POINTERS_PER_BLOCK; block_num++)
    {
        
        
        // figure out which block we are reading
        size_t block_to_write;
        if (block_num < POINTERS_PER_INODE) 
        {
            
            //allocating the  block if necessary
            if (inode.Direct[block_num] == 0)
            {
                
                ssize_t allocated_block = allocate_free_block();
                
                if (allocated_block == -1)
                 {
                    break;
                }
                
                inode.Direct[block_num] = allocated_block;
                modified_inode = true;
            }

            block_to_write = inode.Direct[block_num];
        } 
        else 
        { 
            //indirect block
            
            
            //allocating the indirect block if necessary
            if (inode.Indirect == 0) 
            {
                ssize_t allocated_block = allocate_free_block();
                if (allocated_block == -1) 
                {
                    return written;
                }
                
                inode.Indirect = allocated_block;
                modified_indirect = true;
            }
 
            
            
            //reading the indirect block if has not been read yet
            if (!read_indirect) 
            {
                disk->read(inode.Indirect,indirect.Data);
                read_indirect = true;
            }
 

            //allocating the  block if necessary
            if (indirect.Pointers[block_num - POINTERS_PER_INODE] == 0)
             {
                ssize_t allocated_block = allocate_free_block();
                
                if (allocated_block == -1)
                 {
                    break;
                }
                
                indirect.Pointers[block_num - POINTERS_PER_INODE] = allocated_block;
                modified_indirect = true;
            }
            
            block_to_write = indirect.Pointers[block_num-POINTERS_PER_INODE];
        }
 
        
        //getting the block -- from either direct or indirect
        size_t write_offset;
        size_t write_length;
 
        
        // if it's the first block written, have to start from an offset
        // and write either until the end of the block, or the whole request
        if (written == 0) 
        {
            write_offset = offset % disk->BLOCK_SIZE;
            write_length = std::min(disk->BLOCK_SIZE - write_offset, length);
        }
         else 
         {

            // otherwise, start from the beginning, and write
            // either the whole block or the rest of the request
            write_offset = 0;
            write_length = std::min(disk->BLOCK_SIZE-0, length-written);
        }
 
       
        char write_buffer[disk->BLOCK_SIZE];
 
       
        // if we are not writing the whole block, need to copy what's there
        if (write_length < disk->BLOCK_SIZE) 
        {
            disk->read(block_to_write,(char*)write_buffer);
        }
 
        
        //copying into the buffer
        memcpy(write_buffer + write_offset, data + written, write_length);
        disk->write(block_to_write,(char*)write_buffer);
        written += write_length;
    }
 
    
    //updating the inode size
    uint32_t new_siz = std::max((size_t)inode.Size, written + offset);
    
    if (new_siz != inode.Size)
     {
        inode.Size = new_siz;
        modified_inode = true;
    }
 
    
    //saving the inode and indirect if necessary
    if (modified_inode) 
    {
        save_inode(inumber,&inode);
    }
     
    
    if (modified_indirect) 
    {
        disk->write(inode.Indirect,indirect.Data);
    }
 
    
    return written;
}
 


//loading the inode 
bool FileSystem::load_inode(size_t inum, Inode *n) 
{
    size_t block_num = 1 + (inum / INODES_PER_BLOCK);
    size_t inode_offset = inum % INODES_PER_BLOCK;
 
    if (inum >= num_inodes) 
    {
        return false;
    }
 
    
    Block blo;
    disk->read(block_num,blo.Data);
 
    *n = blo.Inodes[inode_offset];
 
    return true;
}
 
 

//saving the inode 
bool FileSystem::save_inode(size_t inum, Inode *n ) 
{
 
    size_t block_num = 1 + inum / INODES_PER_BLOCK;
    size_t inode_offset = inum % INODES_PER_BLOCK;
 
    
    if (inum >= num_inodes) 
    {
        return false;
    }
 
    
    Block blo;
    disk->read(block_num,blo.Data);
    blo.Inodes[inode_offset] = *n;
    disk->write(block_num,blo.Data);
 
    return true;
}
 
 