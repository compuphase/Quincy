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
 *  Version: $Id: QuincyReplaceDlg.cpp 5689 2017-06-05 14:05:58Z thiadmer $
 */
#include "QuincyReplaceDlg.h"
#include <wx/stc/stc.h>

QuincyReplaceDlg::QuincyReplaceDlg( wxWindow* parent ) :
    ReplaceDlg( parent )
{
    ctrlSearchText->SetFocus();
}

void QuincyReplaceDlg::SetFlags(unsigned flags)
{
    chkWholeWord->SetValue((flags & wxSTC_FIND_WHOLEWORD) != 0);
    chkMatchCase->SetValue((flags & wxSTC_FIND_MATCHCASE) != 0);
    chkRegEx->SetValue((flags & wxSTC_FIND_REGEXP) != 0);
}

unsigned QuincyReplaceDlg::GetFlags() const
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

void QuincyReplaceDlg::SetSearchText(const wxString& text)
{
    ctrlSearchText->SetValue(text);
    ctrlSearchText->SelectAll();
    ctrlSearchText->SetFocus();
}

wxString QuincyReplaceDlg::GetSearchText() const
{
    return ctrlSearchText->GetValue();
}

wxString QuincyReplaceDlg::GetReplaceText() const
{
    return ctrlReplaceText->GetValue();
}

void QuincyReplaceDlg::SetRecentList(const wxArrayString& choices)
{
    ctrlSearchText->Clear();
    ctrlSearchText->Append(choices);
}

void QuincyReplaceDlg::SetReplaceList(const wxArrayString& choices)
{
    ctrlReplaceText->Clear();
    ctrlReplaceText->Append(choices);
}

