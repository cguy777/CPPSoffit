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

#pragma once
#include <vector>
#include <string>
#include <iostream>

/**
 * The CPPSoffit namespace
 */
namespace CPPSoffit {

    class SoffitField;

    class SoffitObject {
    private:
        std::string type;
        std::string name;
        SoffitObject* parent = nullptr;
        std::vector<SoffitField*> fields;
        std::vector<SoffitObject*> objects;
        int nestingLevel = -1;
        const int initialVectorCapacity = 5;

        void calculateNestingLevel();
        void setParent(SoffitObject* p);
        void reserveInitialVectorCapacity();

    public:
        /**
            * Constructs a SoffitObject with a specified type and name.
            * If this is to be a root object, best practice states that the type and name should be null.
            * @param type
            * @param name
            */
        SoffitObject(std::string type, std::string name);

        /**
            * Constructs a nameless/anonymous SoffitObject with a specified type.
            * @param type
            */
        SoffitObject(std::string type);
        ~SoffitObject();

        /**
            * Returns the type of this object.
            */
        std::string getType();

        /**
            * Returns the name of this object.
            * The name may be blank.
            * @return
            */
        std::string getName();

        /**
            * Primarily used internally for setting indentation during a write function.
            */
        int getNestingLevel();

        /**
            * Returns the pointer to this object's parent.
            * This pointer should be nullptr if it is the root object.
            * It will also be nullptr if it hasn't been added to another object
            */
        SoffitObject* getParent();

        /**
            * Renames this object.
            * @param name
            */
        void setName(std::string name);

        /**
            * Re-types this object
            * @param type
            */
        void setType(std::string type);

        /**
            * Adds a SoffitField to this object.
            * @param field
            */
        void add(SoffitField* field);

        /**
            * Adds a child object to this object.
            * @param object
            */
        void add(SoffitObject* object);

        /**
            * Gets a child object by name.
            * Will return nullptr if the object is not found.
            * @param objectName
            */
        SoffitObject* getObject(std::string objectName);

        /**
            * Gets a contained field by name.
            * Will return nullptr if the field is not found.
            * @param fieldName
            */
        SoffitField* getField(std::string fieldName);

        /**
         * Returns true if this object contains a field with the name specified.
         */
        bool hasField(std::string fieldName);

        /**
            * Returns a vector containing pointers of all of the child objects.
            * Vector will be empty if there are no child objects.
            */
        std::vector<SoffitObject*> getAllObjects();

        /**
            * Returns a vector conaining pointers of all of this object's fields.
            * Vector will be empty if this object contains no fields.
            */
        std::vector<SoffitField*> getAllFields();

        /**
            * A convenience method to return the first child object.
            * This is useful in situations where you know there is only one child object.
            * The returned pointer will be nullptr if this object does not have any child objects.
            */
        SoffitObject* getFirstObject();

        /**
            * Gets the first object that matches the specified type and name.
            * The returned pointer will be nullptr if there is no matching child object.
            */
        SoffitObject* getObjectByTypeAndName(std::string type, std::string name);

        /**
            * Returns a vector containing pointers too all of this object's children with names matching what is specified.
            * The returned vector will be empty if this object contains no child objects that match the provided name.
            */
        std::vector<SoffitObject*> getObjectsByName(std::string objectsName);

        /**
            * Returns a vector containing pointers too all of this object's fields with names matching what is specified.
            * The returned vector will be empty if this object contains no fields that match the provided name.
            */
        std::vector<SoffitField*> getFieldsByName(std::string fieldName);

        /**
            * Returns a vector containing pointers too all of this object's children with types matching what is specified.
            * The returned vector will be empty if this object contains no child objects that match the provided type.
            */
        std::vector<SoffitObject*> getObjectsByType(std::string objectsType);

        /**
            * Returns true if this object contains child objects.
            */
        bool hasObjects();

        /**
            * Returns true if this object contains fields.
            */
        bool hasFields();

        /**
            * Returns true if this object is not a child of another object.
            */
        bool isRoot();

        /**
            * Detaches and deletes the specified child object from this object.
            * This will delete/deallocate the memory used by the specified child object.
            * If you do not want to delete/deallocate the specified child object, consider using an appropriate detach method.
            * This does nothing if the object is not found.
            */
        void deleteObject(std::string name);

        /**
            * Detaches and deletes the specified child objects from this object.
            * This will delete/deallocate the memory used by the specified child objects.
            * If you do not want to delete/deallocate the specified child objects, consider using an appropriate detach method.
            * This does nothing if the object is not found.
            */
        void deleteObjectsByType(std::string type);

        /**
            * Detaches and deletes all child objects from this object.
            * This will delete/deallocate the memory used by the child objects.
            * If you do not want to delete/deallocate child objects, consider using an appropriate detach method.
            * This does nothing if this object has no child objects.
            */
        void deleteAllObjects();

        /**
            * Detaches and deletes the specified field from this object.
            * This will delete/deallocate the memory used by the specified field.
            * If you do not want to delete/deallocate the specified field, consider using an appropriate detach method.
            * This does nothing if the field is not found.
            */
        void deleteField(std::string name);

        /**
            * Detaches and deletes all fields from this object.
            * This will delete/deallocate the memory used by the fields.
            * If you do not want to delete/deallocate fields, consider using an appropriate detach method.
            * This does nothing if this object has no fields.
            */
        void deleteAllFields();

