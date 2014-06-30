// Copyright 2014 Gordonlee

#include "common_header/base_header.h"
// FIXME: remove stdio.h
#include <stdio.h>

// FIXME: Use interface later, so that buffer changes other concept.
class IBuffer {
};

const int BUFFER_SIZE = 512;
// FIXME: Naming later.
class Buffer {
 public:
    Buffer(void) {
        ::memset(m_Buffer, 0, BUFFER_SIZE);
    }

    char* GetPtr(void) {
        return reinterpret_cast<char*>(m_Buffer);
    }

    void Clear(void) {
        ::memset(m_Buffer, 0, BUFFER_SIZE);
    }

 private:
    byte m_Buffer[BUFFER_SIZE];
};
