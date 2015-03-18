

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Mar 18 12:53:04 2015
 */
/* Compiler settings for RGBFilters.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __iRGBFilters_h__
#define __iRGBFilters_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAdder_FWD_DEFINED__
#define __IAdder_FWD_DEFINED__
typedef interface IAdder IAdder;

#endif 	/* __IAdder_FWD_DEFINED__ */


#ifndef __IFrameOverlay_FWD_DEFINED__
#define __IFrameOverlay_FWD_DEFINED__
typedef interface IFrameOverlay IFrameOverlay;

#endif 	/* __IFrameOverlay_FWD_DEFINED__ */


#ifndef __IOverlaysEnum_FWD_DEFINED__
#define __IOverlaysEnum_FWD_DEFINED__
typedef interface IOverlaysEnum IOverlaysEnum;

#endif 	/* __IOverlaysEnum_FWD_DEFINED__ */


#ifndef __ITimeline_FWD_DEFINED__
#define __ITimeline_FWD_DEFINED__
typedef interface ITimeline ITimeline;

#endif 	/* __ITimeline_FWD_DEFINED__ */


#ifndef __ITimelineFilter_FWD_DEFINED__
#define __ITimelineFilter_FWD_DEFINED__
typedef interface ITimelineFilter ITimelineFilter;

#endif 	/* __ITimelineFilter_FWD_DEFINED__ */


#ifndef __IFrameProcessCallback_FWD_DEFINED__
#define __IFrameProcessCallback_FWD_DEFINED__
typedef interface IFrameProcessCallback IFrameProcessCallback;

#endif 	/* __IFrameProcessCallback_FWD_DEFINED__ */


#ifndef __IAudioSkip_FWD_DEFINED__
#define __IAudioSkip_FWD_DEFINED__
typedef interface IAudioSkip IAudioSkip;

#endif 	/* __IAudioSkip_FWD_DEFINED__ */


#ifndef __INotesAdder_FWD_DEFINED__
#define __INotesAdder_FWD_DEFINED__
typedef interface INotesAdder INotesAdder;

#endif 	/* __INotesAdder_FWD_DEFINED__ */


#ifndef __IParams_FWD_DEFINED__
#define __IParams_FWD_DEFINED__
typedef interface IParams IParams;

#endif 	/* __IParams_FWD_DEFINED__ */


#ifndef __ILogoFilter_FWD_DEFINED__
#define __ILogoFilter_FWD_DEFINED__
typedef interface ILogoFilter ILogoFilter;

#endif 	/* __ILogoFilter_FWD_DEFINED__ */


#ifndef __CWebinariaLogoFilter_FWD_DEFINED__
#define __CWebinariaLogoFilter_FWD_DEFINED__

#ifdef __cplusplus
typedef class CWebinariaLogoFilter CWebinariaLogoFilter;
#else
typedef struct CWebinariaLogoFilter CWebinariaLogoFilter;
#endif /* __cplusplus */

#endif 	/* __CWebinariaLogoFilter_FWD_DEFINED__ */


#ifndef __CRemoveBackground_FWD_DEFINED__
#define __CRemoveBackground_FWD_DEFINED__

#ifdef __cplusplus
typedef class CRemoveBackground CRemoveBackground;
#else
typedef struct CRemoveBackground CRemoveBackground;
#endif /* __cplusplus */

#endif 	/* __CRemoveBackground_FWD_DEFINED__ */


#ifndef __CAdder_FWD_DEFINED__
#define __CAdder_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAdder CAdder;
#else
typedef struct CAdder CAdder;
#endif /* __cplusplus */

#endif 	/* __CAdder_FWD_DEFINED__ */


#ifndef __CNotesAdder_FWD_DEFINED__
#define __CNotesAdder_FWD_DEFINED__

#ifdef __cplusplus
typedef class CNotesAdder CNotesAdder;
#else
typedef struct CNotesAdder CNotesAdder;
#endif /* __cplusplus */

#endif 	/* __CNotesAdder_FWD_DEFINED__ */


#ifndef __CAudioSkipper_FWD_DEFINED__
#define __CAudioSkipper_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAudioSkipper CAudioSkipper;
#else
typedef struct CAudioSkipper CAudioSkipper;
#endif /* __cplusplus */

#endif 	/* __CAudioSkipper_FWD_DEFINED__ */


#ifndef __CTimeRemover_FWD_DEFINED__
#define __CTimeRemover_FWD_DEFINED__

#ifdef __cplusplus
typedef class CTimeRemover CTimeRemover;
#else
typedef struct CTimeRemover CTimeRemover;
#endif /* __cplusplus */

#endif 	/* __CTimeRemover_FWD_DEFINED__ */


