#pragma once

#include "Types.h"

namespace Util
{
    template <typename T>
    inline void Swap(T& a, T& b)
    {
        T t = a;
        a = b;
        b = t;
    }
    
    template <typename T>
    inline T Min(T a, T b)
    {
        return (a <= b) ? a : b;
    }

    template <typename T>
    inline T Max(T a, T b)
    {
        return (a >= b) ? a : b;
    }

    template <typename T>
    inline T Min3(T a, T b, T c)
    {
        return Min(Min(a, b), c);
    }

    template <typename T>
    inline T Max3(T a, T b, T c)
    {
        return Max(Max(a, b), c);
    }

    template <typename T>
    bool LessThanFunc(const T* v1, const T* v2)
    {
        return *v1 < *v2;
    }

    namespace Internal
    {
        template<typename T> inline void ArraySortSection(T* data, int i, int count, bool(*lessThanFunc)(const T* e1, const T* e2))
        {
            T hold = data[i];

            int k = i * 2 + 1;
            while (k < count)
            {
                if (k + 1 < count && lessThanFunc(&data[k], &data[k + 1]))
                {
                    ++k;
                }

                if (!lessThanFunc(&hold, &data[k]))
                {
                    break;
                }

                data[i] = data[k];
                i = k;
                k = i * 2 + 1;
            }

            data[i] = hold;
        }
    }

    template <typename T>
    void ArraySort(T* data, int count, bool(*lessThanFunc)(const T* v1, const T* v2) = LessThanFunc)
    {
        int i = count / 2;
        while (i-- > 0)
        {
            Internal::ArraySortSection(data, i, count, lessThanFunc);
        }

        while (--count > 0)
        {
            Swap(&data[0], &data[count]);
            Internal::ArraySortSection(data, 0, count, lessThanFunc);
        }
    }
}