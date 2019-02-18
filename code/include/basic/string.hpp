#pragma once

#include "debug.hpp"
#include "vector.hpp"

namespace basic
{
using char_t = char;

class String
{
public:
    static const uint32 MAX_LEN = Vector<char_t>::MAX_LEN;
    static const char CSTR_END = 0;

	String();
	String(const char_t* cstr);
    String(const char_t* cstr, uint32 count);
	String(const String& other);
	String(String&& other);

	void init(const char_t* cstr);

	void append(const char_t* cstr);

	void append(const char_t* cstr, uint32 count);

	bool is_empty() const;

	const char_t* get_cstr() const;

	String& operator=(const String& str);

	String& operator=(const char_t* cstr);

	String operator+(const char_t* cstr) const;

	void operator+=(const char_t* cstr);

	void operator+=(const String& str);

    char operator[](uint32 index) const;

    bool find_first(uint32& out_index, char_t value, uint32 pos = 0) const;

    bool find_last(uint32& out_index, char_t value, uint32 pos = MAX_LEN) const;

    bool ends_of( const String& str );

    String get_substr(uint32 from, uint32 to) const;

    uint32 get_size() const;

	void split(Vector< String >& out, char_t item) const;

    template < class Type, class Convert >
	void split_to(basic::Vector< Type >& out, char_t item, Convert convert_func) const
    {
		Vector<String> splits;
        split(splits, item);

        for (uint32 i = 0; i < splits.get_size(); ++i)
        {
            out.push( convert_func(std::move(splits[i])) );
        }
    }

	char_t front() const;

	char_t back() const;

    uint32 last_index() const;

    void trim();

    static uint32 read_line(char_t* cstr, uint32 max_size, String& out);

    static bool format( char_t* buffer, uint32 size, const char* const fmt, ... );

private:
    void push_cend();

private:
    Vector< char_t > m_buffer;
};

bool
operator==(const String& s1, const String& s2);

bool
operator==(const String& s1, const char* s2);

String operator+(const String& s1, const String& s2);

}
