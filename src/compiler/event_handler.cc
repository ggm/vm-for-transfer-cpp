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

#include <event_handler.h>
#include <iostream>

EventHandler::EventHandler() {
	codeGenerator = NULL;
}

EventHandler::EventHandler(const EventHandler &e) {
	copy(e);
}

EventHandler::~EventHandler() {
	codeGenerator = NULL;
}

EventHandler& EventHandler::operator=(const EventHandler &e) {
	if (this != &e) {
		this->~EventHandler();
		this->copy(e);
	}
	return *this;
}

void EventHandler::copy(const EventHandler &e) {
	transferStage = e.transferStage;
	transferDefault = e.transferDefault;
	codeGenerator = e.codeGenerator;
}

/**
 * Set one of the derived code generators to use.
 *
 * @param codeGenerator the code generator to use.
 */
void EventHandler::setCodeGenerator(CodeGenerator *codeGenerator) {
	this->codeGenerator = codeGenerator;
}

void EventHandler::handleTransferStart(const Event &event) {
	transferStage = TRANSFER;

	wstring value = event.getAttribute(L"default");
	if (value == L"chunk")
		transferDefault = CHUNK;
	else
		transferDefault = LU;

	codeGenerator->genTransferStart(event);
}

void EventHandler::handleTransferEnd(const Event &event) {

}

void EventHandler::handleInterchunkStart(const Event &event) {
	transferStage = INTERCHUNK;
	codeGenerator->genInterchunkStart(event);
}

void EventHandler::handleInterchunkEnd(const Event &event) {

}

void EventHandler::handlePostchunkStart(const Event &event) {
	transferStage = POSTCHUNK;
	codeGenerator->genPostchunkStart(event);
}

void EventHandler::handlePostchunkEnd(const Event &event) {

}
