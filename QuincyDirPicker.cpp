/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright ITB CompuPhase, 2016
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
 *  Version: $Id: QuincyDirPicker.cpp 5689 2017-06-05 14:05:58Z thiadmer $
 */
#include "wxQuincy.h"
#include "QuincyDirPicker.h"

QuincyDirPicker::QuincyDirPicker(wxWindow* parent, const wxString& Caption, const wxString& Path, const wxString& RequiredFile)
    : CustomDirPicker(parent)
{
    m_lblPicker->SetLabelText(Caption);
    if (Path.Length() > 0) {
        m_dirPicker->SetInitialDirectory(Path);
        m_dirPicker->SetPath(Path);
        SelectedPath = Path;
    }
    TestFile = RequiredFile;
}

wxString QuincyDirPicker::GetPath()
{
    return SelectedPath;
}

void QuincyDirPicker::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void QuincyDirPicker::OnOK(wxCommandEvent& /*event*/)
{
    SelectedPath = m_dirPicker->GetPath();
    if (TestFile.Length() > 0) {
        wxString path = SelectedPath;
        int len = path.Length();
        if (len > 0 && path[len - 1] != DIRSEP_CHAR)
            path += wxT(DIRSEP_STR);
        path += TestFile;
        if (!wxFileExists(path)) {
            int reply = wxMessageBox(wxT("The selected folder does not contain the required files.\nDo you want to select it anyway?"),
                                     wxT("Required files not present"), wxYES_NO);
            if (reply == wxNO)
                return;
        }
    }

    EndModal(wxID_OK);
}
