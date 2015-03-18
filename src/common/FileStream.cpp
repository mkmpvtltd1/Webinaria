#include "StdAfx.h"
#include "FileStream.h"

#include <assert.h>
#include <memory.h>

FileStream::FileStream()
	: file(INVALID_HANDLE_VALUE)
{
}

FileStream::~FileStream()
{
	Close();
}

bool FileStream::Open(const TCHAR *fileName, DWORD access, DWORD shareMode, DWORD creationDisposition, 
					  DWORD attributes/* = FILE_ATTRIBUTE_NORMAL*/, LPSECURITY_ATTRIBUTES security/* = 0*/)
{
	file = CreateFile(fileName, access, shareMode, security, creationDisposition, attributes, 0);

	return (INVALID_HANDLE_VALUE != file);
}

void FileStream::Close()
{
	if (INVALID_HANDLE_VALUE != file)
	{
		FlushFileBuffers(file);
		CloseHandle(file);
		file = INVALID_HANDLE_VALUE;
	}
}

bool FileStream::ReadData(short type, void **data, size_t *dataSize)
{
	assert(dataSize);

	if (INVALID_HANDLE_VALUE == file)
		return false;

	DWORD read = 0;

	short t = 0;
	if (ReadFile(file, &t, sizeof(t), &read, 0) && read == sizeof(t) && t == type)
	{
		size_t ds = 0;
		if (ReadFile(file, &ds, sizeof(ds), &read, 0) && read == sizeof(ds))
		{
			if (*data)
			{
				if (ds != *dataSize)
					return false;

				return (ReadFile(file, *data, *dataSize, &read, 0) && read == *dataSize);
			}
			else
			{
				*data = calloc(ds, 1);

				if (!ReadFile(file, *data, ds, &read, 0) || read != ds)
				{
					free(*data);
					*data = 0;
					*dataSize = 0;

					return false;
				}

				*dataSize = ds;

				return true;
			}
		}
	}

	return false;
}

bool FileStream::WriteData(short type, const void *data, size_t dataSize)
{
	if (INVALID_HANDLE_VALUE == file)
		return false;

	DWORD written = 0;

	if (WriteFile(file, &type, sizeof(type), &written, 0) && written == sizeof(type))
	{
		if (WriteFile(file, &dataSize, sizeof(dataSize), &written, 0) && written == sizeof(dataSize))
		{
			return (WriteFile(file, data, dataSize, &written, 0) && written == dataSize);
		}
	}

	return false;
}

bool FileStream::WriteULong(unsigned long value)
{
	return WriteData(FileStreamData_ULong, &value, sizeof(value));
}

bool FileStream::WriteInt(int value)
{
	return WriteData(FileStreamData_Int, &value, sizeof(value));
}

bool FileStream::WriteShort(short value)
{
	return WriteData(FileStreamData_Short, &value, sizeof(value));
}

bool FileStream::WriteDouble(double value)
{
	return WriteData(FileStreamData_Double, &value, sizeof(value));
}

bool FileStream::WriteWString(const std::wstring *value)
{
	return WriteData(FileStreamData_WString, value->c_str(), value->length() * sizeof(std::wstring::value_type));
}

FileStream& FileStream::operator<<(int value)
{
	if (!WriteInt(value))
		throw EFileStreamWriteFailure();

	return *this;
}

FileStream& FileStream::operator<<(unsigned long value)
{
	if (!WriteULong(value))
		throw EFileStreamWriteFailure();

	return *this;
}

FileStream& FileStream::operator<<(short value)
{
	if (!WriteShort(value))
		throw EFileStreamWriteFailure();

	return *this;
}

FileStream& FileStream::operator<<(double value)
{
	if (!WriteDouble(value))
		throw EFileStreamWriteFailure();

	return *this;
}

FileStream& FileStream::operator<<(std::wstring value)
{
	if (!WriteWString(&value))
		throw EFileStreamWriteFailure();

	return *this;
}

bool FileStream::ReadInt(int *value)
{
	size_t size = sizeof(*value);
	return ReadData(FileStreamData_Int, reinterpret_cast<void**>(&value), &size);
}

bool FileStream::ReadULong(unsigned long *value)
{
	size_t size = sizeof(*value);
	return ReadData(FileStreamData_ULong, reinterpret_cast<void**>(&value), &size);
}

bool FileStream::ReadShort(short *value)
{
	size_t size = sizeof(*value);
	return ReadData(FileStreamData_Short, reinterpret_cast<void**>(&value), &size);
}

bool FileStream::ReadDouble(double *value)
{
	size_t size = sizeof(*value);
	return ReadData(FileStreamData_Double, reinterpret_cast<void**>(&value), &size);
}

bool FileStream::ReadWString(std::wstring *value)
{
	size_t size = 0;
	char *data = 0;

	if (ReadData(FileStreamData_WString, reinterpret_cast<void**>(&data), &size))
	{
		value->assign(reinterpret_cast<wchar_t*>(data), size / sizeof(wchar_t));
		return true;
	}

	return false;
}

FileStream& FileStream::operator>>(int &value)
{
	if (!ReadInt(&value))
		throw EFileStreamReadFailure();

	return *this;
}

FileStream& FileStream::operator>>(unsigned long &value)
{
	if (!ReadULong(&value))
		throw EFileStreamReadFailure();

	return *this;
}

FileStream& FileStream::operator>>(short &value)
{
	if (!ReadShort(&value))
		throw EFileStreamReadFailure();

	return *this;
}

FileStream& FileStream::operator>>(double &value)
{
	if (!ReadDouble(&value))
		throw EFileStreamReadFailure();

	return *this;
}

FileStream& FileStream::operator>>(std::wstring &value)
{
	if (!ReadWString(&value))
		throw EFileStreamWriteFailure();

	return *this;
}
