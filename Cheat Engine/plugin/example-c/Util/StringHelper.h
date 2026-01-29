#pragma once

char* getDynimicChars(const char* s);

std::wstring s2ws(const std::string& s);
__int64 strToHex(char* s);
void InitConsoleWindow(bool switch_on);
std::string GbkToUTF8(const std::string& gbkData);
std::string UTF8ToGbk(const std::string& utf8Data);