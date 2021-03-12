// This is a part of the Objective Grid C++ Library.
// Copyright (C) 1995-1998 Stingray Software, Inc.
// All rights reserved.
//
// This source code is only intended as a supplement to
// the Objective Grid Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding
// the Objective Grid product.
//
// Author: Stefan Hoenig
//

// gxtime.cpp : date and time parsing and formatting
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#include <stddef.h>     /* for size_t */
#include <stdarg.h>     /* for va_arg */
#include <time.h>       /* for struct tm */
#include <ctype.h>

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXDLL_H_
#include "gxdll.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXTIME")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#if _MFC_VER >= 0x0300

// GXIntlStrFtime is obsolete for newer MFC versions
// because strftime already provides the necessary
// fucntionality.
//
// You should call _tsetlocale() in the initialization
// code of your application if you want to support
// locale date/time representatons.
//

size_t AFXAPI GXIntlStrFtime(HINSTANCE, UINT, LPTSTR s, size_t maxs, LPCTSTR f, const struct tm *t)
{
#if defined(_UNICODE) && _MFC_VER < 0x0400
	char astring[64];

	WideCharToMultiByte (CP_ACP, 0, f, -1,
						astring, 64, NULL, NULL);

	return _tcsftime(s, maxs, astring, t);
#else
	return _tcsftime(s, maxs, f, t);
#endif
}

#else

static void AFX_CDECL strfmt(LPTSTR str, LPCTSTR fmt, ...);

#define ADAY_OFFSET     0
#define DAY_OFFSET      7
#define AMONTH_OFFSET   14
#define MONTH_OFFSET    26
#define STD_DATETIME    38
#define STD_DATE        39
#define STD_TIME        40

/**
 *
 * size_t GXIntlStrFtime(HINSTANCE nResourceHandle,
 *                      UINT nStrFTimeResourceID,
 *          LPTSTR str,
 *                      size_t maxs,
 *          LPCTSTR fmt,
 *                      const struct tm *t)
 *
 *      this functions acts much like a sprintf for time/date output.
 *      given a pointer to an output buffer, a format string and a
 *      time, it copies the time to the output buffer formatted in
 *      accordance with the format string.  the parameters are used
 *      as follows:
 *
 *
 *          nResourceHandle should be assigned with AfxGetResourceHandle()
 *          in MFC
 *
 *          nStrFTimeResourceID is the resource ID in your .RC file
 *          where the day abbreviations start. First there must be
 *          seven day abbreviations, then seven full day names, then
 *          month abbreviations and at last full month. It is important
 *          that you have continous resource ids for these names.
 *
 *          str is a pointer to the output buffer, there should
 *          be at least maxs characters available at the address
 *          pointed to by str.
 *
 *          maxs is the maximum number of characters to be copied
 *          into the output buffer, included the '\0' terminator
 *
 *          fmt is the format string.  a percent sign (%) is used
 *          to indicate that the following character is a special
 *          format character.  the following are valid format
 *          characters:
 *
 *              %A      full weekday name (Monday)
 *              %a      abbreviated weekday name (Mon)
 *              %B      full month name (January)
 *              %b      abbreviated month name (Jan)
 *              %c      standard date and time representation
 *              %d      day-of-month (01-31)
 *              %H      hour (24 hour clock) (00-23)
 *              %I      hour (12 hour clock) (01-12)
 *              %j      day-of-year (001-366)
 *              %M      minute (00-59)
 *              %m      month (01-12)
 *              %p      local equivalent of AM or PM
 *              %S      second (00-59)
 *              %U      week-of-year, first day sunday (00-53)
 *              %W      week-of-year, first day monday (00-53)
 *              %w      weekday (0-6, sunday is 0)
 *              %X      standard time representation
 *              %x      standard date representation
 *              %Y      year with century
 *              %y      year without century (00-99)
 *              %Z      timezone name
 *              %%      percent sign
 *
 *      the standard date string is equivalent to:
 *
 *          %a %b %d %Y
 *
 *      the standard time string is equivalent to:
 *
 *          %H:%M:%S
 *
 *      the standard date and time string is equivalent to:
 *
 *          %a %b %d %H:%M:%S %Y
 *
 *      GXIntlStrFtime returns the number of characters placed in the
 *      buffer, not including the terminating \0, or zero if more
 *      than maxs characters were produced.
 *
**/

