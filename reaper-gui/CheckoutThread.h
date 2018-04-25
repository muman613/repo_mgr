#ifndef __CHECKOUTTHREAD_H__
#define __CHECKOUTTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include "functions.h"

class CheckoutThread : public wxThread
{
    public:
        CheckoutThread(wxWindow* pParent, CHECKOUT_PARMS& parms);
        virtual ~CheckoutThread();

        int             m_err;

    protected:
        virtual ExitCode Entry();

    private:
        CHECKOUT_PARMS  m_parms;
        wxWindow*       m_parent;
};

wxDECLARE_EVENT(CHECKOUT_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TEXT_EVENT, wxCommandEvent);

extern wxThread*        pCurrThread;

#endif // __CHECKOUTTHREAD_H__
