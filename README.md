# pakal persist
C++ serialization library

Overview

Pakal Persist is  a C++ library that serializes data to and from streams in XML format ( for the moment)
it compiles with Visual Studio 2015

Features:

Serialization of basic types including strings
Serialization of enums automatically storing it as the underlying type
Serialization of arbitrary objects
serialization of arrays
Multiple and possibly cyclical references to objects.
Serialization of the STL containers including maps
Backwards compatibility ( you can reorder the elements add & remove with no problem)

Binary Archives ( coming soon)
Polymorphism ( coming soon )

Extensibility ( you can easy create your own text based serializer  (eg Json, Lua)


#Usage:


Pakal Persist serializes arbitrary data to and from an iostream. Typically this is a file but can be a buffer in memory or any more complex combination supported by iostreams.

Data is serialized out by creating a writer and calling its write() method. This recursively traverses the data model being serialized using function templates that describe the relationships in the data model.
Similarly data is read in by creating a reader and calling its read() method. This reconstructs the data in memory using the same functions.

The function templates to traverse the data model are typically provided as persist() member function of the class being serialized.

	void persist( Archive* archive )
	
For example the classic "Hello World!" using Pakal Persist:


	#include "XmlWriter.h"

	struct HelloWorld
	{
		std::string message;
		
		void persist( Archive* archive )
		{
			archive->value( "message", message );
		}
	};


	HelloWorld hello_world;
	hello_world.message = "Hello World!";

	XmlWriter writer;
	writer.write( "persist_hello_world_example.xml", "hello_world", hello_world );
	
	
	
#Cyclic References

Pakal Persist supports serialization of cyclic references by allowing objects to be persisted by reference instead of by value.
Serializing by reference uses the address of an object as a unique identifier that can be written out in place of the object's content and used to resolve back to the equivalent object when the serialized archive is read back into memory. Any referenced object must be serialized somewhere within the archive so that references to it can be properly recovered.
When a serialized archive is read back in it is read using two passes. The first pass loads the data from the archive back into memory and builds a mapping from each object's address when it was written into the archive to the address that it has been allocated at when read in. The second pass traverses the data in memory to resolve references made using the old addresses to objects at their new addresses in memory.
To serialize an object by reference instead of by value the application calls the refer() function on the archive rather than the value() function. The obvious usage of this is in serializing a raw pointer to an object but it may also be applied to objects with pointer like semantics (e.g. smart pointers) or to containers. In the latter case the contents of the container are serialized as references rather than as values.
For example a simple network of people with cyclic references from each person to their friends:

	include <vector>
	#include "../XmlReader.h"
	#include "../XmlWriter.h"

	using namespace Pakal;


	struct Person
	{
		std::string name;
		std::vector<Person*> friends;

		Person(): name(),friends()
		{}

		Person(const std::string& name): name(name),friends()
		{}

		void add_friend(Person* person)
		{
			friends.push_back(person);
		}

		void persist(Archive* archive)
		{
			archive->value("name", name);
			archive->refer("friends", "friend", friends);
		}
	};


	void persist_cyclic_references_example()
	{
		std::vector<Person> people;
		people.push_back(Person("Alfred"));
		people.push_back(Person("Ben"));
		people.push_back(Person("Camilla"));
		people.push_back(Person("Denise"));

		Person& alfred = people[0];
		Person& ben = people[1];
		Person& camilla = people[2];
		Person& denise = people[3];

		alfred.add_friend(&ben);
		alfred.add_friend(&camilla);
		ben.add_friend(&alfred);
		ben.add_friend(&denise);
		camilla.add_friend(&alfred);
		camilla.add_friend(&ben);
		camilla.add_friend(&denise);
		denise.add_friend(&camilla);

		XmlWriter xml_writer;
		xml_writer.write("cyclic_references_example.xml", "people", people);

		people.clear();
		XmlReader xml_reader;
		xml_reader.read("cyclic_references_example.xml", "people", people);
	}
	
The example program produces the following output:	

	<?xml version="1.0" ?>
	<people>
		<item name="Alfred" address="0x31f08">
			<friends>
				<friend address="0x31f18"/>
				<friend address="0x31f28"/>
			</friends>
		</item>
		<item name="Ben" address="0x31f18">
			<friends>
				<friend address="0x31f08"/>
				<friend address="0x31f38"/>
			</friends>
		</item>
		<item name="Camilla" address="0x31f28">
			<friends>
				<friend address="0x31f08"/>
				<friend address="0x31f18"/>
				<friend address="0x31f38"/>
			</friends>
		</item>
		<item name="Denise" address="0x31f38">
			<friends>
				<friend address="0x31f28"/>
			</friends>
		</item>
	</people>