size_t AFXAPI GXIntlStrFtime(HINSTANCE nResourceHandle, UINT nStrFTimeResourceID, LPTSTR s, size_t maxs, LPCTSTR f, const struct tm *t)
{
	  int w;
	  LPTSTR p;
	  LPTSTR q;
	  LPTSTR r;

	  UINT aday   = nStrFTimeResourceID+ADAY_OFFSET;
	  UINT day    = nStrFTimeResourceID+DAY_OFFSET;
	  UINT amonth = nStrFTimeResourceID+AMONTH_OFFSET;
	  UINT month  = nStrFTimeResourceID+MONTH_OFFSET;
	  UINT ddt    = nStrFTimeResourceID+STD_DATETIME;
	  UINT ddate  = nStrFTimeResourceID+STD_DATE;
	  UINT dtime  = nStrFTimeResourceID+STD_TIME;

	  char buf[128], buf1[128];

	  p = s;
	  q = s + maxs - 1;
	  while ((*f != '\0'))
	  {
			if (*f++ == '%')
			{
				  r = buf;
				  switch (*f++)
				  {
				  case '%' :
						r = "%";
						break;

				  case 'a' :
						LoadString(nResourceHandle, aday+t->tm_wday, buf, sizeof(buf));
						break;

				  case 'A' :
						LoadString(nResourceHandle, day+t->tm_wday, buf, sizeof(buf));
						break;

				  case 'b' :
						LoadString(nResourceHandle, amonth+t->tm_mon, buf, sizeof(buf));
						break;

				  case 'B' :
						LoadString(nResourceHandle, month+t->tm_mon, buf, sizeof(buf));
						break;

				  case 'c' :
						LoadString(nResourceHandle, ddt, buf1, sizeof(buf1));
						GXIntlStrFtime(nResourceHandle, nStrFTimeResourceID, buf, sizeof(buf), buf1, t);
						break;

				  case 'd' :
						strfmt(r,"%2",t->tm_mday);
						break;

				  case 'H' :
						strfmt(r,"%2",t->tm_hour);
						break;

				  case 'I' :
						strfmt(r,"%2",(t->tm_hour%12)?t->tm_hour%12:12);
						break;

				  case 'j' :
						strfmt(r,"%3",t->tm_yday+1);
						break;

				  case 'm' :
						strfmt(r,"%2",t->tm_mon+1);
						break;

				  case 'M' :
						strfmt(r,"%2",t->tm_min);
						break;

				  case 'p' :
						r = (t->tm_hour>11)?"PM":"AM";
						break;

				  case 'S' :
						strfmt(r,"%2",t->tm_sec);
						break;

				  case 'U' :
						w = t->tm_yday/7;
						if (t->tm_yday%7 > t->tm_wday)
							  w++;
						strfmt(r, "%2", w);
						break;

				  case 'W' :
						w = t->tm_yday/7;
						if (t->tm_yday%7 > (t->tm_wday+6)%7)
							  w++;
						strfmt(r, "%2", w);
						break;

				  case 'w' :
						strfmt(r,"%1",t->tm_wday);
						break;

				  case 'x' :
						{
							BOOL bCentury;
							TCHAR chDateSep;
							GXDateFormat DateFormat = GXGetDateFormatFromWinIni(FALSE, bCentury, chDateSep);
							if (bCentury)
							{
								if (DateFormat == gxDDMMYY)
									wsprintf(buf1, _T("%%d%c%%m%c%%Y"), chDateSep, chDateSep);
								else if (DateFormat == gxMMDDYY)
									wsprintf(buf1, _T("%%m%c%%d%c%%Y"), chDateSep, chDateSep);
								else if (DateFormat == gxYYMMDD)
									wsprintf(buf1, _T("%%Y%c%%m%c%%d"), chDateSep, chDateSep);
							}
							else
							{
								if (DateFormat == gxDDMMYY)
									wsprintf(buf1, _T("%%d%c%%m%c%%y"), chDateSep, chDateSep);
								else if (DateFormat == gxMMDDYY)
									wsprintf(buf1, _T("%%m%c%%d%c%%y"), chDateSep, chDateSep);
								else if (DateFormat == gxYYMMDD)
									wsprintf(buf1, _T("%%y%c%%m%c%%d"), chDateSep, chDateSep);
							}

							GXIntlStrFtime(nResourceHandle, nStrFTimeResourceID, buf, sizeof(buf), buf1, t);
						}
						break;

				  case 'X' :
						LoadString(nResourceHandle, dtime, buf1, sizeof(buf1));
						GXIntlStrFtime(nResourceHandle, nStrFTimeResourceID, buf, sizeof(buf), buf1, t);
						break;

				  case 'y' :
						strfmt(r,"%2",t->tm_year%100);
						break;

				  case 'Y' :
						strfmt(r,"%4",t->tm_year+1900);
						break;

				  case 'Z' :
#if _MFC_VER >= 0x0300
						{
							TIME_ZONE_INFORMATION tzi;

// Wind/U - GetTimeZoneInformation not supported on Solaris and on MAC

#if defined(_UNIX_) || defined(_MAC)
							DWORD dwRet = TIME_ZONE_ID_UNKNOWN;
#else
							DWORD dwRet = GetTimeZoneInformation(&tzi);
#endif
							switch (dwRet)
							{
							case TIME_ZONE_ID_UNKNOWN:
								break;
							case TIME_ZONE_ID_STANDARD:
								WideCharToMultiByte(CP_ACP, 0, tzi.StandardName, -1,
									r, sizeof(buf), NULL, NULL);
								break;
							case TIME_ZONE_ID_DAYLIGHT:
								WideCharToMultiByte(CP_ACP, 0, tzi.DaylightName, -1,
									r, sizeof(buf), NULL, NULL);
								break;
							}
						}

#else
						r = (t->tm_isdst && tzname[1][0])?tzname[1]:tzname[0];
#endif
						break;

				  default:
						buf[0] = '%';     /* reconstruct the format */
						buf[1] = f[-1];
						buf[2] = '\0';
							if (buf[1] == 0)
							  f--;        /* back up if at end of string */
				  }
				  while (*r)
				  {
						if (p == q)
						{
							  *q = '\0';
							  return 0;
						}
						*p++ = *r++;
				  }
			}
			else
			{
				  if (p == q)
				  {
						*q = '\0';
						return 0;
				  }
				  *p++ = f[-1];
			}
	  }
	  *p = '\0';
	  return p - s;
}

