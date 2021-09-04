#pragma once

#include <string>

namespace playground
{
	// Returns true if the string *s* starts with *prefix*
	bool StrStartsWith(const char *prefix, const char *s);

	// Removes the whitespace in the string.
	// Includes: " ", "\n", "\t", "\r".
	// s: The string to remove whitespace from. This will be overwritten.
	void StrRemoveWhitespace(char *s);

	// Removes the whitespace at the beginning and end of the string.
	// Includes: " ", "\n", "\t", "\r".
	// s: The string to remove whitespace from. This will be overwritten.
	void StrRemoveCapWhitespace(char *s);

	// Removes the whitespace at the beginning of the string.
	// Includes: " ", "\n", "\t", "\r".
	// s: The string to remove whitespace from. This will be overwritten.
	void StrRemoveLeadingWhitespace(char *s);

	// Removes the whitespace at the end of the string.
	// Includes: " ", "\n", "\t", "\r".
	// s: The string to remove whitespace from. This will be overwritten.
	void StrRemoveTrailingWhitespace(char *s);

	// Changes the string to be all lower case.
	// @param s - The string to de-capitalize.
	void StrToLowerCase(char *s);

	// Changes the string to be all upper case.
	// @param s - The string to capitalize.
	void StrToUpperCase(char *s);

	void StrToWideChar(wchar_t *buf, size_t n, const char *s);

	bool StrStartsWith(const std::string &s, const std::string &prefix);
	bool StrEndsWith(const std::string &s, const std::string &suffix);
	void StrLTrim(std::string &s);
	void StrRTrim(std::string &s);
	void StrTrim(std::string &s);

	std::wstring StrToWideString(const std::string &s);
}