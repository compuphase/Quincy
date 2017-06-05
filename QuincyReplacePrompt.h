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
 *  Version: $Id: QuincyReplacePrompt.h 5689 2017-06-05 14:05:58Z thiadmer $
 */
#ifndef _QUINCYREPLACEPROMPT_H
#define _QUINCYREPLACEPROMPT_H

#include "QuincyDialogs.h"

class QuincyReplacePrompt : public ReplacePrompt
{
public:
    QuincyReplacePrompt( wxWindow* parent );

    virtual void OnDialogKey(wxKeyEvent& event);
    virtual void OnYes(wxCommandEvent& event);
    virtual void OnNo(wxCommandEvent& event);
    virtual void OnLast(wxCommandEvent& event);
    virtual void OnYesToAll(wxCommandEvent& event);
    virtual void OnCancel(wxCommandEvent& event);
};

#endif /* _QUINCYREPLACEPROMPT_H */
