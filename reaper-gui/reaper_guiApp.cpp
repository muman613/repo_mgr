/***************************************************************
 * Name:      reaper_guiApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Michael Uman (muman613@gmail.com)
 * Created:   2015-09-17
 * Copyright: Michael Uman ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "reaper_guiApp.h"
#include "ReaperDialog.h"

IMPLEMENT_APP(reaper_guiApp);

bool reaper_guiApp::OnInit() {
    ReaperDialog*   dlg = new ReaperDialog(NULL);
    dlg->Show();

    return true;
}
