/***************************************************************
 * Name:      reaper_guiApp.h
 * Purpose:   Defines Application Class
 * Author:    Michael Uman (muman613@gmail.com)
 * Created:   2015-09-17
 * Copyright: Michael Uman ()
 * License:
 **************************************************************/

#ifndef REAPER_GUIAPP_H
#define REAPER_GUIAPP_H

#include <wx/app.h>

class reaper_guiApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // REAPER_GUIAPP_H
