/*
 * Terra Informatica Lightweight Embeddable HTMLayout control
 * http://terrainformatica.com/htmlayout
 * 
 * HTMLayout value class. 
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) 2003-2004, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/**\file
 * \brief value, aka variant, aka discriminated union
 **/

#ifndef __htmlayout_value_hpp__
#define __htmlayout_value_hpp__

#pragma warning(disable:4786) //identifier was truncated...

#include <assert.h>
#include <map>
#include <string>
#include <vector>

// DISCLAIMER: this code is using primitives from std:: namespace probably 
// in the not efficient manner. (I am not using std:: anywhere beside of this file)
// Feel free to change it. Would be nice if you will drop me your updates.

#pragma once


/**HTMLayout namespace.*/
namespace htmlayout
{
  class named_value_t;

  /// \class value_t
  /// \brief The value type, can hold values of bool, int, double, string(wchar_t) and array(value_t)
  /// value_t is also known as JSON value, see: http://json.org/

  class value_t 
  {
  public:

    /// Type of the value.
    typedef enum VALUE_TYPE { 
      V_UNDEFINED = 0,  ///< empty
      V_BOOL = 1,       ///< bool
      V_INT = 2,        ///< int
      V_REAL = 3,       ///< double
      V_STRING = 4,     ///< string of wchar_t
      V_ARRAY = 5,      ///< array of value_t elements
      V_MAP = 6         ///< map of name/value pairs - simple map
    };

    typedef unsigned char byte;

    value_t()                            :v_type(V_UNDEFINED) { data.l_val = 0; }
    value_t(const value_t& src) :v_type(V_UNDEFINED) { set(src); }
    
    /// constructor
    explicit value_t(bool v)             :v_type(V_UNDEFINED) { set(v); }
    /// constructor
    explicit value_t(int v)              :v_type(V_UNDEFINED) { set(v); }
    /// constructor
    explicit value_t(double v)           :v_type(V_UNDEFINED) { set(v); }
    /// constructor
    explicit value_t(const byte* utf8)   :v_type(V_UNDEFINED) { set(utf8); }
    /// constructor
    explicit value_t(const wchar_t* psz) :v_type(V_UNDEFINED) { set(psz); }
    /// constructor
    explicit value_t(const value_t* v, size_t n) :v_type(V_UNDEFINED) { set(v,n); }
    /// constructor
    explicit value_t(const std::wstring& s) :v_type(V_UNDEFINED) { set(s.c_str()); }

    /// current type
    VALUE_TYPE  type() const  { return v_type; }

    /// clear it and free resources
    inline void  clear();

    /// set new value
    inline void  set(const value_t& src); 

    /// assign utf8 encoded string
    void  set(const byte* utf8) { clear(); if( utf8 && utf8[0] ) { v_type = V_STRING; data.s_val = decode_utf8(utf8); } }
    /// assign string
    void  set(const wchar_t* src) { clear(); if( src && src[0] ) { v_type = V_STRING; data.s_val = string_data::allocate(src); } }
    /// assign int
    void  set(int i)    { clear(); data.i_val = i; v_type = V_INT; }
    /// assign bool
    void  set(bool b)   { clear(); data.i_val = (int)b; v_type = V_BOOL; }
    /// assign float
    void  set(double r) { clear(); data.r_val = r; v_type = V_REAL; }
    /// assign vector of values
    void  set(const value_t* v, size_t n) { clear(); if( v && n ) { v_type = V_ARRAY; data.a_val = allocate_array(v,n); } }
    /// assign value to key, convert this value to map if needed.
    void  set(const value_t& key, const value_t& value)
    {
      if( !is_map() ) clear(); 
      v2k(key, value);
    }
    
    ~value_t()  { clear(); }

    /// indicator
    bool is_undefined() const { return (v_type == V_UNDEFINED); }
    /// indicator
    bool is_int() const { return (v_type == V_INT); }
    /// indicator
    bool is_real() const { return (v_type == V_REAL); }
    /// indicator
    bool is_bool() const { return (v_type == V_BOOL); }
    /// indicator
    bool is_string() const { return (v_type == V_STRING); }
    /// indicator
    bool is_array() const { return (v_type == V_ARRAY); }
    /// indicator
    bool is_map() const { return (v_type == V_MAP); }