        /**
            * Detaches the specified child object from this object.
            * This does not delete/deallocate the specified object.
            * It only removes it from this object's children vector and resets the child object's parent pointer.
            * If you want to delete/deallocate the child object, consider using and appropriate delete method.
            * This does nothing if the object is not found.
            */
        void detachObject(std::string name);

        /**
            * Detaches the specified child objects by type from this object.
            * This does not delete/deallocate the specified objects.
            * It only removes it from this object's children vector and resets the child objects' parent pointer.
            * If you want to delete/deallocate the child objects, consider using an appropriate delete method.
            * This does nothing if the objects are not found.
            */
        void detachObjectsByType(std::string type);

        /**
            * Detaches all specified child objects from this object.
            * This does not delete/deallocate the objects.
            * It only removes them from this object's children vector and resets the child objects' parent pointer.
            * If you want to delete/deallocate the child objects, consider using an appropriate delete method.
            * This does nothing if there are no child ojects.
            */
        void detachAllObjects();

        /**
            * Detaches the specified field from this object.
            * This does not delete/deallocate the field.
            * It only removes them from this object's field vector and resets the field's parent pointer.
            * If you want to delete/deallocate the field, consider using an appropriate delete method.
            * This does nothing if the fields is not found.
            */
        void detachField(std::string name);

        /**
            * Detaches all fields from this object.
            * This does not delete/deallocate the fields.
            * It only removes them from this object's field vector and resets the fields' parent pointer.
            * If you want to delete/deallocate the field, consider using an appropriate delete method.
            * This does nothing if this object has no fields.
            */
        void detachAllFields();
    };

    class SoffitField {
    private:
        std::string name;
        std::string value;
        SoffitObject* parent = nullptr;

    public:
        /**
            * Constructs a new SoffitField with the specified name and value.
            */
        SoffitField(std::string name, std::string value);

        /**
            * Constructs a null SoffitField with a specified name.
            * A null field is a field that contains no value.
            */
        SoffitField(std::string name);
        ~SoffitField();

        /**
            * Returns the name of this field.
            */
        std::string getName();

        /**
        * Returns the value of this field.
        */
        std::string getValue();

        /**
        * Sets the value of this field.
        */
        void setValue(std::string v);

        /**
            * Returns the nesting level of this field.
            * This is really only used internally to set indention on a write operation.
            */
        int getNestingLevel();

        /**
            * Returns a pointer to this field's parent object.
            */
        SoffitObject* getParent();

        /**
            * Internal use.
            */
        void setParent(SoffitObject* p);
    };

    class SoffitException : public std::exception {
    private:
        std::string message;

    public:
        // Default constructor
        SoffitException();
        SoffitException(const std::string& message);
        SoffitException(const std::string& message, int soffitLineNumber);
        SoffitException(const SoffitException& se, int soffitLineNumber);
        const char* what() const noexcept override;
    };

    //**************************************
    //********** BEGIN UTILITIES************
    //**************************************

    /**
        * Parses an input stream and returns a root SoffitObject pointer containing the parsed data.
        * May throw a SoffitException for multiple reasons during input and parsing.
        * Delete the returned root object at some point.
        * If the root object is deleted, the destructor will also delete all child objects and fields.
        */
    SoffitObject* ReadStream(std::istream& stream);

    /**
        * Writes a root SoffitObject to an output stream.
        * Contains an optional flag to indent objects and fields based off of their nesting level.
        * Indentation may be turned off in situations where the stream is not made to be primarily human readable.
        * This will save a small amount of bandwidth/memory and time during writing, and conversly, re-parsing the data.
        */
    void WriteStream(SoffitObject* root, std::ostream& output, bool indent = true);

    /**
        * Parses a string and returns a root SoffitObject pointer containing the parsed data.
        * May throw a SoffitException for multiple reasons during input and parsing.
        * Delete the returned root object at some point.
        * If the root object is deleted, the destructor will also delete all child objects and fields.
        */
    SoffitObject* ReadStreamFromString(std::string& stream);

    /**
        * Writes a root SoffitObject to a string.
        * Contains an optional flag to indent objects and fields based off of their nesting level.
        * Indentation may be turned off in situations where the stream is not made to be primarily human readable.
        * This will save a small amount of bandwidth/memory and time during writing, and conversly, re-parsing the data.
        */
    std::string WriteStreamToString(SoffitObject* root, bool indent = true);

    //internal implementation
    void _writeObjects(SoffitObject* object, std::ostream& output, bool indent);
    void _parseObject(std::istream& stream, SoffitObject* parent, int lineNumber);
    std::vector<std::string> _getLineTokens(std::string& line, int lineNumber);
    std::string _getLine(std::istream& stream, int& lineNumber);
    bool _isObject(std::vector<std::string>& tokens);
    bool _isField(std::vector<std::string>& tokens);
    std::string _convertFromEscapeSequence(const std::string& s, int lineNumber);
    std::string _convertToEscapeSequence(const std::string& s);
    std::string _stripQuotations(std::string& s);
    std::string _stripWhitespace(std::string& s);
    bool _isTokenBlank(std::string token);
    bool _containsCharacter(std::string s, char c);
}