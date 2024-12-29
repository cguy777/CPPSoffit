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

using namespace std;

namespace CPPSoffit {
    SoffitObject::SoffitObject(string type, string name) {
        this->type = type;
        this->name = name;

        reserveInitialVectorCapacity();
    }

    SoffitObject::SoffitObject(string type) {
        this->type = type;
        name = "";

        reserveInitialVectorCapacity();
    }

    SoffitObject::~SoffitObject() {
        parent->setParent(nullptr);

        //Delete all stored objects
        for (int i = 0; i < objects.size(); i++) {
            delete objects.back();
            objects.pop_back();
        }

        //Delete all stored Fields
        for (int i = 0; i < fields.size(); i++) {
            delete fields.back();
            fields.pop_back();
        }
    }

    string SoffitObject::getType() {
        return type;
    }

    string SoffitObject::getName() {
        return name;
    }

    int SoffitObject::getNestingLevel() {
        return nestingLevel;
    }

    SoffitObject* SoffitObject::getParent() {
        return parent;
    }

    void SoffitObject::setName(string name) {
        this->name = name;
    }

    void SoffitObject::setType(string type) {
        this->type = type;
    }

    void SoffitObject::add(SoffitField* field) {
        field->setParent(this);
        fields.push_back(field);
    }

    void SoffitObject::add(SoffitObject* object) {
        object->setParent(this);
        object->calculateNestingLevel();
        objects.push_back(object);
    }

    SoffitObject* SoffitObject::getObject(string objectName) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects.at(i)->getName() == objectName) {
                return objects.at(i);
            }
        }
        return nullptr;
    }

    SoffitObject* SoffitObject::getFirstObject() {
        if (objects.size() > 0)
            return objects.at(0);

        return nullptr;
    }

    SoffitObject* SoffitObject::getObjectByTypeAndName(string type, string name) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects.at(i)->getType() == type && objects.at(i)->getName() == name)
                return objects.at(i);
        }

        return nullptr;
    }

    vector<SoffitObject*> SoffitObject::getObjectsByName(string objectsName) {
        vector<SoffitObject*> foundObjects;

        for (int i = 0; i < objects.size(); i++) {
            if (objects.at(i)->getName() == objectsName) {
                foundObjects.push_back(objects.at(i));
            }
        }

        return foundObjects;
    }

    vector<SoffitObject*> SoffitObject::getObjectsByType(string objectsType) {
        vector<SoffitObject*> foundObjects;

        for (int i = 0; i < objects.size(); i++) {
            if (objects.at(i)->getType() == objectsType) {
                foundObjects.push_back(objects.at(i));
            }
        }

        return foundObjects;
    }

    vector<SoffitObject*> SoffitObject::getAllObjects() {
        return objects;
    }

    SoffitField* SoffitObject::getField(string fieldName) {
        for (int i = 0; i < fields.size(); i++) {
            if (fields[i]->getName() == fieldName) {
                return fields[i];
            }
        }
        return nullptr;
    }

    vector<SoffitField*> SoffitObject::getFieldsByName(string fieldName) {
        vector<SoffitField*> foundFields;

        for (int i = 0; i < fields.size(); i++) {
            if (fields.at(i)->getName() == fieldName) {
                foundFields.push_back(fields.at(i));
            }
        }

        return foundFields;
    }

    vector<SoffitField*> SoffitObject::getAllFields() {
        return fields;
    }

    bool SoffitObject::hasObjects() {
        return objects.size() > 0;
    }

    bool SoffitObject::hasFields() {
        return fields.size() > 0;
    }

    bool SoffitObject::isRoot() {
        return parent == nullptr;
    }

    void SoffitObject::deleteObject(string name) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects[i]->getName() == name) {
                delete objects[i];
                objects.erase(objects.begin() + i);
                return;
            }
        }
    }

    void SoffitObject::deleteObjectsByType(string type) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects[i]->getType() == type) {
                delete objects[i];
                objects.erase(objects.begin() + i);
            }
        }
    }

    void SoffitObject::deleteAllObjects() {
        for (int i = 0; i < objects.size(); i++) {
            delete objects.back();
            objects.pop_back();
        }
    }

    void SoffitObject::deleteField(string name) {
        for (int i = 0; i < fields.size(); i++) {
            if (fields.at(i)->getName() == name) {
                delete fields[i];
                fields.erase(fields.begin() + i);
                return;
            }
        }
    }

    void SoffitObject::deleteAllFields() {
        for (int i = 0; i < fields.size(); i++) {
            delete fields.back();
            fields.pop_back();
        }
    }

    void SoffitObject::detachObject(string name) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects.at(i)->getName() == name) {
                objects[i]->setParent(nullptr);
                objects.erase(objects.begin() + i);
                return;
            }
        }
    }

    void SoffitObject::detachObjectsByType(string type) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects.at(i)->getType() == type) {
                objects[i]->setParent(nullptr);
                objects.erase(objects.begin() + i);
            }
        }
    }

    void SoffitObject::detachAllObjects() {
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->setParent(nullptr);
        }
        objects.clear();
    }

    void SoffitObject::detachField(string name) {
        for (int i = 0; i < fields.size(); i++) {
            if (fields[i]->getName() == name) {
                fields[i]->setParent(nullptr);
                fields.erase(fields.begin() + i);
                return;
            }
        }
    }

    void SoffitObject::detachAllFields() {
        for (int i = 0; i < fields.size(); i++) {
            fields[i]->setParent(nullptr);
        }
        fields.clear();
    }

    void SoffitObject::calculateNestingLevel() {
        if (parent) {
            nestingLevel = parent->getNestingLevel() + 1;
        }
        else {
            nestingLevel = 0;
        }
    }

    void SoffitObject::setParent(SoffitObject* p) {
        parent = p;
    }

    void SoffitObject::reserveInitialVectorCapacity() {
        objects.reserve(initialVectorCapacity);
        fields.reserve(initialVectorCapacity);
    }
}