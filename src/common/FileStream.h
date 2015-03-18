#pragma once

#include <string>

class FileStream
{

protected:
	HANDLE file;

	enum DataType
	{
		FileStreamData_Int = 1,
		FileStreamData_ULong,
		FileStreamData_Short,
		FileStreamData_Double,
		FileStreamData_WString
	};

protected:
	bool ReadData(short type, void **data, size_t *dataSize);
	bool WriteData(short type, const void *data, size_t dataSize);

public:
	FileStream();
	~FileStream();

	bool Open(const TCHAR *fileName, DWORD access, DWORD shareMode, DWORD creationDisposition, 
		DWORD attributes = FILE_ATTRIBUTE_NORMAL, LPSECURITY_ATTRIBUTES security = 0);
	void Close();

public:
	bool ReadInt(int *value);
	bool ReadULong(unsigned long *value);
	bool ReadShort(short *value);
	bool ReadDouble(double *value);
	bool ReadWString(std::wstring *value);

	FileStream& operator>>(int &value);
	FileStream& operator>>(unsigned long &value);
	FileStream& operator>>(short &value);
	FileStream& operator>>(double &value);
	FileStream& operator>>(std::wstring &value);

public:
	bool WriteInt(int value);
	bool WriteULong(unsigned long value);
	bool WriteShort(short value);
	bool WriteDouble(double value);
	bool WriteWString(const std::wstring *value);

	FileStream& operator<<(int value);
	FileStream& operator<<(unsigned long value);
	FileStream& operator<<(short value);
	FileStream& operator<<(double value);
	FileStream& operator<<(std::wstring value);

};

class EFileStreamReadFailure
{
};

class EFileStreamWriteFailure
{
};
