#include <wx/wx.h>
#include "functions.h"
#include "CheckoutThread.h"


wxDEFINE_EVENT(CHECKOUT_EVENT, wxCommandEvent);
wxDEFINE_EVENT(TEXT_EVENT, wxCommandEvent);

wxThread*       pCurrThread = nullptr;

CheckoutThread::CheckoutThread(wxWindow* pParent, CHECKOUT_PARMS& parms)
:   m_parms(parms),
    m_parent(pParent)
{
    //ctor
    pCurrThread = this;
}

CheckoutThread::~CheckoutThread()
{
    //dtor
    pCurrThread = nullptr;
}

wxThread::ExitCode CheckoutThread::Entry() {
    wxLogDebug("CheckoutThread::Entry()");

    m_err = checkout_branch(m_parms);

    wxCommandEvent      event(CHECKOUT_EVENT);

    event.SetInt( m_err );
    wxPostEvent(m_parent->GetEventHandler(), event);

    return (ExitCode)0;
}
