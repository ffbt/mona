/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __FAT_H__
#define __FAT_H__

#include "IStorageDevice.h"
#include "vnode.h"

class FatFileSystem
{
public:
    FatFileSystem(IStorageDevice& dev)
    {
        if (dev.read(0, bootParameters__, SECTOR_SIZE) != M_OK) {
            monapi_fatal("can' read boot parameter block");
        }
        bsbpb_ = (struct bsbpb*)(bootParameters__ + sizeof(struct bs));
        bsxbpb_ = (struct bsxbpb*)(bootParameters__ + sizeof(struct bs) + sizeof(struct bsbpb));

        ASSERT(getBytesPerSector() == 512);
        ASSERT(getSectorsPerCluster() <= 64); // Max 32KB
        ASSERT(getNumberOfFats() == 2);

        logprintf("getSectorsPerCluster=%d", getSectorsPerCluster());
        logprintf("getRootDirectoryCluster=%d", getRootDirectoryCluster());
        logprintf("root entries=%d", little2host16(bsbpb_->rde));

        char buf[SECTOR_SIZE];
        if (dev.read(getSectorsPerCluster() * getRootDirectoryCluster(), buf, SECTOR_SIZE) != M_OK) {
            monapi_fatal("can' read root directory entry");
        }
        for (int i = 0; i < SECTOR_SIZE; i++) {
            logprintf("%c", buf[i]);
        }
    }

    Vnode* getRoot() const
    {
        return NULL;
    }

    // Public for testability
    uint16_t getBytesPerSector() const
    {
        return little2host16(bsbpb_->bps);
    }

private:

    uint8_t getSectorsPerCluster() const
    {
        return bsbpb_->spc;
    }

    uint8_t getNumberOfFats() const
    {
        return bsbpb_->nft;
    }

    uint16_t getFatVersion() const
    {
        return little2host16(bsxbpb_->vers);
    }

    uint16_t getRootDirectoryCluster() const
    {
        return little2host32(bsxbpb_->rdcl);
    }

    uint16_t little2host16(uint8_t* p) const
    {
        return (p[1] << 8) | (p[0]);
    }

    uint32_t little2host32(uint8_t* p) const
    {
        return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
    }

    enum
    {
        SECTOR_SIZE = 512
    };

    struct bs {
        uint8_t jmp[3];            /* bootstrap entry point */
        uint8_t oem[8];            /* OEM name and version */
    };

    // BPB (BIOS Parameter Block)
    struct bsbpb {
        uint8_t bps[2];            /* bytes per sector */
        uint8_t spc;               /* sectors per cluster */
        uint8_t res[2];            /* reserved sectors */
        uint8_t nft;               /* number of FATs */
        uint8_t rde[2];            /* root directory entries */
        uint8_t sec[2];            /* total sectors */
        uint8_t mid;               /* media descriptor */
        uint8_t spf[2];            /* sectors per FAT */
        uint8_t spt[2];            /* sectors per track */
        uint8_t hds[2];            /* drive heads */
        uint8_t hid[4];            /* hidden sectors */
        uint8_t bsec[4];           /* big total sectors */
    };

    // Extended Boot Record
    struct bsxbpb {
        uint8_t bspf[4];           /* big sectors per FAT */
        uint8_t xflg[2];           /* FAT control flags */
        uint8_t vers[2];           /* file system version */
        uint8_t rdcl[4];           /* root directory start cluster */
        uint8_t infs[2];           /* file system info sector */
        uint8_t bkbs[2];           /* backup boot sector */
        uint8_t rsvd[12];          /* reserved */
    };

    uint8_t bootParameters__[SECTOR_SIZE];
    struct bsbpb* bsbpb_;
    struct bsxbpb* bsxbpb_;
};

#endif // __FAT_H__
