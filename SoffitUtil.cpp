/*
BSD 3-Clause License

Copyright (c) 2024, Noah McLean

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CPPSoffit.h"
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <stdexcept>

namespace CPPSoffit {

    const std::string SOFFIT_START = "__SoffitStart";
    const std::string SOFFIT_END = "__SoffitEnd";
    const char ESCAPE_SEQUENCE = '\\';

    SoffitObject* ReadStream(std::istream& stream) {
        int lineNumber = 0;

        SoffitObject* root = new SoffitObject("", "");

        std::string header = _getLine(stream, lineNumber);
        if (header != SOFFIT_START)
            throw SoffitException("SOFFIT header not found.");

        _parseObject(stream, root, lineNumber);

        return root;
    }

    SoffitObject* FindInStream(std::istream& stream, std::string type, std::string name) {
        int lineNumber = 0;

        SoffitObject* root = new SoffitObject("", "");

        std::string header = _getLine(stream, lineNumber);
        if (header != SOFFIT_START)
            throw SoffitException("SOFFIT header not found.");

        SoffitObject* foundObject = _findInStream(stream, root, lineNumber, type, name);
        if(foundObject == nullptr)
            throw SoffitException("Requested SOFFIT object not found in 'FindInStream' call.");

        return foundObject;
    }

    void WriteStream(SoffitObject* root, std::ostream& output, bool indent) {
        output << SOFFIT_START << "\n";
        _writeObjects(root, output, indent);
        output << SOFFIT_END << "\n";
    }

    SoffitObject* ReadStreamFromString(std::string& stream) {
        std::istringstream iss(stream);
        return ReadStream(iss);
    }

    std::string WriteStreamToString(SoffitObject* root, bool indent) {
        std::ostringstream oss;
        WriteStream(root, oss, indent);
        return oss.str();
    }

    //************************************************
    //*********BEGIN INTERNAL IMPLEMENTATION**********
    //************************************************

    void _writeObjects(SoffitObject* object, std::ostream& output, bool indent) {
        // Write fields
        for (size_t i = 0; i < object->getAllFields().size(); i++) {
            SoffitField* field = object->getAllFields()[i];

            //Set indentation
            if (indent) {
                for (int i = 0; i < field->getNestingLevel(); i++)
                    output << "\t";
            }

            output << field->getName();
            if (field->getValue() != "")
                output << " " << "\"" << _convertToEscapeSequence(field->getValue()) << "\"\n";
            else
                output << "\n";
        }

        // Write nested objects
        for (int i = 0; i < object->getAllObjects().size(); i++) {
            SoffitObject* currentObject = object->getAllObjects()[i];

            //Set indentation
            if (indent) {
                for (int i = 0; i < currentObject->getNestingLevel(); i++)
                    output << "\t";
            }

            output << currentObject->getType();
            if (currentObject->getName() != "")
                output << " " << "\"" << _convertToEscapeSequence(currentObject->getName()) << "\" {\n";
            else
                output << " {\n";

            _writeObjects(currentObject, output, indent);

            if (indent) {
                for (int i = 0; i < currentObject->getNestingLevel(); i++)
                    output << "\t";
            }
            output << "}\n";
        }
    }

    // Parse an individual SOFFIT object and its contents from the stream
    void _parseObject(std::istream& stream, SoffitObject* parent, int lineNumber) {
        std::stack<SoffitObject*> stack;
        stack.push(parent);

        while (!stack.empty()) {
            SoffitObject* currentObject = stack.top();
            std::string line = _getLine(stream, lineNumber);

            if (line.empty()) {
                throw SoffitException("Incomplete SOFFIT stream.");
            }

            std::vector<std::string> tokens = _getLineTokens(line, lineNumber);

            //Ensure there are no double quotes in first token (The first token would be an object type or field name)
            if (_containsCharacter(tokens[0], '"'))
                throw SoffitException("SOFFIT syntax error.", lineNumber);

            // Handle various tokens
            if (tokens.size() == 1 && tokens[0] == "}") {
                if (!currentObject->isRoot()) {
                    stack.pop();
                }
                else {
                    throw SoffitException("Too many closing brackets.", lineNumber);
                }
                //Handle footer
            }
            else if (tokens[0] == SOFFIT_END) {
                if (!currentObject->isRoot()) {
                    throw SoffitException("SOFFIT footer encountered in non-root object.", lineNumber);
                }
                break;
                //Handle object
            }
            else if (_isObject(tokens)) {
                SoffitObject* newObject;

                std::string objType = tokens[0];
                if (tokens.size() == 2) {
                    newObject = new SoffitObject(objType);
                }
                else {
                    std::string objName = _stripQuotations(tokens[1]);
                    objName = _convertFromEscapeSequence(objName, lineNumber);
                    newObject = new SoffitObject(objType, objName);
                }

                currentObject->add(newObject);
                stack.push(newObject);
                //Handle field
            }
            else if (_isField(tokens)) {
                std::string fieldName = tokens[0];
                std::string fieldValue = "";

                //Set value, if defined
                if (tokens.size() > 1)
                    fieldValue = _stripQuotations(tokens[1]);

                fieldValue = _convertFromEscapeSequence(fieldValue, lineNumber);
                currentObject->add(new SoffitField(fieldName, fieldValue));
            }
            else {
                throw SoffitException("SOFFIT syntax error.", lineNumber);
            }
        }
    }

    SoffitObject* _findInStream(std::istream& stream, SoffitObject* parent, int lineNumber, std::string type, std::string name) {
        SoffitObject* foundObject = nullptr;

        std::stack<SoffitObject*> stack;
        stack.push(parent);

        while (!stack.empty()) {
            SoffitObject* currentObject = stack.top();
            std::string line = _getLine(stream, lineNumber);

            if (line.empty()) {
                throw SoffitException("Incomplete SOFFIT stream.");
            }

            std::vector<std::string> tokens = _getLineTokens(line, lineNumber);

            //Ensure there are no double quotes in first token (The first token would be an object type or field name)
            if (_containsCharacter(tokens[0], '"'))
                throw SoffitException("SOFFIT syntax error.", lineNumber);

            // Handle various tokens
            if (tokens.size() == 1 && tokens[0] == "}") {
                if (!currentObject->isRoot()) {
                    stack.pop();

                    //Check if we just parsed the requested object
                    if (currentObject->getType() == type && currentObject->getName() == name) {
                        _cleanObject(parent, currentObject);
                        return currentObject;
                    }
                }
                else {
                    throw SoffitException("Too many closing brackets.", lineNumber);
                }
                //Handle footer
            }
            else if (tokens[0] == SOFFIT_END) {
                if (!currentObject->isRoot()) {
                    throw SoffitException("SOFFIT footer encountered in non-root object.", lineNumber);
                }
                break;
                //Handle object
            }
            else if (_isObject(tokens)) {
                SoffitObject* newObject;

                std::string objType = tokens[0];
                if (tokens.size() == 2) {
                    newObject = new SoffitObject(objType);
                }
                else {
                    std::string objName = _stripQuotations(tokens[1]);
                    objName = _convertFromEscapeSequence(objName, lineNumber);
                    newObject = new SoffitObject(objType, objName);
                }

                currentObject->add(newObject);
                stack.push(newObject);
                //Handle field
            }
            else if (_isField(tokens)) {
                std::string fieldName = tokens[0];
                std::string fieldValue = "";

                //Set value, if defined
                if (tokens.size() > 1)
                    fieldValue = _stripQuotations(tokens[1]);

                fieldValue = _convertFromEscapeSequence(fieldValue, lineNumber);
                currentObject->add(new SoffitField(fieldName, fieldValue));
            }
            else {
                throw SoffitException("SOFFIT syntax error.", lineNumber);
            }
        }

        //Return null if the requested object was not found
        return nullptr;
    }

    void _cleanObject(SoffitObject* root, SoffitObject* objToBeCleaned) {
        objToBeCleaned->detachFromParant();
        delete root;
    }

    std::vector<std::string> _getLineTokens(std::string& line, int lineNumber) {
        std::vector<std::string> tokens;
        std::string currentToken = "";

        int mark = 0;
        bool insideQuotes = false;

        for (int i = 0; i < line.size(); i++) {
            if (line[i] == '"') {
                //Check for escape sequence inside a quoted area
                if (insideQuotes) {
                    //an array bounds problem in this case indicates a soffit syntax error
                    if (i - 1 < 0)
                        throw SoffitException("SOFFIT syntax error.", lineNumber);

                    if (line[i - 1] != '\\') {
                        insideQuotes = !insideQuotes;
                    }
                }
                else {
                    insideQuotes = !insideQuotes;
                }
            }

            if (line[i] == ' ' && !insideQuotes) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
            }
            else {
                currentToken += line[i];
            }
        }

        if (!currentToken.empty()) {
            tokens.push_back(currentToken);
        }

        return tokens;
    }

    std::string _getLine(std::istream& stream, int& lineNumber) {
        while (true) {
            bool eos = false;
            std::string line = "";
            lineNumber++;

            while (true) {

                //try {
                    int c = stream.get();

                    //Check for EOS;
                    if (c == -1) {
                        eos = true;
                        break;
                    }

                    //Check for new line
                    if (c == (int)'\n')
                        break;
                    if (c == (int)'\r')
                        break;

                    line.append(1, (char) c);

                //}
                //catch (IOException e) {
                    //Explicitly return null
                    //return null;
                //}
            }

            line = _stripWhitespace(line);

            //Check for EOS and essentially a null line
            if (eos && line.length() == 0)
                return "";

            //Return if EOS is reached
            if (eos)
                return line;

            //Check for blank line
            if (line.empty())
                continue;

            //Check for comments
            if (line.at(0) == '#')
                continue;

            //if (line != null)
                return line;
        }

        /*
        std::string line;

        while (std::getline(stream, line)) {
            lineNumber++;
            line = _stripWhitespace(line);
            if (line.empty() || line[0] == '#')
                continue; // Skip blank or comment lines

            return line;
        }
        return "";
        */
    }

    //Check if the tokens represent a SOFFIT object
    bool _isObject(std::vector<std::string>& tokens) {
        return (tokens.size() == 2 && tokens[1] == "{") || (tokens.size() == 3 && tokens[1][0] == '"' && tokens[1].back() == '"');
    }

    //Check if the tokens represent a SOFFIT field
    bool _isField(std::vector<std::string>& tokens) {
        return tokens.size() == 1 || (tokens.size() == 2 && tokens[1][0] == '"');
    }

    std::string _convertFromEscapeSequence(const std::string& s, int lineNumber) {
        std::string result = "";

        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '\\') {

                //double quote
                if (s[i + 1] == '"') {
                    result += '"';
                    i++;
                    continue;
                }

                //Newline
                if (s[i + 1] == 'n') {
                    result += '\n';
                    i++;
                    continue;
                }

                //Backslash
                if (s[i + 1] == '\\') {
                    result += '\\';
                    i++;
                    continue;
                }

                throw SoffitException("Invalid escape sequence", lineNumber);
            }
            else {
                result += s[i];
            }
        }

        return result;
    }

    std::string _convertToEscapeSequence(const std::string& s) {
        std::string result = "";

        for (char c : s) {
            //Double quote correction
            if (c == '"') {
                result += "\\\"";
                continue;
            }

            //Newline correction
            if (c == '\n') {
                result += "\\n";
                continue;
            }

            //Backslash correction
            if (c == ESCAPE_SEQUENCE) {
                result += "\\\\";
                continue;
            }

            //Add all normal characters
            result += c;
        }

        return result;
    }

    std::string _stripQuotations(std::string& s) {
        return s.substr(1, s.size() - 2);
    }

    //Strip leading and trailing whitespace
    std::string _stripWhitespace(std::string& s) {
        std::string leading = "";
        std::string trailing = "";

        //Strip leading
        for (int i = 0; i < s.length(); i++) {
            if (s.at(i) != ' ' && s.at(i) != '\t') {
                leading.append(s.substr(i, s.length() - i));
                break;
            }
        }

        //Strip trailing
        for (int i = leading.length(); i > 0; i--) {
            if (leading.at(i - 1) != ' ' && leading.at(i - 1) != '\t') {
                trailing.append(leading.substr(0, i));
                break;
            }
        }

        return trailing;
    }

    bool _isTokenBlank(std::string token) {
        for (int i = 0; i < token.length(); i++) {
            if (token[i] != ' ' || token[i] != '\t')
                return false;
        }

        return true;
    }

    bool _containsCharacter(std::string s, char c) {
        for (int i = 0; i < s.length(); i++) {
            if (s[i] == c)
                return true;
        }

        return false;
    }
}
