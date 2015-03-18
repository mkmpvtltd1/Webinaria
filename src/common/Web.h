#pragma once

class Web
{

private:
	Web() { }

public:
	static int DownloadToString(const TCHAR *url, char *buffer, int bufSize);
	static void OpenPage(const TCHAR *url);

public:
	static bool GetCurrentVersion(TCHAR *buffer, int bufSize);
	static void OpenForums();
	static void OpenUpdates();
	static void OpenUploads();

};
