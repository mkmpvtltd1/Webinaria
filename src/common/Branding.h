#pragma once

class Branding
{

private:
	Branding();

	void ReadStringFromRegistry(HKEY key, const TCHAR *valueName, TCHAR **value, const TCHAR *defaultValue);
	bool FileExists(const TCHAR *fileName) const;

    TCHAR* dataFolder;
    TCHAR* homepageUrl;
    TCHAR* helpUrl;
    TCHAR* updateUrl;
    TCHAR* downloadUpdatesUrl;
    TCHAR* publishUrl;
    TCHAR* recorderWindowTitle;
    TCHAR* editorWindowTitle;
    TCHAR mainIconPath[MAX_PATH];
    TCHAR trayIconPath[MAX_PATH];
    TCHAR videoSmallLogoPath[MAX_PATH];
    TCHAR videoLargeLogoPath[MAX_PATH];

public:
	~Branding();

	static Branding* Instance();

public:
    const TCHAR* DataFolder() const { return dataFolder; }
    const TCHAR* HomepageUrl() const { return homepageUrl; }
    const TCHAR* HelpUrl() const { return helpUrl; }
    const TCHAR* UpdateUrl() const { return updateUrl; }
	const TCHAR* DownloadUpdatesUrl() const { return downloadUpdatesUrl; }
    const TCHAR* PublishUrl() const { return publishUrl; }
    const TCHAR* RecorderWindowTitle() const { return recorderWindowTitle; }
    const TCHAR* EditorWindowTitle() const { return editorWindowTitle; }
    const TCHAR* MainIconPath() const { return mainIconPath; }
    const TCHAR* TrayIconPath() const { return trayIconPath; }
    const TCHAR* VideoSmallLogoPath() const { return videoSmallLogoPath; }
    const TCHAR* VideoLargeLogoPath() const { return videoLargeLogoPath; }

	bool IsVideoLogoBranded() const;

};
