/*
 * Minimal compatibility definitions needed by the bundled PowerVR
 * PVRTDecompress.cpp. These integer aliases and clamp macros match the
 * PVRTGlobal.h definitions shipped with the original SDK source.
 */
#ifndef RENUX_PVRTC_REFERENCE_TEXTURE_DEFINITIONS_H_
#define RENUX_PVRTC_REFERENCE_TEXTURE_DEFINITIONS_H_

typedef unsigned char PVRTuint8;
typedef unsigned int PVRTuint32;

const PVRTuint32 ETC_MIN_TEXWIDTH = 4;
const PVRTuint32 ETC_MIN_TEXHEIGHT = 4;

#define PVRT_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define PVRT_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define PVRT_CLAMP(x, low, high) (PVRT_MIN((high), PVRT_MAX((x), (low))))

#endif
