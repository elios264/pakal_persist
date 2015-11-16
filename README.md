# pakal_persist
C++ Xml serialization library

not ready yet!

usage: 


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