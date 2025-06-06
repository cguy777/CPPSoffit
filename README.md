# CPPSoffit

CPPSoffit is an object oriented SOFFIT processor for C++.
SOFFIT (String Object Framework For Information Transfer) is a general-purpose data serialization framework/format.
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

There are two main functions implemented in SoffitUtil.cpp that help you process SOFFIT streams:  
`SoffitObject* ReadStream(std::istream&)`  
`void WriteStream(SoffitObject*, std::ostream&)`  

There are also two alternate functions to work with `std::string` instead of iostreams:  
`SoffitObject* ReadStreamFromString(std::string&)`  
`std::string WriteStreamToString(SoffitObject*)`  

Call one of the read functions to create a `SoffitObject*`.  
You must manually delete the created `SoffitObject*` when you are done with the object.  
The destructor will also automatically delete all of its child objects and fields.  
You can also manually instantiate objects and fields using traditional constructors:  
`SoffitObject* exampleObject = new SoffitObject("ObjectType", "ObjectName");`  
There is a plethora methods associated with the SoffitObject and SoffitField classes to help you manage your data in many different ways.
