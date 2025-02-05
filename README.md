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
`SoffitObject* ReadStream(std::istream&)`  
`void WriteStream(SoffitObject*, std::ostream&)`  

There are also two alternate functions to work with `std::string` instead of iostreams:  
`SoffitObject* ReadStreamFromString(std::string&)`  
`std::string WriteStreamToString(SoffitObject*)`  

Call one of the read functions to create a `SoffitObject*`.  
You must manually delete the created `SoffitObject*` when you are done with the object.  
The destructor will also automatically delete all of its child objects and fields.  
If you created a `SoffitObject*` in-software and want to write it somewhere, call one of the write functions.  
