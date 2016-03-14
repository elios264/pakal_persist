# Pakal persist
C++ serialization library

Coming soon: Persist C# serialization library (same features as this library)

#Overview

Pakal Persist is  a C++ library that serializes data to and from streams in XML/Json formats
it compiles with Visual Studio 2015

Features:

* Serialization of basic types including std::string
* Serialization of enums automatically storing it as the underlying type
* Serialization of arbitrary objects
* Serialization of arrays
* Polymorphism 
* Multiple and possibly cyclical references to objects.
* Serialization of the STL containers including maps
* Backwards compatibility ( you can reorder the elements add & remove with no problem)
* Extensibility ( you can easy create your own text based serializer )
* Non intrusive serialization ( not compatible with polymorphic objects)
* Binary Archives ( coming soon)


#Usage


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
	
	
# Non intrusive serialization

When you have a struct/class that you cant or dont want to modify you can create an external function containing the serialization code.
Note: ##### this is not compatible with polymorphic objects

	struct ProprietaryStruct
	{
		int x, y;
	};

	template<>
	struct Persist<ProprietaryStruct>
	{
		static void persist(Archive* archive, ProprietaryStruct& str)
		{
			archive->value("x", str.x);
			archive->value("y", str.y);
		}
	};

	void non_intrusive_persist()
	{
		std::vector<ProprietaryStruct> str;

		str.push_back({ 1,8 });
		str.push_back({ 2,7 });
		str.push_back({ 3,6 });
		str.push_back({ 4,5 });

		XmlWriter w;
		w.write("files/non_intrusive_serialization.xml", "PropietaryObjects", str);

		str.clear();
		XmlReader r;

		r.read("files/non_intrusive_serialization.xml", "PropietaryObjects", str);

	}
	
	
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
	
#Polymorphism

Pakal Persist supports serializing polymorphic objects. To do so polymorphic types need to call  set_type_name inside the persist method:

	struct Object
	{
		virtual void persist(Archive* archive)
		{
			archive->set_type_name("Object");
			archive->value("name", m_name);
		}
	}
	
In order for Pakal Persist to support deserializing polymorphic objects you must  use the built in SimpleFactoryManager or  inherit from 

	class IFactoryManager
	{
	public:
		virtual ~IFactoryManager() {}
		virtual void* create_object(const std::string& className) = 0;
	};
	
and assign an instance  to the static field

	Archive::ObjectFactory

For example a contrived example with left and right object inheriting from a base object. It's no worse than using animals in an example.

	struct Object
	{

		std::string m_name;

		Object()
			: m_name()
		{
		}

		Object(const std::string& name)
			: m_name(name)
		{
		}

		virtual ~Object()
		{
		}

		virtual void persist(Archive* archive)
		{
			archive->set_type_name("Object");
			archive->value("name", m_name);
		}
	};

	struct LeftObject : Object
	{
		int m_left_value;

		LeftObject()
			: Object(""),
			m_left_value(0)
		{
		}

		LeftObject(const std::string& name, int left_value)
			: Object(name),
			m_left_value(left_value)
		{
		}

		virtual void persist(Archive* archive) override
		{
			Object::persist(archive);

			archive->set_type_name("LeftObject");
			archive->value("left_value", m_left_value);
		}
	};

	struct RightObject : Object
	{
		float m_right_value;

		RightObject()
			: Object(""),
			m_right_value(0.0f)
		{
		}

		RightObject(const std::string& name, float right_value)
			: Object(name),
			m_right_value(right_value)
		{
		}

		virtual void persist(Archive* archive) override
		{
			Object::persist(archive);

			archive->set_type_name("RightObject");
			archive->value("right_value", m_right_value);
		}
	};
	
	struct Polymorphism
	{
		std::vector<Object*> m_objects;

		Polymorphism()
			: m_objects()
		{
		}

		~Polymorphism()
		{
			while (!m_objects.empty())
			{
				delete m_objects.back();
				m_objects.pop_back();
			}
		}

		void create()
		{
			m_objects.push_back(new Object("object_0"));
			m_objects.push_back(new LeftObject("left_0", 0));
			m_objects.push_back(new RightObject("right_0", 0.0f));
			m_objects.push_back(new LeftObject("left_1", 1));
			m_objects.push_back(new LeftObject("left_2", 2));
			m_objects.push_back(new RightObject("right_1", 1.0f));
			m_objects.push_back(new RightObject("right_2", 2.0f));
			m_objects.push_back(new LeftObject("left_3", 3));
		}

		void persist(Archive* archive)
		{
			archive->value("objects", "object", m_objects);
		}
	};	
		
	void persist_polymorphism_example()
	{
		SimpleFactoyManager manager;

		manager.declare_object<Object>("Object", []() { return new Object(); });
		manager.declare_object<LeftObject>("LeftObject", []() { return new LeftObject(); });
		manager.declare_object<RightObject>("RightObject", []() { return new RightObject(); });

		Archive::ObjectFactory = &manager;

		Polymorphism polymorphism;
		polymorphism.create();

		XmlWriter xml_writer;
		
		xml_writer.write("files/persist_polymorphism_example.xml", "polymorphism", polymorphism);

		Polymorphism other_polymorphism;
		XmlReader xml_reader;
		
		xml_reader.read("files/persist_polymorphism_example.xml", "polymorphism", other_polymorphism);
	}	

The example program produces the following output:

	<?xml version="1.0"?>
	<polymorphism>
		<objects>
			<object class="Object" name="object_0" />
			<object class="LeftObject" name="left_0" left_value="0" />
			<object class="RightObject" name="right_0" right_value="0" />
			<object class="LeftObject" name="left_1" left_value="1" />
			<object class="LeftObject" name="left_2" left_value="2" />
			<object class="RightObject" name="right_1" right_value="1" />
			<object class="RightObject" name="right_2" right_value="2" />
			<object class="LeftObject" name="left_3" left_value="3" />
		</objects>
	</polymorphism>
		