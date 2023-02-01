/*
* Tests for Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2022 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestFile.h"

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#endif

#if (defined (__linux__)) || (defined (__APPLE__))
#include <unistd.h>
#include <dirent.h>
#endif

namespace Test
{
    bool FileExists(const String& path)
    {
#if defined(WIN32)
        DWORD fileAttribute = ::GetFileAttributes(path.c_str());
        return (fileAttribute != INVALID_FILE_ATTRIBUTES);
#else
        return (::access(path.c_str(), F_OK) != -1);
#endif
    }

    bool DirectoryExists(const String & path)
    {
#if defined(WIN32)
        DWORD fileAttribute = GetFileAttributes(path.c_str());
        return ((fileAttribute != INVALID_FILE_ATTRIBUTES) &&
            (fileAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0);
#elif defined(__linux__)
        DIR * dir = opendir(path.c_str());
        if (dir != NULL)
        {
            ::closedir(dir);
            return true;
        }
        else
            return false;
#else
        return false;
#endif
    }

    String DirectoryByPath(const String & path)
    {
        size_t pos = path.find_last_of(FolderSeparator());
        if (pos == std::string::npos)
            return path;
        else
            return path.substr(0, pos);
    }

    bool CreatePath(const String & path)
    {
#ifdef WIN32
        return std::system((String("mkdir ") + path).c_str()) == 0;
#else
        return std::system((String("mkdir -p ") + path).c_str()) == 0;
#endif
    }

    bool CreatePathIfNotExist(const String & path, bool file)
    {
        String dir;
        if (file)
        {
            dir = DirectoryByPath(path);
            if (dir == path)
                return true;
        }
        else
        {
            dir = path;
            if (dir.empty())
                return true;
        }
        if (!DirectoryExists(dir))
        {
            if (!CreatePath(dir))
            {
                TEST_LOG_SS(Info, "Can't create directory '" << dir << "' !");
                return false;
            }
        }
        return true;
    }

    bool FileLoad(const char* path, uint8_t** data, size_t* size)
    {
        bool result = false;
        ::FILE* file = ::fopen(path, "rb");
        if (file)
        {
            ::fseek(file, 0, SEEK_END);
            *size = ::ftell(file);
            *data = (uint8_t*)SimdAllocate(*size, SimdAlignment());
            ::fseek(file, 0, SEEK_SET);
            result = ::fread(*data, 1, *size, file) == *size;
            ::fclose(file);
        }
        return result;
    }

    bool FileSave(const uint8_t* data, size_t size, const char* path)
    {
        bool result = false;
        ::FILE* file = ::fopen(path, "wb");
        if (file)
        {
            result = ::fwrite(data, 1, size, file) == size;
            ::fclose(file);
        }
        return result;
    }
}
