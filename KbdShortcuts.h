/* Support functions for keyboard shortcuts
 *
 * Copyright ITB CompuPhase, 2014-2016
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Version: $Id: KbdShortcuts.h 5504 2016-05-15 13:42:30Z  $
 */
#ifndef _KBDSHORTCUTS_H
#define _KBDSHORTCUTS_H

#include <wx/wx.h>
#include <wx/dynarray.h>

class KbdShortcut {
public:
    KbdShortcut(const wxString& label, const wxString& name,
                const wxString& shortcut = wxEmptyString,
                const wxString& category = wxEmptyString)
        : m_label(label), m_name(name), m_shortcut(shortcut), m_category(category)
        {}
    wxString GetLabel() const { return m_label; }
    wxString GetName(bool decorated = true) const;
    wxString GetCategory() const { return m_category; }
    wxString GetShortcut() const { return m_shortcut; }
    void SetShortcut(const wxString& shortcut) { m_shortcut = shortcut; }
private:
    wxString m_label;       /* the name in the INI file */
    wxString m_name;        /* a descriptive name */
    wxString m_shortcut;    /* shortcut conforming to wxWidgets description */
    wxString m_category;    /* an optional category */
};

WX_DECLARE_OBJARRAY(KbdShortcut, ArrayKbdShortcut);

class KbdShortcutList {
public:
    KbdShortcutList() { m_shortcuts.Clear(); }
    void Add(const wxString& label, const wxString& name,
             const wxString& shortcut = wxEmptyString,
             const wxString& category = wxEmptyString)
    {
        KbdShortcut s(label, name, shortcut, category);
        m_shortcuts.Add(s);
    }
    void Clear() { m_shortcuts.Clear(); }
    int Count() const { return (int)m_shortcuts.Count(); }
    KbdShortcut* GetItem(int index) const;
    int FindNextInCat(int base, const wxString& category);
    int FindLabel(const wxString& label);
    wxString FormatShortCut(const wxString& label,
                            bool includename = false,
                            bool parenthesis = false);
private:
    ArrayKbdShortcut m_shortcuts;
};

#endif /* _KBDSHORTCUTS_H */

