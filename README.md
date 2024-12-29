# CPPSoffit

CPPSoffit is an Object oriented SOFFIT processor for C++.
See 'SOFFIT Definition.txt' for info on the SOFFIT standard.

## Basic Usage

CPPSoffit uses the `CPPSoffit` namespace.

### Building Blocks

CPPSoffit has two primary classes:  
`SoffitObject`  
`SoffitField`  
SoffitObject is a container that holds SOFFIT fields and other objects.  
SoffitField is glorified version of a key-value pair.

### Main Functions

There are two main functions that help you process SOFFIT streams:  
`SoffitObject* ReadStream(std::istream& stream)`  
`void WriteStream(SoffitObject* root, std::ostream& output)`  

There are also two alternate functions to work with `std::string` instead of iostreams:  
`SoffitObject* ReadStreamFromString(std::string& stream)`  
`std::string WriteStreamToString(SoffitObject* root, bool indent = true)`  

Call one of the read functions to create a `SoffitObject*`.  
You must manually delete the created `SoffitObject*` when you are done with the object.  
The destructor will also automatically delete all of its child objects and fields.  
