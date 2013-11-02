#include <Frame.h>

#include <debugger.h> 

using namespace core::igui;

Frame::Frame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
: wxFrame(parent, id, title, pos, size, style, name)
{

}

Frame::~Frame()
{

}