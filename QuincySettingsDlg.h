/*  Quincy IDE for the Pawn scripting language
 *
 *  Copyright ITB CompuPhase, 2009-2017
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
 *  Version: $Id: QuincySettingsDlg.h 5689 2017-06-05 14:05:58Z thiadmer $
 */
#ifndef _QUINCYSETTINGSDLG_H
#define _QUINCYSETTINGSDLG_H

#include "QuincyDialogs.h"
#include "QuincyFrame.h"
#include <wx/dir.h>

/** Implementing SettingsDlg */
class QuincySettingsDlg : public SettingsDlg
{
protected:
    // Handlers for SettingsDlg events.
    virtual void OnCancel(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnTargetHost(wxCommandEvent& event);
    virtual void OnDebuggerLocal(wxCommandEvent& event);
    virtual void OnDebuggerRS232(wxCommandEvent& event);
    virtual void OnKbdChanged(wxPropertyGridEvent& event);
    virtual void OnSnippetEdit(wxGridEvent& event);
    virtual void OnUserPDFReader(wxCommandEvent& event);
    virtual void OnUserReaderBrowse(wxCommandEvent& event);

public:
    QuincySettingsDlg(wxWindow* parent);
    void InitData();
    void CopyData();

private:
    QuincyFrame* Parent;
    bool NeedRestart;

    void CollectTargetHosts();
};

#endif /* _QUINCYSETTINGSDLG_H */
