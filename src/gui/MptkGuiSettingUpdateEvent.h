#ifndef MPTKGUISETTINGUPDATEEVENT_H_
#define MPTKGUISETTINGUPDATEEVENT_H_

#include <wx/wx.h>
#include <wx/event.h>

DECLARE_EVENT_TYPE(wxSETTING_UPDATEEVENT,7777)
#define EVT_UPDATESETTING(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxSETTING_UPDATEEVENT, wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
    ),
class MptkGuiSettingUpdateEvent : public wxCommandEvent {
 public:
  
  MptkGuiSettingUpdateEvent();

  DECLARE_DYNAMIC_CLASS(MptkGuiSettingUpdateEvent);
};

#endif /*MPTKGUISETTINGEVENT_H_*/