#ifndef __CSynchronizer_FWD_DEFINED__
#define __CSynchronizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSynchronizer CSynchronizer;
#else
typedef struct CSynchronizer CSynchronizer;
#endif /* __cplusplus */

#endif 	/* __CSynchronizer_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __RmBkFilter_LIBRARY_DEFINED__
#define __RmBkFilter_LIBRARY_DEFINED__

/* library RmBkFilter */
/* [version][uuid] */ 

typedef struct _FrameInfo
    {
    LONG lFrame;
    HDC hdc;
    RECT rect;
    } 	FRAME_INFO;


DEFINE_GUID(LIBID_RmBkFilter,0x56ECA530,0x1D00,0x43d0,0x94,0x65,0x4D,0x7E,0x3B,0xBD,0x36,0x1A);

#ifndef __IAdder_INTERFACE_DEFINED__
#define __IAdder_INTERFACE_DEFINED__

/* interface IAdder */
/* [unique][helpstring][uuid][object] */ 


DEFINE_GUID(IID_IAdder,0x5EE985B0,0x4AC4,0x41aa,0xA6,0xA3,0xCB,0x4A,0xF0,0x9F,0x21,0xB6);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5EE985B0-4AC4-41aa-A6A3-CB4AF09F21B6")
    IAdder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetIntervals( 
            void *MainIntervals,
            void *WebIntervals,
            void *WebPosIntervals,
            void *ArrowIntervals,
            void *TextIntervals) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThumbnailPath( 
            BSTR path) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAdderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAdder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAdder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAdder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetIntervals )( 
            IAdder * This,
            void *MainIntervals,
            void *WebIntervals,
            void *WebPosIntervals,
            void *ArrowIntervals,
            void *TextIntervals);
        
        HRESULT ( STDMETHODCALLTYPE *SetThumbnailPath )( 
            IAdder * This,
            BSTR path);
        
        END_INTERFACE
    } IAdderVtbl;

    interface IAdder
    {
        CONST_VTBL struct IAdderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAdder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAdder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAdder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAdder_SetIntervals(This,MainIntervals,WebIntervals,WebPosIntervals,ArrowIntervals,TextIntervals)	\
    ( (This)->lpVtbl -> SetIntervals(This,MainIntervals,WebIntervals,WebPosIntervals,ArrowIntervals,TextIntervals) ) 

#define IAdder_SetThumbnailPath(This,path)	\
    ( (This)->lpVtbl -> SetThumbnailPath(This,path) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAdder_INTERFACE_DEFINED__ */


#ifndef __IFrameOverlay_INTERFACE_DEFINED__
#define __IFrameOverlay_INTERFACE_DEFINED__

/* interface IFrameOverlay */
/* [object][unique][uuid] */ 


DEFINE_GUID(IID_IFrameOverlay,0x7EE71CDF,0x0198,0x4fed,0xB4,0x48,0x94,0x60,0xFA,0x9E,0x45,0x5D);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7EE71CDF-0198-4fed-B448-9460FA9E455D")
    IFrameOverlay : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Draw( 
            /* [in] */ FRAME_INFO *pFrameInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetBoundaries( 
            /* [out] */ LONG *plBegin,
            /* [out] */ LONG *plEnd) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFrameOverlayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFrameOverlay * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFrameOverlay * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFrameOverlay * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Draw )( 
            IFrameOverlay * This,
            /* [in] */ FRAME_INFO *pFrameInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetBoundaries )( 
            IFrameOverlay * This,
            /* [out] */ LONG *plBegin,
            /* [out] */ LONG *plEnd);
        
        END_INTERFACE
    } IFrameOverlayVtbl;

    interface IFrameOverlay
    {
        CONST_VTBL struct IFrameOverlayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFrameOverlay_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFrameOverlay_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFrameOverlay_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFrameOverlay_Draw(This,pFrameInfo)	\
    ( (This)->lpVtbl -> Draw(This,pFrameInfo) ) 

#define IFrameOverlay_GetBoundaries(This,plBegin,plEnd)	\
    ( (This)->lpVtbl -> GetBoundaries(This,plBegin,plEnd) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFrameOverlay_INTERFACE_DEFINED__ */


#ifndef __IOverlaysEnum_INTERFACE_DEFINED__
#define __IOverlaysEnum_INTERFACE_DEFINED__

/* interface IOverlaysEnum */
/* [unique][uuid][object] */ 


DEFINE_GUID(IID_IOverlaysEnum,0xB5B31710,0x4D9E,0x4278,0x9B,0xCF,0x95,0x76,0xBF,0x75,0x8F,0x29);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B5B31710-4D9E-4278-9BCF-9576BF758F29")
    IOverlaysEnum : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Rewind( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNext( 
            /* [out] */ IFrameOverlay **ppOverlay) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IOverlaysEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOverlaysEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOverlaysEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOverlaysEnum * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Rewind )( 
            IOverlaysEnum * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetNext )( 
            IOverlaysEnum * This,
            /* [out] */ IFrameOverlay **ppOverlay);
        
        END_INTERFACE
    } IOverlaysEnumVtbl;

    interface IOverlaysEnum
    {
        CONST_VTBL struct IOverlaysEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOverlaysEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOverlaysEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOverlaysEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOverlaysEnum_Rewind(This)	\
    ( (This)->lpVtbl -> Rewind(This) ) 

#define IOverlaysEnum_GetNext(This,ppOverlay)	\
    ( (This)->lpVtbl -> GetNext(This,ppOverlay) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOverlaysEnum_INTERFACE_DEFINED__ */


#ifndef __ITimeline_INTERFACE_DEFINED__
#define __ITimeline_INTERFACE_DEFINED__

/* interface ITimeline */
/* [object][unique][uuid] */ 


DEFINE_GUID(IID_ITimeline,0x09332D2A,0xABD4,0x4dc9,0x8D,0x30,0x16,0xFE,0x7F,0xC2,0x00,0x94);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("09332D2A-ABD4-4dc9-8D30-16FE7FC20094")
    ITimeline : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetOverlaysEnum( 
            /* [out] */ IOverlaysEnum **ppEnum) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITimelineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITimeline * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITimeline * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITimeline * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetOverlaysEnum )( 
            ITimeline * This,
            /* [out] */ IOverlaysEnum **ppEnum);
        
        END_INTERFACE
    } ITimelineVtbl;

    interface ITimeline
    {
        CONST_VTBL struct ITimelineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITimeline_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITimeline_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITimeline_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITimeline_GetOverlaysEnum(This,ppEnum)	\
    ( (This)->lpVtbl -> GetOverlaysEnum(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimeline_INTERFACE_DEFINED__ */


#ifndef __ITimelineFilter_INTERFACE_DEFINED__
#define __ITimelineFilter_INTERFACE_DEFINED__

/* interface ITimelineFilter */
/* [object][unique][uuid] */ 


DEFINE_GUID(IID_ITimelineFilter,0x73E4BA38,0x506C,0x4f95,0xA3,0xBE,0x5C,0x92,0xC6,0x27,0x7C,0x58);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("73E4BA38-506C-4f95-A3BE-5C92C6277C58")
    ITimelineFilter : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AssignTimeline( 
            /* [in] */ ITimeline *pTimeline) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITimelineFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITimelineFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITimelineFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITimelineFilter * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AssignTimeline )( 
            ITimelineFilter * This,
            /* [in] */ ITimeline *pTimeline);
        
        END_INTERFACE
    } ITimelineFilterVtbl;

    interface ITimelineFilter
    {
        CONST_VTBL struct ITimelineFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITimelineFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITimelineFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITimelineFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITimelineFilter_AssignTimeline(This,pTimeline)	\
    ( (This)->lpVtbl -> AssignTimeline(This,pTimeline) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimelineFilter_INTERFACE_DEFINED__ */


#ifndef __IFrameProcessCallback_INTERFACE_DEFINED__
#define __IFrameProcessCallback_INTERFACE_DEFINED__

/* interface IFrameProcessCallback */
/* [unique][helpstring][uuid][object] */ 


DEFINE_GUID(IID_IFrameProcessCallback,0xD7332619,0xE6A9,0x4bf1,0x96,0xDD,0x0F,0xD2,0x03,0xF0,0xB4,0x7A);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7332619-E6A9-4bf1-96DD-0FD203F0B47A")
    IFrameProcessCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStart( 
            LONG lWidth,
            LONG lHeight,
            LONG lWidthOpt,
            LONG lHeightOpt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnFrame( 
            BYTE *pFrame,
            BYTE *pOptional) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStop( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFrameProcessCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFrameProcessCallback * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFrameProcessCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFrameProcessCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnStart )( 
            IFrameProcessCallback * This,
            LONG lWidth,
            LONG lHeight,
            LONG lWidthOpt,
            LONG lHeightOpt);
        
        HRESULT ( STDMETHODCALLTYPE *OnFrame )( 
            IFrameProcessCallback * This,
            BYTE *pFrame,
            BYTE *pOptional);
        
        HRESULT ( STDMETHODCALLTYPE *OnStop )( 
            IFrameProcessCallback * This);
        
        END_INTERFACE
    } IFrameProcessCallbackVtbl;

    interface IFrameProcessCallback
    {
        CONST_VTBL struct IFrameProcessCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFrameProcessCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFrameProcessCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFrameProcessCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFrameProcessCallback_OnStart(This,lWidth,lHeight,lWidthOpt,lHeightOpt)	\
    ( (This)->lpVtbl -> OnStart(This,lWidth,lHeight,lWidthOpt,lHeightOpt) ) 

#define IFrameProcessCallback_OnFrame(This,pFrame,pOptional)	\
    ( (This)->lpVtbl -> OnFrame(This,pFrame,pOptional) ) 

#define IFrameProcessCallback_OnStop(This)	\
    ( (This)->lpVtbl -> OnStop(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFrameProcessCallback_INTERFACE_DEFINED__ */


#ifndef __IAudioSkip_INTERFACE_DEFINED__
#define __IAudioSkip_INTERFACE_DEFINED__

/* interface IAudioSkip */
/* [unique][helpstring][uuid][object] */ 


DEFINE_GUID(IID_IAudioSkip,0x9A03C590,0x6EAF,0x4541,0x85,0xE8,0x4F,0x1D,0xFB,0x6D,0x9A,0x2A);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9A03C590-6EAF-4541-85E8-4F1DFB6D9A2A")
    IAudioSkip : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetIntervals( 
            void *MainIntervals,
            void *AudioIntervals) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSamplesCount( 
            LONGLONG llFramesCount,
            LONGLONG llSamplesCount) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAudioSkipVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioSkip * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioSkip * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioSkip * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetIntervals )( 
            IAudioSkip * This,
            void *MainIntervals,
            void *AudioIntervals);
        
        HRESULT ( STDMETHODCALLTYPE *SetSamplesCount )( 
            IAudioSkip * This,
            LONGLONG llFramesCount,
            LONGLONG llSamplesCount);
        
        END_INTERFACE
    } IAudioSkipVtbl;

    interface IAudioSkip
    {
        CONST_VTBL struct IAudioSkipVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioSkip_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioSkip_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioSkip_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioSkip_SetIntervals(This,MainIntervals,AudioIntervals)	\
    ( (This)->lpVtbl -> SetIntervals(This,MainIntervals,AudioIntervals) ) 

#define IAudioSkip_SetSamplesCount(This,llFramesCount,llSamplesCount)	\
    ( (This)->lpVtbl -> SetSamplesCount(This,llFramesCount,llSamplesCount) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioSkip_INTERFACE_DEFINED__ */


#ifndef __INotesAdder_INTERFACE_DEFINED__
#define __INotesAdder_INTERFACE_DEFINED__

/* interface INotesAdder */
/* [unique][helpstring][uuid][object] */ 


DEFINE_GUID(IID_INotesAdder,0x908C5E37,0x0286,0x4230,0x9A,0xC9,0x17,0xB7,0x7D,0xD7,0xA2,0x0E);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("908C5E37-0286-4230-9AC9-17B77DD7A20E")
    INotesAdder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetIntervals( 
            void *TextIntervals,
            void *ArrowIntervals) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INotesAdderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INotesAdder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INotesAdder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INotesAdder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetIntervals )( 
            INotesAdder * This,
            void *TextIntervals,
            void *ArrowIntervals);
        
        END_INTERFACE
    } INotesAdderVtbl;

    interface INotesAdder
    {
        CONST_VTBL struct INotesAdderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INotesAdder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INotesAdder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INotesAdder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INotesAdder_SetIntervals(This,TextIntervals,ArrowIntervals)	\
    ( (This)->lpVtbl -> SetIntervals(This,TextIntervals,ArrowIntervals) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INotesAdder_INTERFACE_DEFINED__ */


#ifndef __IParams_INTERFACE_DEFINED__
#define __IParams_INTERFACE_DEFINED__

/* interface IParams */
/* [unique][helpstring][uuid][object] */ 

typedef 
enum Params
    {
        roParamSamplesTotal	= 1,
        roParamSamplesPassed	= 2,
        roParamSampleRate	= 3
    } 	Params;


DEFINE_GUID(IID_IParams,0x48F5FFC2,0xE3C4,0x4c19,0xAF,0xF3,0x8B,0x2F,0x9D,0x27,0x4B,0x86);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("48F5FFC2-E3C4-4c19-AFF3-8B2F9D274B86")
    IParams : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetLong( 
            /* [in] */ LONG lParamId,
            /* [in] */ LONGLONG llParamValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLong( 
            /* [in] */ LONG lParamId,
            /* [out] */ LONGLONG *pllParamValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IParamsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IParams * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IParams * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IParams * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetLong )( 
            IParams * This,
            /* [in] */ LONG lParamId,
            /* [in] */ LONGLONG llParamValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetLong )( 
            IParams * This,
            /* [in] */ LONG lParamId,
            /* [out] */ LONGLONG *pllParamValue);
        
        END_INTERFACE
    } IParamsVtbl;

    interface IParams
    {
        CONST_VTBL struct IParamsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IParams_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IParams_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IParams_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IParams_SetLong(This,lParamId,llParamValue)	\
    ( (This)->lpVtbl -> SetLong(This,lParamId,llParamValue) ) 

#define IParams_GetLong(This,lParamId,pllParamValue)	\
    ( (This)->lpVtbl -> GetLong(This,lParamId,pllParamValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IParams_INTERFACE_DEFINED__ */


#ifndef __ILogoFilter_INTERFACE_DEFINED__
#define __ILogoFilter_INTERFACE_DEFINED__

/* interface ILogoFilter */
/* [unique][helpstring][uuid][object] */ 


DEFINE_GUID(IID_ILogoFilter,0x7E4526E6,0xA509,0x4dd0,0xA7,0xF5,0x49,0x27,0x6B,0x8D,0xBD,0x73);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7E4526E6-A509-4dd0-A7F5-49276B8DBD73")
    ILogoFilter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetLogoPath( 
            BSTR smallLogoPath,
            BSTR largeLogoPath) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILogoFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogoFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogoFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogoFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetLogoPath )( 
            ILogoFilter * This,
            BSTR smallLogoPath,
            BSTR largeLogoPath);
        
        END_INTERFACE
    } ILogoFilterVtbl;

    interface ILogoFilter
    {
        CONST_VTBL struct ILogoFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogoFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogoFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogoFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogoFilter_SetLogoPath(This,smallLogoPath,largeLogoPath)	\
    ( (This)->lpVtbl -> SetLogoPath(This,smallLogoPath,largeLogoPath) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILogoFilter_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_CWebinariaLogoFilter,0xD42D63C8,0x2780,0x4b13,0x8D,0xA9,0x65,0x97,0x79,0x93,0xA6,0xB2);

#ifdef __cplusplus

class DECLSPEC_UUID("D42D63C8-2780-4b13-8DA9-65977993A6B2")
CWebinariaLogoFilter;
#endif

DEFINE_GUID(CLSID_CRemoveBackground,0x345446AC,0xB638,0x40b4,0xBB,0xE5,0x40,0x98,0x02,0x8A,0xA1,0x4B);

#ifdef __cplusplus

class DECLSPEC_UUID("345446AC-B638-40b4-BBE5-4098028AA14B")
CRemoveBackground;
#endif

DEFINE_GUID(CLSID_CAdder,0xC513B782,0x0AB5,0x4e93,0xA9,0xED,0xB2,0x0C,0x2D,0x5C,0x4E,0xD6);

#ifdef __cplusplus

class DECLSPEC_UUID("C513B782-0AB5-4e93-A9ED-B20C2D5C4ED6")
CAdder;
#endif

DEFINE_GUID(CLSID_CNotesAdder,0xA6435C17,0xFF74,0x4788,0xA5,0x03,0x60,0xE2,0x7B,0x0A,0x6D,0x4D);

#ifdef __cplusplus

class DECLSPEC_UUID("A6435C17-FF74-4788-A503-60E27B0A6D4D")
CNotesAdder;
#endif

DEFINE_GUID(CLSID_CAudioSkipper,0x12439433,0xF20A,0x4a58,0x9F,0x95,0xDF,0x33,0x26,0x27,0x17,0xC6);

#ifdef __cplusplus

class DECLSPEC_UUID("12439433-F20A-4a58-9F95-DF33262717C6")
CAudioSkipper;
#endif

DEFINE_GUID(CLSID_CTimeRemover,0xFC2C83DF,0x37CA,0x44ab,0x9A,0xC0,0xBD,0xF1,0xB8,0x87,0x65,0xAB);

#ifdef __cplusplus

class DECLSPEC_UUID("FC2C83DF-37CA-44ab-9AC0-BDF1B88765AB")
CTimeRemover;
#endif

DEFINE_GUID(CLSID_CSynchronizer,0xCD26AE0C,0x4361,0x4e98,0xA8,0x17,0xB0,0x0A,0xF1,0xA5,0xA4,0x56);

#ifdef __cplusplus

class DECLSPEC_UUID("CD26AE0C-4361-4e98-A817-B00AF1A5A456")
CSynchronizer;
#endif
#endif /* __RmBkFilter_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


