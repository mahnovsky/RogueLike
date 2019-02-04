#include "basic/string.hpp"

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

	String::String(const char_t* cstr, size_t count)
		: m_buffer()
	{
		ASSERT(str_length(cstr, MAX_LEN) >= count);
		m_buffer.init(cstr, count);
		if (m_buffer.back() != CSTR_END)
		{
			m_buffer.push(CSTR_END);
		}
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
		m_buffer.init(cstr, str_length(cstr, MAX_LEN) + 1);
	}

	void String::append(const char_t* cstr)
	{
		ASSERT(m_buffer.back() == CSTR_END);

		m_buffer.pop();
		m_buffer.append(cstr, str_length(cstr, MAX_LEN) + 1);
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

		return std::move(res);
	}

	void String::operator+=(const char_t* cstr)
	{
		append(cstr);
	}

	char_t String::operator[](size_t index) const
	{
		ASSERT(index < m_buffer.get_size());

		return m_buffer[index];
	}

	bool String::find_first(size_t& out_index, char_t value, size_t pos) const
	{
		return m_buffer.find_first(out_index, value, pos);
	}

	bool String::find_last(size_t& out_index, char_t value, size_t pos) const
	{
		return m_buffer.find_last(out_index, value, pos);
	}

	String String::get_substr(size_t pos, size_t count) const
	{
		ASSERT((pos + count) <= m_buffer.get_size());

		String res(get_cstr() + pos, count);

		return std::move(res);
	}

	size_t String::get_size() const
	{
		return m_buffer.get_size();
	}

	void String::split(basic::Vector< String >& out, char_t item) const
	{
		size_t pos = 0;
		size_t next_index = MAX_LEN;

		while (pos < get_size())
		{
			if (!find_first(next_index, item, pos))
			{
				size_t count = get_size() - pos;
				if (count > 1 && count == get_size() && m_buffer[count - 1] == 0)
				{
					count -= 1;
				}

				out.push(std::move(get_substr(pos, count)));
				break;
			}

			if (next_index == pos)
			{
				++next_index;
				pos = next_index;
				continue;
			}

			out.push(std::move(get_substr(pos, next_index - pos)));

			pos = next_index + 1;
		}
	}

	char_t String::front() const
	{
		return m_buffer.front();
	}

	char_t String::back() const
	{
		return m_buffer.back();
	}

	String String::read_line(char_t* cstr, size_t max_size)
	{
		String result;

		for (size_t i = 0; i < max_size; ++i)
		{
			char_t item = *(cstr + i);
			const char_t next_line = '\n';

			if (item == next_line)
			{
				break;
			}

			result.m_buffer.push(item);
		}

		return std::move(result);
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
		ASSERT(s2 != nullptr);

		size_t size = str_length< char_t >(s2, basic::String::MAX_LEN) + 1;

		if (s1.get_size() != size)
		{
			return false;
		}

		return mem_cmp(s1.get_cstr(), s2, size) == 0;
	}
}