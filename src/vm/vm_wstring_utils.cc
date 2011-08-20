/*Copyright (C) 2011  Gabriel Gregori Manzano

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "vm_wstring_utils.h"

#include <locale>

/**
 * Check if a wide string is lower case, locale specific.
 *
 * @param wstr the wide string to check
 *
 * @return true if it's lower case, otherwise, false
 */
bool VMWstringUtils::iswlower(const wstring &wstr) {
  locale loc;

  for (unsigned int i = 0; i < wstr.size(); i++) {
    if (islower(wstr[i], loc)) {
      return false;
    }
  }

  return true;
}

/**
 * Check if every character of a wide string is a digit, locale specific.
 *
 * @param wstr the wide string to check
 *
 * @return true if it's a number, otherwise, false
 */
bool VMWstringUtils::iswnumeric(const wstring &wstr) {
  locale loc;

  for (unsigned int i = 0; i < wstr.size(); i++) {
    if (!isdigit(wstr[i], loc)) {
      return false;
    }
  }

  return true;
}

/**
 * Change the case of a wide string to lower case, locale specific.
 *
 * @param wstr the wide string to change
 *
 * @return the wide string with the changed case
 */
wstring VMWstringUtils::wtolower(const wstring &wstr) {
  locale loc("");
  wstring wstrLower = L"";

  for (unsigned int i = 0; i < wstr.size(); i++) {
    wstrLower += tolower(wstr[i], loc);
  }

  return wstrLower;
}

/**
 * Change the case of a wide string to upper case, locale specific.
 *
 * @param wstr the wide string to change
 *
 * @return the wide string with the changed case
 */
wstring VMWstringUtils::wtoupper(const wstring &wstr) {
  locale loc;
  wstring wstrUpper = L"";

  for (unsigned int i = 0; i < wstr.size(); i++) {
    wstrUpper += toupper(wstr[i], loc);
  }

  return wstrUpper;
}


/**
 * Get the case of a wide string as defined by Apertium.
 *
 * @param wstr the wide string to get the case from
 *
 * @return the case of the wide string
 */
CASE VMWstringUtils::getCase(const wstring &wstr) {
  locale loc;
  bool isFirstUpper = isupper(wstr[0], loc);
  bool isUpper = VMWstringUtils::iswlower(wstr);

  // If it's a 1-length string and is upper, capitalize it.
  if (isUpper && wstr.size() == 1) {
    return Aa;
  } else if (isFirstUpper && !isUpper) {
    return Aa;
  } else if (isUpper) {
    return AA;
  } else {
    return aa;
  }
}

/**
 * Change the case of a wide string to the one passed as parameter.
 *
 * @param wstr the wide string to change the case of
 * @param luCase the case for the wide string
 *
 * @return the wide string with the new case
 */
wstring VMWstringUtils::changeCase(const wstring &wstr, CASE luCase) {
  locale loc;
  wstring wstrNewCase = L"";

  switch (luCase) {
  case aa:
    wstrNewCase = VMWstringUtils::wtolower(wstr);
    break;
  case Aa:
    wstrNewCase = wstr;
    wstrNewCase[0] = toupper(wstr[0], loc);
    break;
  case AA:
    wstrNewCase = VMWstringUtils::wtoupper(wstr);
    break;
  }

  return wstrNewCase;
}

/**
 * Change the case of a wide string to the one passed as parameter.
 *
 * @param wstr the wide string to change the case of
 * @param luCase a wide string representing a case
 *
 * @return the wide string with the new case
 */
wstring VMWstringUtils::changeCase(const wstring &wstr, const wstring &luCase) {
  if (luCase == L"aa") {
    return VMWstringUtils::changeCase(wstr, aa);
  } else if (luCase == L"Aa") {
    return VMWstringUtils::changeCase(wstr, Aa);
  } else if (luCase == L"AA") {
    return VMWstringUtils::changeCase(wstr, AA);
  } else {
    return wstr;
  }
}

/**
 * Replace a wide substring with another wide substring.
 *
 * @param wstr the wide string with the content
 * @param replaced the wide string to be replaced
 * @param replacement the wide string to put in the replaced place
 *
 * @return the wide string with the replacement done
 */
wstring VMWstringUtils::replace(wstring &wstr, const wstring &replaced,
    const wstring &replacement) {
  size_t pos = wstr.find(replaced);

  while (pos != string::npos) {
    wstr.replace(pos, replaced.size(), replacement);
    pos = wstr.find(replaced);
  }

  return wstr;
}
