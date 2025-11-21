// Fix BCB6's tchar.h which only has _TCHAR but not TCHAR.
#if defined(__BORLANDC__) && !defined(_TCHAR_DEFINED)
    typedef _TCHAR    TCHAR, *PTCHAR;
    typedef _TCHAR    TBYTE, *PTBYTE;
    #define _TCHAR_DEFINED
#endif    // #if defined(__BORLANDC__) && !defined(_TCHAR_DEFINED)
