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
 * Version: $Id: KbdShortcuts.cpp 5504 2016-05-15 13:42:30Z  $
 */
#include "KbdShortcuts.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayKbdShortcut);

/** GetName() returns the name of the shortcut. The "&" decoration (of menu
 *  items) may optionally be removed.
 */
wxString KbdShortcut::GetName(bool decorated) const
{
    if (decorated)
        return m_name;
    wxString name = m_name;
    name.Replace(wxT("&"), wxT(""));
    return name;
}

/** FindNextInCat() finds the first item with a matching category after "base".
 *  Set "base" to -1 to find the very first item. The comparison is
 *  case-sensitive.
 *  \return the index in the list, or -1 when not found.
 */
int KbdShortcutList::FindNextInCat(int base, const wxString& category)
{
    base += 1;
    while (base < (int)m_shortcuts.Count()) {
        if (m_shortcuts[base].GetCategory().Cmp(category) == 0)
            return base;
        base++;
    }
    return -1;
}

/** FindLabel() finds the index of the shortcut with the given label, or
 *  returns -1 ig not found.
 */
int KbdShortcutList::FindLabel(const wxString& label)
{
    for (unsigned index = 0; index < m_shortcuts.Count(); index++) {
        if (m_shortcuts[index].GetLabel().Cmp(label) == 0)
            return (int)index;
    }
    return -1;
}

KbdShortcut* KbdShortcutList::GetItem(int index) const
{
    if (index >= (int)m_shortcuts.Count())
        return NULL;
    return &m_shortcuts[index];
}

wxString KbdShortcutList::FormatShortCut(const wxString& label,
                                         bool includename, bool parentheses)
{
    int index = FindLabel(label);
    if (index < 0)
        return wxEmptyString;
    wxASSERT(index >= 0 && index < (int)m_shortcuts.Count());
    wxString shortcut = m_shortcuts[index].GetShortcut();
    if (shortcut.Length() > 0) {
        if (parentheses) {
            shortcut = wxT("(") + shortcut + wxT(")");
            if (!includename)
                shortcut = wxT(" ") + shortcut;
        }
        if (includename)
            shortcut = m_shortcuts[index].GetName() + wxT("\t") + shortcut;
    } else if (includename) {
        shortcut = m_shortcuts[index].GetName();
    }
    return shortcut;
}
