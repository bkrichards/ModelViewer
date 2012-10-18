#pragma once
#ifndef __BKR_UTILS__
#define __BKR_UTILS__

#include <string.h>
namespace Utils {
   void copy(float* ary, float* src);
   void copy(float* ary, float r,  float g, float b, float a);
   void copy(float* ary, float r,  float g, float b);
}

// Functor for deleting pointers in vector.
template<class T> class DeleteVector
{
    public:
    // Overloaded () operator.
    // This will be called by for_each() function.
    bool operator()(T x) const
    {
        // Delete pointer.
        delete x;
        return true;
    }
};
#endif