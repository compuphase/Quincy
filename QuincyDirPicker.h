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
 *  Version: $Id: QuincyDirPicker.h 5689 2017-06-05 14:05:58Z thiadmer $
 */
#ifndef _QUINCYDIRPICKER_H
#define _QUINCYDIRPICKER_H

#include "QuincyDialogs.h"

class QuincyDirPicker : public CustomDirPicker
{
public:
    /** Constructor */
    QuincyDirPicker(wxWindow* parent, const wxString& Caption, const wxString& Path = wxEmptyString, const wxString& RequiredFile = wxEmptyString);
    wxString GetPath();

    virtual void OnCancel(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);

private:
    wxString TestFile;
    wxString SelectedPath;
};

#endif /* _QUINCYDIRPICKER_H */
