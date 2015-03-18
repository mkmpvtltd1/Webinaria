#ifndef __HTMENGINE_AUX_H__
#define __HTMENGINE_AUX_H__

#include <stdio.h>
#include <stdlib.h>

//
// This file is a part of 
// Terra Informatica Lightweight Embeddable HTMEngine control SDK
// Created by Andrew Fedoniouk @ TerraInformatica.com
//

//
// Auxiliary classes and functions
//

#define SIZED_STRUCT( type_name, var_name ) type_name var_name; memset(&var_name,0,sizeof(var_name)); var_name.cbSize = sizeof(var_name)


inline bool streq(const char* s, const char* s1)
{
  if( s && s1 )
    return strcmp(s,s1) == 0;
  return false;
}

inline bool wcseq(const wchar_t* s, const wchar_t* s1)
{
  if( s && s1 )
    return wcscmp(s,s1) == 0;
  return false;
}

inline bool streqi(const char* s, const char* s1)
{
  if( s && s1 )
    return _stricmp(s,s1) == 0;
  return false;
}

inline bool wcseqi(const wchar_t* s, const wchar_t* s1)
{
  if( s && s1 )
    return wcsicmp(s,s1) == 0;
  return false;
}

// helper convertor objects wchar_t to ACP and vice versa
class w2a 
{
  char* buffer;
public:
  explicit w2a(const wchar_t* wstr):buffer(0)
  { 
    if(wstr)
    {
      size_t nu = wcslen(wstr);
      size_t n = ::WideCharToMultiByte(CP_ACP,0,wstr,int(nu),NULL,0,NULL,NULL);
      buffer = new char[n+1];
      ::WideCharToMultiByte(CP_ACP,0,wstr,int(nu),buffer,n,NULL,NULL);
      buffer[n] = 0;
    }
  }
  ~w2a() {  delete[] buffer;  }

  operator const char*() { return buffer; }
};

#ifdef UNICODE
  #define a2t( str ) a2w(str)
#else
  #define a2t( str ) (str)
#endif


class a2w 
{
  wchar_t* buffer;
public:
  explicit a2w(const char* str):buffer(0)
  { 
    if(str)
    {
      size_t nu = strlen(str);
      size_t n = ::MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),NULL,0);
      buffer = new wchar_t[n+1];
      ::MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),buffer,int(n));
      buffer[n] = 0;
    }
  }
  ~a2w() {  delete[] buffer;  }

  operator const wchar_t*() { return buffer; }

};

// helper convertor objects wchar_t to utf8 and vice versa
class utf2w 
{
  wchar_t* buffer;
public:
  explicit utf2w(const unsigned char* utf8):buffer(0)
  { 
    if(utf8)
    {
      size_t nu = strlen((const char*)utf8);
      size_t n = ::MultiByteToWideChar(CP_UTF8,0,(const char *)utf8,int(nu),NULL,0);
      buffer = new wchar_t[n+1];
      ::MultiByteToWideChar(CP_UTF8,0,(const char *)utf8,int(nu),buffer,int(n));
      buffer[n] = 0;
    }
  }
  ~utf2w() {  delete[] buffer;  }

  operator const wchar_t*() { return buffer; }

};

class w2utf 
{
  char* buffer;
public:
  explicit w2utf(const wchar_t* wstr):buffer(0)
  { 
    if(wstr)
    {
      size_t nu = wcslen(wstr);
      size_t n = ::WideCharToMultiByte(CP_UTF8,0,wstr,int(nu),NULL,0,NULL,NULL);
      buffer = new char[n+1];
      ::WideCharToMultiByte(CP_UTF8,0,wstr,int(nu),buffer,n,NULL,NULL);
      buffer[n] = 0;
    }
  }
  ~w2utf() {  delete[] buffer;  }

  operator const unsigned char*() { return (unsigned char*)buffer; }
};


/**aux namespace.**/
namespace aux {

  /** byte_buffer class is an in-memory dynamic buffer implementation.**/
  class byte_buffer 
  {
    unsigned char*  _body;
    size_t          _allocated;
    size_t          _size;

