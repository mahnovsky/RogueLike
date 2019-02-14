#include "basic/string.hpp"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb/stb_sprintf.h"

namespace basic
{
	String::String()
		: m_buffer()
	{
	}

	String::String(const char_t* cstr)
		: m_buffer()
	{
		init(cstr);
	}

    String::String(const char_t* cstr, uint32 count)
		: m_buffer()
	{
        basic::memory_size len = str_length( cstr, MAX_LEN ) + 1;

        ASSERT( len >= count );

		m_buffer.init(cstr, count);

        push_cend();
	}

	String::String(const String& other)
		: m_buffer(other.m_buffer)
	{
	}

	String::String(String&& other)
		: m_buffer()
	{
		m_buffer = std::move(other.m_buffer);
	}

	void String::init(const char_t* cstr)
	{
        m_buffer.init(cstr, static_cast<uint32>( str_length(cstr, MAX_LEN) + 1 ) );
	}

	void String::append(const char_t* cstr)
	{
		ASSERT(m_buffer.back() == CSTR_END);

		m_buffer.pop();
        m_buffer.append(cstr, static_cast<uint32>( str_length(cstr, MAX_LEN) + 1) );
	}

	bool String::is_empty() const
	{
		return m_buffer.is_empty();
	}

	const char_t* String::get_cstr() const
	{
		return m_buffer.get_raw();
	}

	String& String::operator=(const String& str)
	{
		init(str.get_cstr());

		return *this;
	}

	String& String::operator=(const char_t* cstr)
	{
		init(cstr);

		return *this;
	}

	String String::operator+(const char_t* cstr) const
	{
		String res(get_cstr());

		res.append(cstr);

        return res;
	}

	void String::operator+=(const char_t* cstr)
	{
		append(cstr);
	}

    char_t String::operator[](uint32 index) const
	{
		ASSERT(index < m_buffer.get_size());

		return m_buffer[index];
	}

    bool String::find_first(uint32& out_index, char_t value, uint32 pos) const
	{
		return m_buffer.find_first(out_index, value, pos);
	}

    bool String::find_last(uint32& out_index, char_t value, uint32 pos) const
	{
        return m_buffer.find_last(out_index, value, pos);
    }

    bool String::ends_of(const String &str)
    {
        uint32 size = str.get_size();

        if( size < get_size() )
        {
            uint32 pos = get_size() - size;

            for( uint32 i = 0; i < size; ++i )
            {
                char_t sym = m_buffer[pos + i];
                char_t sym1 = str.m_buffer[i];
                if( sym != sym1 )
                {
                    return false;
                }
            }

            return true;
        }
        else if( size == get_size() )
        {
            return str == (*this);
        }

        return false;
    }

    String String::get_substr( uint32 from, uint32 to ) const
	{
        ASSERT( from <= to );
        ASSERT( to < get_size() );

        String res( get_cstr() + from, to - from + 1 );

        return res;
	}

    uint32 String::get_size() const
	{
		return m_buffer.get_size();
	}

	void String::split(basic::Vector< String >& out, char_t item) const
	{
        if( is_empty() )
        {
            return;
        }

        uint32 from = 0;
        uint32 next_index = 0;

        while ( from < get_size() )
		{
            if ( !find_first( next_index, item, from ) )
            {
                if( from < (get_size() - 1) )
                {
                    out.push( get_substr( from, get_size() - 1 ) );
                }

				break;
			}

            if ( next_index <= from )
			{
				++next_index;
                from = next_index;
				continue;
			}

            out.push( get_substr( from, next_index - 1 ) );

            from = next_index + 1;
		}
	}

	char_t String::front() const
	{
		return m_buffer.front();
	}

	char_t String::back() const
	{
        uint32 last = last_index();

        return m_buffer[last];
    }

    uint32 String::last_index() const
    {
        ASSERT( get_size() > 1 );

        uint32 last = get_size() - 2;

        return last;
    }

    void String::trim()
    {
        const uint32 size = m_buffer.get_size();
        if(size == 0)
        {
            return;
        }

        uint32 beg = 0;
        while (beg < size)
        {
            if(m_buffer[beg] != ' ')
            {
                break;
            }
            ++beg;
        }

        uint32 end = m_buffer.get_size() - 1;
        while (end > 0)
        {
            if(m_buffer[end] != ' ')
            {
                break;
            }
            --end;
        }

        if( end > beg )
        {
            get_substr( beg, end - beg );
        }
    }

    String String::read_line(char_t* cstr, uint32 max_size)
	{
		String result;
		const char_t r = '\r';
        const char_t n = '\n';
        char_t item = 0;

        for (uint32 i = 0; i < max_size; ++i)
		{
            item = *(cstr + i);

            if (item == n || item == r)
			{
                if( result.is_empty() && (i + 1) < max_size )
                {
                    continue;
                }
				break;
			}

			result.m_buffer.push(item);
		}

        if( item != n && result.is_empty() )
        {
            result.init( cstr );
        }

        result.push_cend();

        return result;
    }

    void String::push_cend()
    {
        if( !is_empty() && m_buffer.back() != CSTR_END )
        {
            m_buffer.push( CSTR_END );
        }
    }

    bool String::format(char_t *buffer, uint32 size, const char* fmt, ...)
    {
        va_list args;
        va_start( args, fmt );

        int result = stbsp_vsnprintf( buffer, static_cast<int>(size), fmt, args );

        va_end( args );

        return result > 0;
    }

	bool operator==(const String& s1, const String& s2)
	{
		if (s1.get_size() != s2.get_size())
		{
			return false;
		}

		return mem_cmp(s1.get_cstr(), s2.get_cstr(), s2.get_size()) == 0;
	}

	bool operator==(const String& s1, const char* s2)
	{
        ASSERT(!s1.is_empty());
		ASSERT(s2 != nullptr);

        uint32 size = static_cast<uint32>( str_length(s2, basic::String::MAX_LEN) + 1 );

		if (s1.get_size() != size)
		{
			return false;
		}

		return mem_cmp(s1.get_cstr(), s2, size) == 0;
	}
}
