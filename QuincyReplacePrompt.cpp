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
 *  Version: $Id: QuincyReplacePrompt.cpp 5504 2016-05-15 13:42:30Z  $
 */
#include "QuincyReplacePrompt.h"

QuincyReplacePrompt::QuincyReplacePrompt( wxWindow* parent ) :
    ReplacePrompt( parent )
{
    SetFocus();
}

void QuincyReplacePrompt::OnDialogKey(wxKeyEvent& event)
{
    switch (event.GetKeyCode()) {
    case 'Y':
        EndDialog(wxID_YES);
        break;
    case 'N':
        EndDialog(wxID_NO);
        break;
    case 'L':
        EndDialog(wxID_LAST);
        break;
    case 'A':
        EndDialog(wxID_YESTOALL);
        break;
    default:
        event.Skip();
    }
}

void QuincyReplacePrompt::OnYes(wxCommandEvent& /* event */)
{
    EndDialog(wxID_YES);
}

void QuincyReplacePrompt::OnNo(wxCommandEvent& /* event */)
{
    EndDialog(wxID_NO);
}

void QuincyReplacePrompt::OnLast(wxCommandEvent& /* event */)
{
    EndDialog(wxID_LAST);
}

void QuincyReplacePrompt::OnYesToAll(wxCommandEvent& /* event */)
{
    EndDialog(wxID_YESTOALL);
}

void QuincyReplacePrompt::OnCancel(wxCommandEvent& /* event */)
{
    EndDialog(wxID_CANCEL);
}

