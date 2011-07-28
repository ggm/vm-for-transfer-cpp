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

#ifndef COMPILEREXCEPTION_H_
#define COMPILEREXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

/// This class defines a compiler error as an exception.
class CompilerException : public exception
{
public:
	CompilerException(string m="exception!") : msg(m) {}
	virtual ~CompilerException() throw () {}
	const char* what() const throw() { return msg.c_str(); }

private:
	string msg;
};

#endif /* COMPILEREXCEPTION_H_ */
