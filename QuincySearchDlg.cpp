/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright ITB CompuPhase, 2014-2016
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *  use this file except in compliance with the License. You may obtain a copy
 *  of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 *
 *  Version: $Id: QuincySearchDlg.cpp 5504 2016-05-15 13:42:30Z  $
 */
#include "QuincySearchDlg.h"
#include <wx/stc/stc.h>

QuincySearchDlg::QuincySearchDlg(wxWindow* parent) :
    SearchDlg(parent)
{
    ctrlSearchText->SetFocus();
}

void QuincySearchDlg::SetFlags(unsigned flags)
{
    chkWholeWord->SetValue((flags & wxSTC_FIND_WHOLEWORD) != 0);
    chkMatchCase->SetValue((flags & wxSTC_FIND_MATCHCASE) != 0);
    chkRegEx->SetValue((flags & wxSTC_FIND_REGEXP) != 0);
}

unsigned QuincySearchDlg::GetFlags() const
{
    unsigned flags = 0;
    if (chkWholeWord->GetValue())
        flags |= wxSTC_FIND_WHOLEWORD;
    if (chkMatchCase->GetValue())
        flags |= wxSTC_FIND_MATCHCASE;
    if (chkRegEx->GetValue())
        flags |= wxSTC_FIND_REGEXP;
    return flags;
}

void QuincySearchDlg::SetScope(int scope)
{
    wxASSERT(scope >= 0 && scope < 3);
    optLocation->SetSelection(scope);
}

int QuincySearchDlg::GetScope() const
{
    return optLocation->GetSelection();
}

void QuincySearchDlg::SetSearchText(const wxString& text)
{
    ctrlSearchText->SetValue(text);
	ctrlSearchText->SelectAll();
	ctrlSearchText->SetFocus();
}

wxString QuincySearchDlg::GetSearchText() const
{
    return ctrlSearchText->GetValue();
}

void QuincySearchDlg::SetRecentList(const wxArrayString& choices)
{
    ctrlSearchText->Clear();
    ctrlSearchText->Append(choices);
}