    value_t& operator = (const wchar_t* ps) { set(ps); return *this; }
    value_t& operator = (const byte* utf8) { set(utf8); return *this; }
    value_t& operator = (int v) { set(v); return *this; }
    value_t& operator = (double v) { set(v); return *this; }
    value_t& operator = (bool v) { set(v); return *this; }
    value_t& operator = (const value_t& v) { set(v); return *this; }
    value_t& operator = (const std::vector<value_t>& a) { if(a.size() != 0) set(&a[0], a.size()); return *this; }
    value_t& operator = (const std::wstring& s) { set(s.c_str()); return *this; }

    bool operator == ( const value_t& rs ) const { 
      if(v_type != rs.v_type ) return false;
      if(data.l_val == rs.data.l_val ) return true;
      if( v_type == V_STRING )
        return wcscmp(data.s_val->data(), rs.data.s_val->data()) == 0;
      if( v_type == V_ARRAY )
        return arrays_are_equal(data.a_val, rs.data.a_val);
      return false; 
    }

    bool operator != ( const value_t& rs ) const { return !(operator==(rs)); }

    /// fetcher
    int                 get(int def_val) const { return (v_type == V_INT)? data.i_val: def_val; }
    /// fetcher
    double              get(double def_val) const  { return (v_type == V_REAL)? data.r_val: def_val; }
    /// fetcher
    bool                get(bool def_val) const { return (v_type == V_BOOL)? (data.i_val != 0): def_val; }
    /// fetcher
    const wchar_t*      get(const wchar_t* def_val) const { return (v_type == V_STRING)? data.s_val->data(): def_val; }
    /// get array size and n-th element (if any)        
    int                 size() const { return (v_type == V_ARRAY)? data.a_val[1].get(0): 0; }
    /// get n-th element of the array
    value_t             nth(int n) const { return ( n >= 0 && n < size() )? data.a_val[n+2]: value_t(); }
    /// get value by key in the map
    value_t             k2v(const value_t& k) const;
    /// set value to key in the map
    void                v2k(const value_t& k, const value_t& v);


    // Joel's rewording of the gets above
    /// converter
    operator bool()           { assert(v_type == V_BOOL); return get( false ); }
    /// converter
    operator int()            { assert(v_type == V_INT); return get( 0 ); }
    /// converter
    operator double()         { assert(v_type == V_REAL); return get( 0.0 ); }
    /// converter
    operator const wchar_t*() { assert(v_type == V_STRING); return get((const wchar_t*)0); }
    /// indexer
    value_t operator []( int index ) const { assert(v_type == V_ARRAY); if(v_type == V_ARRAY) return  nth( index ); return value_t(); }
    /// map read access
    value_t operator []( const value_t& key ) const { assert(v_type == V_MAP); if(v_type == V_MAP) return  k2v( key ); return value_t(); }


    

    /// converter
    std::wstring to_string() const
    {
      switch(v_type)
      {
        case V_UNDEFINED: return L"{undefined}";
        case V_BOOL:      return get(false)? L"true": L"false";
        case V_INT:       { wchar_t buf[128]; swprintf(buf, L"%d", get(0)); return buf; }
        case V_REAL:      { wchar_t buf[128]; swprintf(buf, L"%f", get(0.0)); return buf; }
        case V_STRING:    return get(L"");
        case V_ARRAY:     
          {
            int total = size(); 
            if( total == 0 ) return L"[]";
            std::wstring r = L"[" + nth(0).to_string(); // I've been told that wstring is not good here
            for( int n = 1; n < total; ++n ) { r += L","; r += nth(n).to_string(); } // and here
                                                        // is there something like string_builder/buffer or wstrstream? 
            return r + L"]";
          }
      }
      return std::wstring();
    }
    
    /// converter
    static value_t from_string(const wchar_t* str)
    {
      if(str == 0 || str[0] == 0)
        return value_t(); // undefined

      //guessing conversion
      const wchar_t* endstr = str + wcslen(str);
      wchar_t* lastptr;
      int i = wcstol(str,&lastptr,10); if(lastptr == endstr) return value_t(i);
      double d = wcstod(str,&lastptr); if(lastptr == endstr) return value_t(d);
      if(wcsicmp(str,L"true")==0) return value_t(true);
      if(wcsicmp(str,L"false")==0) return value_t(false);
      return value_t(str);
    }

  protected:

    struct string_data // ref_counted wchar buffer
    {
      long n_refs;
      void add_ref() { InterlockedIncrement(&n_refs); }
      void release() { if (InterlockedDecrement(&n_refs) <= 0) delete[] (byte*)this; }
      
