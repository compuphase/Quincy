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
 *  Version: $Id: QuincyReplaceDlg.h 5689 2017-06-05 14:05:58Z thiadmer $
 */
#ifndef _QUINCYREPLACEDLG_H
#define _QUINCYREPLACEDLG_H

#include "QuincyDialogs.h"

class QuincyReplaceDlg : public ReplaceDlg
{
public:
    QuincyReplaceDlg(wxWindow* parent);

    void SetFlags(unsigned flags);
    unsigned GetFlags() const;
    void SetSearchText(const wxString& text);
    wxString GetSearchText() const;
    wxString GetReplaceText() const;
    void SetRecentList(const wxArrayString& choices);   /* recent search texts */
    void SetReplaceList(const wxArrayString& choices);  /* recent replace texts */
};

#endif /* _QUINCYREPLACEDLG_H */
