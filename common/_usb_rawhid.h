/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _USBrawhid_h_
#define _USBrawhid_h_

#include "usb_desc.h"
#include "usb_rawhid.h"

#if defined(RAWHID_INTERFACE)

#include <inttypes.h>

// C language implementation
#ifdef __cplusplus
extern "C" {
#endif
// Stream
int _usb_rawhid_read(void);
int _usb_rawhid_peek(void);
// Print
size_t _usb_rawhid_writeval(uint8_t val);
size_t _usb_rawhid_writebuf(const uint8_t *buf, size_t size);
#ifdef __cplusplus
}
#endif

//Stream
int _usb_rawhid_read() {
    return -1;
}

int _usb_rawhid_peek() {
    return -1;
}

// extension of usb_rawhid
size_t _usb_rawhid_writeval(uint8_t val) {
	uint8_t sendbuf[64];
    memset(sendbuf,0, sizeof(sendbuf));
	sendbuf[0] = val;
    usb_rawhid_send(sendbuf, 100);
    return (size_t) 1;
}

size_t _usb_rawhid_writebuf(const uint8_t *buf, size_t size) {
    uint8_t sendbuf[64];
	uint8_t _offset = 0;
	while(size > 64){
		memcpy(sendbuf, buf + _offset, 64);
		usb_rawhid_send(sendbuf, 100);
		_offset += 64;
		size -= 64;
	}
    memset(sendbuf,0, sizeof(sendbuf)); //clear buffer to flush unwanted characters
    memcpy(sendbuf, buf + _offset, size);
    usb_rawhid_send(sendbuf, 100);
    return (size_t) 64;
}



// C++ interface
#ifdef __cplusplus
#include "Stream.h"
#include "Print.h"

class _usb_rawhid_class : public Stream
{
public:
    constexpr _usb_rawhid_class() {}
	int available(void) {return usb_rawhid_available(); }
	int recv(void *buffer, uint16_t timeout) { return usb_rawhid_recv(buffer, timeout); }
	int send(const void *buffer, uint16_t timeout) { return usb_rawhid_send(buffer, timeout); }

    // Stream implementation
    int read(){ return _usb_rawhid_read(); };
    int peek(){ return _usb_rawhid_peek(); };

    // Print implementation
    using Print::write; // pull in write(str) and write(buf, size) from Print
    size_t write(uint8_t val) { _usb_rawhid_writeval(val); return 1;};
	size_t write(const uint8_t *buf, size_t size)
				{ _usb_rawhid_writebuf(buf, size); return size; }

	int availableForWrite() { return 64; }
    virtual void flush();

private:
	//size_t printFloat(double n, uint8_t digits);
	//size_t printNumber(unsigned long n, uint8_t base, uint8_t sign);


};

extern _usb_rawhid_class SerialHID;

#endif // __cplusplus

#endif // RAWHID_INTERFACE

#endif // USBrawhid_h_