      static string_data* allocate(const wchar_t* lpsz) { string_data* pd = allocate(wcslen(lpsz)); wcscpy(pd->data(),lpsz); return pd; }
      static string_data* allocate(size_t n) { 
                string_data* pd = (string_data*)new byte[ sizeof(string_data) + sizeof(wchar_t) * (n + 1) ]; 
                pd->data()[n] = 0;
                pd->n_refs = 1; 
                return pd;
              }
      wchar_t* data() const { return (wchar_t *)(this + 1); }
    };

    string_data* decode_utf8( const byte* utf8)
    {
      size_t nu = strlen((const char *)utf8);
      size_t n = ::MultiByteToWideChar(CP_UTF8,0,(const char *)utf8,int(nu),NULL,0);
      string_data* sd = string_data::allocate(n);
      ::MultiByteToWideChar(CP_UTF8,0,(const char *)utf8,int(nu),sd->data(),int(n));
      return sd;
    }
   
    VALUE_TYPE      v_type;
    union data_slot 
    {
      int            i_val;
      double         r_val;
      __int64        l_val;
      string_data*   s_val;
      value_t*       a_val; // array value, contains n+2 elements, first is ref_count, second is size
      named_value_t* m_val; // simple map of name/value pairs.

    } data;

    static value_t* allocate_array( const value_t* values, size_t n )
    {
      value_t* v = new value_t[ 2 + n ];
      v[0] = 1;
      v[1] = int(n);
      for( int i = 2; n; ++i, --n ) v[i] = values[i-2];
      return v;
    }
    static void release_array( value_t* v )
    {
      assert( v && v[0].is_int() );
      int n_refs = v[0].get(0) - 1;
      if( n_refs )
        v[0] = n_refs;
      else
        delete[] v;
    }
    static value_t* assign_array( value_t* v )
    {
      assert( v && v[0].is_int() );
      int n_refs = v[0].get(0) + 1;
      v[0] = n_refs;
      return v;
    }

    static bool arrays_are_equal( const value_t* v1, const value_t* v2 )
    {
      int n1 = v1[1].get(0);
      int n2 = v2[1].get(0);
      if( n1 != n2 ) return false;
      for( int i = 0; i < n1; ++i )
        if( v1[i+2] != v2[i+2] ) return false;
      return true;
    }
  };

  class named_value_t
  {
    value_t           key;
    value_t           val;
    named_value_t*    next;
    long              n_refs;

  public:

    named_value_t(named_value_t* n = 0): next(n), n_refs(1) {}
    ~named_value_t() { assert(n_refs == 0);  if(next) next->release(); }

    void add_ref() { InterlockedIncrement(&n_refs); }
    void release() { if (InterlockedDecrement(&n_refs) <= 0) delete this; }

    static named_value_t* find( named_value_t* root, const value_t& k ) 
      { named_value_t* t = root; while(t) if( t->key == k ) return t; else t = t->next; }

    static bool get( const named_value_t* root, const value_t& k, value_t& v )
      { named_value_t* t = find( const_cast<named_value_t*>(root),k); if( t ) { v = t->val; return true; } return false; }
    
    static void set( named_value_t* &root, const value_t& k, const value_t& v )
      { named_value_t* t = find(root,k); 
        if( !t ) t = new named_value_t(t); 
        t->val = v;
    }

  };

  inline void  value_t::clear() 
    { 
      switch(v_type)
      {
        case V_STRING: data.s_val->release(); break;
        case V_ARRAY:  release_array(data.a_val); break;
        case V_MAP:    data.m_val->release(); break;
      }
      data.l_val = 0; v_type = V_UNDEFINED; 
    }

  inline void value_t::set(const value_t& src) 
    { 
      if( this == &src ) return;
      clear(); 
      v_type = src.v_type; 
      data.l_val = src.data.l_val;
      switch(v_type)
      {
        case V_STRING: data.s_val->add_ref(); break;
        case V_ARRAY:  assign_array(data.a_val); break;
        case V_MAP:    data.m_val->add_ref(); break;
      }
    }

  inline value_t value_t::k2v(const value_t& k) const
    {
      value_t v; named_value_t::get( data.m_val, k, v);
      return v;
    }

  inline void value_t::v2k(const value_t& k, const value_t& v)
    {
      named_value_t::set( data.m_val, k, v);
    }



}

#endif