    unsigned char *reserve(size_t size)
    {
      size_t newsize = _size + size;
      if( newsize > _allocated ) 
      {
        _allocated *= 2;
        if(_allocated < newsize) 
           _allocated = newsize;
        unsigned char *newbody = new unsigned char[_allocated];
        memcpy(newbody,_body,_size);
        delete[] _body;
        _body = newbody;
      }
      return _body + _size;
    }

  public:

    byte_buffer():_size(0)        { _body = new unsigned char[_allocated = 256]; }
    ~byte_buffer()                { delete[] _body;  }

    const unsigned char * data()  {  
             if(_size == _allocated) reserve(1); 
             _body[_size] = 0; return _body; }

    size_t length() const         { return _size; }

    void push(unsigned char c)    { *reserve(1) = c; ++_size; }
    void push(const unsigned char *pc, size_t sz) { memcpy(reserve(sz),pc,sz); _size += sz; }

    void clear()                  { _size = 0; }

  };

  /** Integer to string converter.
      Use it as ostream << itoa(234) 
  **/
  class itoa 
  {
    char buffer[38];
  public:
    itoa(int n, int radix = 10)
    { 
      _itoa(n,buffer,radix);
    }
    operator const char*() { return buffer; }
  };

  /** Float to string converter.
      Use it as ostream << ftoa(234.1); or
      Use it as ostream << ftoa(234.1,"pt"); or
  **/
  class ftoa 
  {
    char buffer[64];
  public:
    ftoa(double d, const char* units = "", int fractional_digits = 1)
    { 
      _snprintf(buffer, 64, "%.*f%s", fractional_digits, d, units );
      buffer[63] = 0;
    }
    operator const char*() { return buffer; }
  };

}

namespace utf8
{
  // UTF8 stream

  // class T must have two methods:
  //   void push(unsigned char c)
  //   void push(const unsigned char *pc, size_t sz)
  
  // bool X - true - XML markup character conversion (characters '<','>',etc).
  //          false - no conversion at all. 

  template <class T, bool X = true>
  class ostream_t : public T
  {
  public:
    ostream_t()
    { 
      // utf8 byte order mark
      static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };
      T::push(BOM, sizeof(BOM));
    }

    // intended to handle only ascii-7 strings
    // use this for markup output 
    ostream_t& operator << (const char* str) 
    { 
      T::push((const unsigned char*)str,strlen(str)); return *this; 
    }

    // use UNICODE chars for value output
    ostream_t& operator << (const wchar_t* wstr)
    {
      const wchar_t *pc = wstr;
      for(unsigned int c = *pc; c ; c = *(++pc)) 
      {
        if(X)
          switch(c) 
          {
              case '<': *this << "&lt;"; continue;
              case '>': *this << "&gt;"; continue;
              case '&': *this << "&amp;"; continue;
              case '"': *this << "&quot;"; continue;
              case '\'': *this << "&apos;"; continue;
          }
        if (c < (1 << 7)) {
         T::push (c);
        } else if (c < (1 << 11)) {
         T::push ((c >> 6) | 0xc0);
         T::push ((c & 0x3f) | 0x80);
        } else if (c < (1 << 16)) {
         T::push ((c >> 12) | 0xe0);
         T::push (((c >> 6) & 0x3f) | 0x80);
         T::push ((c & 0x3f) | 0x80);
        } else if (c < (1 << 21)) {
         T::push ((c >> 18) | 0xf0);
         T::push (((c >> 12) & 0x3f) | 0x80);
         T::push (((c >> 6) & 0x3f) | 0x80);
         T::push ((c & 0x3f) | 0x80);
        }
      }
      return *this;
    }
  };

  // raw ASCII/UNICODE -> UTF8 converter 
  typedef ostream_t<aux::byte_buffer,false> ostream;
  // ASCII/UNICODE -> UTF8 converter with XML support
  typedef ostream_t<aux::byte_buffer,true> oxstream;

}

#endif