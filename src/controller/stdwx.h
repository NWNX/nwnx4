// http://www.litwindow.com/Knowhow/wxHowto/wxhowto.html#precompiled

#include "wx/wx.h" 

// debug memory allocation enhancement (see next tip)
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

// #pragma warning (disable:4786)

