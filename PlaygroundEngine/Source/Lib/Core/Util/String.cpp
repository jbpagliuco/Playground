#include "String.h"

#include <algorithm>
#include <cstring>
#include <cctype>
#include <codecvt>
#include <locale>

#include "Debug/Assert.h"
#include "OS/OSWin32.h"

namespace playground
{
	bool StrStartsWith(const char *prefix, const char *s)
	{
		size_t prefixLen = strlen(prefix);
		size_t sLen = strlen(s);

		return (prefixLen < sLen) ? strncmp(prefix, s, prefixLen) == 0 : false;
	}

	void StrRemoveWhitespace(char *s)
	{
		char *i = s;
		char *sani = s;

		while (*i != 0) {
			*sani = *i++;
			if (!std::isspace(*sani)) {
				++sani;
			}
		}

		*sani = 0;
	}

	void StrRemoveCapWhitespace(char *s)
	{
		StrRemoveLeadingWhitespace(s);
		StrRemoveTrailingWhitespace(s);
	}

	void StrRemoveLeadingWhitespace(char *s)
	{
		char *i = s;
		char *sani = s;

		while (*i != 0) {
			*sani = *i++;
			if (!std::isspace(*sani)) {
				++sani;
				break;
			}
		}

		// Now just copy the rest of the string
		while (*i != 0) {
			*sani++ = *i++;
		}

		*sani = 0;
	}

	void StrRemoveTrailingWhitespace(char *s)
	{
		char *i = s;
		char *lastNonWhitespaceCharacter = nullptr;
		// Go to the last character
		while (*i != 0) {
			if (!std::isspace(*i)) {
				lastNonWhitespaceCharacter = i;
			}

			++i;
		}

		if (lastNonWhitespaceCharacter) {
			*(lastNonWhitespaceCharacter + 1) = 0;
		}
		else if (s != nullptr) {
			// String was all whitespace
			s[0] = 0;
		}
	}

	void StrToLowerCase(char *s)
	{
		char *i = s;
		while (*i != 0) {
			*i = tolower(*i);
			++i;
		}
	}

	void StrToUpperCase(char *s)
	{
		char *i = s;
		while (*i != 0) {
			*i = toupper(*i);
			++i;
		}
	}

	void StrToWideChar(wchar_t *buf, size_t n, const char *s)
	{
		const size_t len = strlen(s);

		MultiByteToWideChar(CP_UTF8, 0, s, (int)len, buf, (int)n);
		buf[min(n - 1, len)] = 0;
	}


	bool StrStartsWith(const std::string &s, const std::string &prefix)
	{
		return s.size() >= prefix.size() && 0 == s.compare(0, prefix.size(), prefix);
	}

	bool StrEndsWith(const std::string &s, const std::string &suffix)
	{
		return s.size() >= suffix.size() && 0 == s.compare(s.size() - suffix.size(), suffix.size(), suffix);
	}

	void StrLTrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	void StrRTrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	void StrTrim(std::string &s)
	{
		StrLTrim(s);
		StrRTrim(s);
	}

	std::wstring StrToWideString(const std::string &s)
	{
		wchar_t buffer[1024];
		
		const int num = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, STATIC_ARRAY_SIZE(buffer));
		CORE_ASSERT_RETURN_VALUE(num > 0, L"", "Failed to convert string \"%s\" to UTF8. Error code: 0x%X", s.c_str(), GetLastError());
		
		return std::wstring(buffer);
	}

	std::vector<std::string> StrSplit(const std::string& string, const std::string& delimiter)
	{
		std::vector<std::string> values;

		std::string s = string;
		size_t pos;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			values.push_back(s.substr(0, pos));
			s.erase(0, pos + delimiter.length());
		}
		values.push_back(s);

		return values;
	}
}