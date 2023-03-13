#!/usr/bin/env python3

import os
import sys

class ProffieSafe:
    def __init__(self, filename):
        self.valid = False
        self.f = open(filename, "rb")

        if self.read_uint32() != 0xFF1E5AFE:
            return;
        
        checksum = self.read_uint32()
        iteration = self.read_uint32()
        length = self.read_uint32()

        if self.read_uint32() != 0xFF1E5AFE:
            return;

        if self.read_uint32() != checksum:
            return
        if self.read_uint32() != iteration:
            return
        if self.read_uint32() != length:
            return

        self.f.seek(512)
        c = 0
        for x in range(0, length):
            c = (c * 997 + self.read_uint8()) & 0xFFFFFFFF
        if c != checksum:
            return

        self.valid = True
        self.checksum = checksum
        self.iteration = iteration
        self.length = length

    def read_uint8(self):
        return ord(self.f.read(1))

    def read_uint16(self):
        a = self.read_uint8()
        b = self.read_uint8()
        return a + (b << 8)

    def read_uint32(self):
        a = self.read_uint16()
        b = self.read_uint16()
        return a + (b << 16)

    def read(self):
        self.f.seek(512)
        return self.f.read(self.length)

    def close(self):
        self.f.close()

def main():
    base = "presets"
    if len(sys.argv) > 1:
        file = sys.argv[1]
        ext = file.split('.')[-1]
        if ext in ["ini", "tmp"]:
            base = file[:-4]
        else:
            base = file

    ini_filename = "%s.ini" % base
    tmp_filename = "%s.tmp" % base
    ini = ProffieSafe(ini_filename)
    tmp = ProffieSafe(tmp_filename)
    best = ini
    if tmp.valid:
        if not ini.valid or tmp.iteration > ini.iteration:
            best = tmp
    data = best.read()
    ini.close()
    tmp.close()
    os.rename(ini_filename, "%s.bak" % base);
    os.rename(tmp_filename, "%s.tmp" % base);
    open(ini_filename, "wb").write(data)


if __name__ == "__main__":
    main()
