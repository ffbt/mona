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

#ifndef _VIRTIODEVICE_
#define _VIRTIODEVICE_

#include <pci/Pci.h>
#include <monapi/io.h>
#include <drivers/virtio/virtio.h>

class VirtioDevice
{
public:
    VirtioDevice(int irq, int basereg) :
        irq_(irq),
        basereg_(basereg)
    {
    }
    virtual ~VirtioDevice()
    {
    }

    int getIRQ() const
    {
        return irq_;
    }

    int getBaseReg() const
    {
        return basereg_;
    }

    void getConfig(void* dest, int offset, int sizeByte)
    {
        uint8_t* p = (uint8_t*)dest;
        for (int i = 0; i < sizeByte; i++) {
            p[i] = inp8(basereg_ + VIRTIO_PCI_CONFIG + i + offset);
        }
    }

    uint8_t getStatus() const
    {
        return inp8(basereg_ + VIRTIO_PCI_STATUS);
    }

    void reset()
    {
        outp8(basereg_ + VIRTIO_PCI_STATUS, 0);
    }

    bool hasFeature(int feature) const
    {
        return getFeatures() & (1 << feature);
    }

    bool tryActivateQueue(int queueIndex)
    {
        // Select the queue to use.
        outp16(basereg_ + VIRTIO_PCI_QUEUE_SEL, queueIndex);

        // How many descriptors do the queue have?
        const int numberOfDesc = inp16(basereg_ + VIRTIO_PCI_QUEUE_NUM);
        if (numberOfDesc == 0) {
            return M_BAD_INDEX;
        }

        logprintf("numberOfDesc=%d", vring_size(numberOfDesc));

        // already activated?
        if (inp32(basereg_ + VIRTIO_PCI_QUEUE_PFN)) {
            return M_BAD_INDEX;
        }


        return M_OK;
    }

    static VirtioDevice* probe(int type, unsigned int nth = 0)
    {
        PciInf pciInf;
        Pci pci;
        pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, type, nth, &pciInf);

        if (pciInf.isExist) {
            return new VirtioDevice(pciInf.irqLine, pciInf.baseAdress & ~1);
        } else {
            return NULL;
        }
    }

private:
    uint32_t getFeatures() const
    {
        return inp32(basereg_ + VIRTIO_PCI_HOST_FEATURES);
    }

    const int irq_;
    const int basereg_;
};


#endif // _VIRTIODEVICE_