static int BASED_CODE npow[5] = { 1, 10, 100, 1000, 10000 };

/**
 * static void strfmt(LPTSTR str, LPTSTR fmt);
 *
 * simple sprintf for strftime
 *
 * each format descriptor is of the form %n
 * where n goes from zero to four
 *
 * 0    -- string %s
 * 1..4 -- int %?.?d
 *
**/

static void AFX_CDECL strfmt(LPTSTR str, LPCTSTR fmt, ...)
{
	  int ival, ilen;
	  LPTSTR sval;
	  va_list vp;

	  va_start(vp, fmt);
	  while (*fmt)
	  {
			if (*fmt++ == '%')
			{
				  ilen = *fmt++ - '0';
				  if (ilen == 0)                /* zero means string arg */
				  {
						sval = va_arg(vp, char*);
						while (*sval)
							  *str++ = *sval++;
				  }
				  else                          /* always leading zeros */
				  {
						ival = va_arg(vp, int);
						while (ilen)
						{
							  ival %= npow[ilen--];
							  *str++ = (char) ('0' + ival / npow[ilen]);
						}
				  }
			}
			else  *str++ = fmt[-1];
	  }
	  *str = '\0';
	  va_end(vp);
}

#endif // _MFC_VER >= 0x0300

//////////////////////////////////////////////////////////////////////
// Parsing date/time values

void AFXAPI GXUpdateDateTimeSettings()
{
	if (GXGetAppData()->m_pDateTimeImp)
		GXGetAppData()->m_pDateTimeImp->UpdateDateTimeSettings();
}

// public methods
GXDateFormat AFXAPI GXGetDateFormatFromWinIni(BOOL bReload, BOOL& bCentury, TCHAR& chDateSep)
{
	if (GXGetAppData()->m_pDateTimeImp)
		return GXGetAppData()->m_pDateTimeImp->GetDateFormatFromWinIni(bReload, bCentury, chDateSep);

	return gxDEFAULT;
}

TCHAR AFXAPI GXGetTimeSeparatorFromWinIni(BOOL bReload)
{
	if (GXGetAppData()->m_pDateTimeImp)
		return GXGetAppData()->m_pDateTimeImp->GetTimeSeparatorFromWinIni(bReload);

	return _T(':');
}

BOOL AFXAPI GXParseDateTime(struct tm* ptm, LPCTSTR szValue, GXDateFormat DateFormat)
{
	GX_CHECKIMP(GXGetAppData()->m_pDateTimeImp, "GXParseDateTime");

	if (GXGetAppData()->m_pDateTimeImp)
		return GXGetAppData()->m_pDateTimeImp->ParseDateTime(ptm, szValue, DateFormat);

	return FALSE;
}

BOOL AFXAPI GXParseDateTime(TIMESTAMP_STRUCT* ptm, LPCTSTR szValue, GXDateFormat DateFormat)
{
	GX_CHECKIMP(GXGetAppData()->m_pDateTimeImp, "GXParseDateTime");

	if (GXGetAppData()->m_pDateTimeImp)
		return GXGetAppData()->m_pDateTimeImp->ParseDateTime(ptm, szValue, DateFormat);

	return FALSE;
}

void AFXAPI GXFormatTimeStamp(CString& strRet, const CTime* pTime)
{
	GX_CHECKIMP(GXGetAppData()->m_pDateTimeImp, "GXFormatTimeStamp");

	if (GXGetAppData()->m_pDateTimeImp)
		GXGetAppData()->m_pDateTimeImp->FormatTimeStamp(strRet, pTime);
}

void AFXAPI GXFormatTimeStamp(CString& strRet, const TIMESTAMP_STRUCT* pTime)
{
	GX_CHECKIMP(GXGetAppData()->m_pDateTimeImp, "GXFormatTimeStamp");

	if (GXGetAppData()->m_pDateTimeImp)
		GXGetAppData()->m_pDateTimeImp->FormatTimeStamp(strRet, pTime);
}

