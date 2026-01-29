#include "MyCEHelper.h"
#include "StringHelper.h"
#include <codecvt>
char* getDynimicChars(const char* s)
{
	int length = strlen(s);
	char* newChars = (char*)malloc(sizeof(char) * length);
	if (newChars) strcpy(newChars, s);
	return newChars;


}
std::wstring s2ws(const std::string& s)
{
	std::string curlLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;

	wchar_t* _Dest = new wchar_t[_Dsize];
	size_t i;
	mbstowcs_s(&i, _Dest, _Dsize, _Source, s.size());
	std::wstring result = _Dest;
	delete[] _Dest;
	setlocale(LC_ALL, curlLocale.c_str());
	return result;
}
__int64 powi(__int64 base, int exponent)
{
	__int64 res = 1;
	for (int i = 1; i <= exponent; i++)
	{
		res = res * base;
	}
	return res;
}
__int64 strToHex(char* s)
{
	__int64 strLen = strlen(s);
	__int64 hex = 0x0;
	for (int i = 0; i < strLen; i++)
	{
		char temp = s[i];
		if ((temp >= 48) && (temp <= 57))
		{
			temp = temp - 48;
		}
		else if ((temp >= 97) && (temp <= 102))
		{
			temp = temp - 87;
		}
		else if ((temp >= 65) && (temp <= 70))
		{
			temp = temp - 55;
		}
		else if (temp == 120 && i == 1 && s[0] == '0')
		{
			temp = 0;
		}

		else return 0;
		hex = hex + temp * (powi(16, strLen - 1 - i));
	}
	return hex;
}

std::string GbkToUTF8(const std::string& gbkData)
{
	const char* GBK_LOCALE_NAME = "CHS";   //GBK在windows下的locale name(.936, CHS ), linux下的locale名可能是"zh_CN.GBK"

	std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>>
		conv(new std::codecvt<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
	std::wstring wString = conv.from_bytes(gbkData);    // string => wstring

	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::string utf8str = convert.to_bytes(wString);     // wstring => utf-8

	return utf8str;
}
std::string UTF8ToGbk(const std::string& utf8Data)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wString = conv.from_bytes(utf8Data);    // utf-8 => wstring

	std::wstring_convert<std::codecvt< wchar_t, char, std::mbstate_t>>
		convert(new std::codecvt< wchar_t, char, std::mbstate_t>("CHS"));
	std::string str = convert.to_bytes(wString);     // wstring => string

	return str;
}