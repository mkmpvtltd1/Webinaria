#pragma once

#include "Error.h"
#include "Parameters.h"
#include "iRGBFilters.h"
#include "FileQualitySelector.h"
#include "Timeline.h"

namespace EditorApplication
{
	namespace EditorLogical
	{
#pragma pack (push,1)
		struct WE_WAVEFORMATEX : public WAVEFORMATEX
		{
			BYTE buf[128];
			WE_WAVEFORMATEX(void)
			{
				ZeroMemory(this, sizeof(WE_WAVEFORMATEX));
				cbSize = sizeof(buf);
			};
		};
#pragma pack (pop)
		class CFLVConverter
		{            
			static DWORD         dwRotReg;
			static IGraphBuilder * pGB;
			static IMediaControl * pMC; // Play handling interface
			static IMediaSeeking * pMS;
			static IMediaEventEx * pME;
			static IBaseFilter * pSource;
			static IBaseFilter * pAVISplitter;
			static IBaseFilter * pAdder;
			static IAdder * pAdderSettings;
			static IBaseFilter * pLogo;
			static IBaseFilter * pCompressor;
			static IBaseFilter * pAVIMux;
			static IBaseFilter * pWriter;

			static wchar_t * DistFileName;
			static wchar_t * tmpAVI;
			static WE_WAVEFORMATEX m_wfx;
			static LONGLONG m_llVideoFramesCount;
			static LONGLONG m_llAudioSamplesCount;

		public:

			CFLVConverter(void);

			virtual ~CFLVConverter(void);

			static bool PreSaveToFLV(	const HWND & hMainWnd,
										const CParameters & param,
										const wchar_t * SourceFileName,
										IMediaEventEx ** pMEE,
                                        Timeline *  timeline);

			static bool PostSaveToFLV(int iBitrate);

			static void ReleaseDirectShow();
			static void Stop();
			static LONGLONG GetDuration(void);

			static FileQuality SelectedFileQuality;

		protected:
			static bool ConvertionShell(const wchar_t * Source, const wchar_t * Dist, int iBitrate);
			static bool OpenFileDialog(const HWND hMainWnd, const wchar_t * AVI, wchar_t * FLV);
			static HRESULT CreateVideoCompressor(IBaseFilter** ppFilter);
			static HRESULT CreateAudioCompressor(WAVEFORMATEX* pWfx, IBaseFilter** ppFilter);
			static HRESULT AssembeAudioBranch(const CParameters & param, IPin* pSrc, IPin* pDest);
		};
	}
}