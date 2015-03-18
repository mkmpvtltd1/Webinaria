
#pragma once

// {C8435C85-768B-4b8a-9B1F-8CC83B337D10}
DEFINE_GUID(IID_IUScreenCaptureSettings, 0xc8435c85, 0x768b, 0x4b8a, 0x9b, 0x1f, 0x8c, 0xc8, 0x3b, 0x33, 0x7d, 0x10);

[uuid("C8435C85-768B-4b8a-9B1F-8CC83B337D10")]
DECLARE_INTERFACE_(IUScreenCaptureSettings, IUnknown) {

    STDMETHOD(get_Region) (THIS_
                int * pnLeftTopX, int * pnLeftTopY, 
				int * pnRightBottomX, int * pnRightBottomY        /* [out] */    // the current size
             ) PURE;

    STDMETHOD(put_Region) (THIS_
                int nLeftTopX, int nLeftTopY, 
				int nRightBottomX, int nRightBottomY 			  /* [in] */     // Change to this size
             ) PURE;

    STDMETHOD(get_FramesPerSec) (THIS_
                 int * pnFramesPerSec			/* [out] */   // the current FrameRate
             ) PURE;                                          // Valid range: 1-30

    STDMETHOD(put_FramesPerSec) (THIS_
                  int  nFramesPerSec			/* [in] */    // Change to this FrameRate
             ) PURE;                                          // Valid range: 1-30
};



