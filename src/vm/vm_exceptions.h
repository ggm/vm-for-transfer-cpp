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

#ifndef VM_EXCEPTIONS_H_
#define VM_EXCEPTIONS_H_

#include <exception>
#include <string>

using namespace std;

/// This class defines a vm error as a base exception.
class VmException: public exception {
public:
  VmException(wstring m) : msg(m) { }
  virtual ~VmException() throw () { }
  wstring getMessage() const { return msg; }

private:
  wstring msg;
};

/// This class defines an exception for loader specific errors.
class LoaderException: public VmException {
};

/// This class defines an exception for interpreter specific errors.
class InterpreterException: public VmException {
};

#endif /* VM_EXCEPTIONS_H_ */
