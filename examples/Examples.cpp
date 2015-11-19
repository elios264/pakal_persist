// Examples.cpp : Defines the entry point for the console application.
//

#include <vector>
#include "../XmlReader.h"
#include "../XmlWriter.h"
#include "../JsonWriter.h"
#include "../JsonReader.h"

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

struct HelloWorld
{
	std::string message;

	void persist(Archive* archive)
	{
		archive->value("message", message);
	}
};

struct vector2df
{
	float x, y;

	vector2df(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	vector2df() : x(0), y(0)
	{
	}

};
	
struct vector2di
{
	int x, y;

	vector2di(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	vector2di() : x(0), y(0)
	{
	}
};

struct recti
{
	vector2di left_corner,size;

	recti(){}

	recti(int x, int y,int w, int h) : left_corner(x,y), size(w,h)
	{
	}

};

class SpriteAnimation
{
	struct Frame
	{
		recti		texture_rect;
		vector2df	offset;
		vector2df	pivot;


		void persist(Archive* archive)
		{

			archive->value("x", texture_rect.left_corner.x);
			archive->value("y", texture_rect.left_corner.y);

			archive->value("w", texture_rect.size.x);
			archive->value("h", texture_rect.size.y);

			archive->value("oX", offset.x);
			archive->value("oY", offset.y);

			archive->value("pX", pivot.x);
			archive->value("pY", pivot.y);
		}
	};

	std::vector<Frame> m_frames;

public:

	std::string name;
	unsigned	duration = 100;
	bool		looped = false;

	inline void add_frame(recti rect, vector2df  offset = { 0,0 })
	{
		m_frames.emplace_back(Frame{ rect, offset });
	}

	inline size_t get_size() const
	{
		return m_frames.size();
	}

	inline const Frame& get_frame(size_t index) const
	{
		return m_frames[index];
	}

	void persist(Archive* archive)
	{
		archive->value("name", name);
		archive->value("looped", looped);
		archive->value("duration", duration);

		//archive->value("", "frame", m_frames);
		/*set the first parameter to empty string if you dont want the children to be nested in a inner tag eg: 
			
		<SpriteAnimation>
			<frame x="23" y="48" />
			<frame x="23" y="48" />
			<frame x="23" y="48" />
		</SpriteAnimation>
		*/

		archive->value("Frames", "frame", m_frames);
		/*
		otherwise eg:
		<SpriteAnimation>
			<Frames>
				<frame x="23" y="48" />
				<frame x="23" y="48" />
				<frame x="23" y="48" />
			</Frames>
		</SpriteAnimation>

		*/

	}

};

struct SpriteLoader
{
	std::string texture_name;
	std::string default_animation;
	unsigned width = 0;
	unsigned height = 0;
	float size_factor = 1;

	std::vector<SpriteAnimation*> animations;

	void persist(Archive* archive)
	{
		archive->value("height", height);
		archive->value("width", width);
		archive->value("size_factor", size_factor);
		archive->value("texture", texture_name);
		archive->value("default", default_animation);
		archive->value("Animations", "animation", animations);
	}
};

struct SpriteLoader2
{
	std::map<std::string, SpriteAnimation> animations;

	void persist(Archive* archive)
	{
		archive->value("", "animation", animations);
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

	JsonWriter json_writer;
	json_writer.write("cyclic_references_example.json", "people", people);

	people.clear();

	JsonReader json_reader;
	json_reader.read("cyclic_references_example.json", "people", people);
	
}

void persist_hello_world_example()
{
	HelloWorld hello_world;
	hello_world.message = "Hello World!";

	XmlWriter xml_writer;
	xml_writer.write("hello_world_example.xml", "hello_world", hello_world);

	hello_world.message.clear();

	XmlReader xml_reader;
	xml_reader.read("hello_world_example.xml", "hello_world", hello_world);

	JsonWriter json_writer;
	json_writer.write("hello_world_example.json", "hello_world", hello_world);

	hello_world.message.clear();

	JsonReader json_reader;
	json_reader.read("hello_world_example.json", "hello_world", hello_world);

}

void persist_nested_example()
{
	SpriteLoader loader;

	loader.default_animation = "standing";
	loader.texture_name = "fulgore.png";
	loader.width = 2048;
	loader.height = 1024;
	loader.size_factor = 5;


	SpriteAnimation animStanding;
	animStanding.name = "standing";
	animStanding.looped = true;
	animStanding.duration = 100;
	animStanding.add_frame(recti(489, 484, 98, 83), vector2df(0, 71));
	animStanding.add_frame(recti(23, 23, 47, 24), vector2df(14, 71));

	SpriteAnimation animBlock;
	animBlock.name = "block1";
	animBlock.looped = false;
	animBlock.duration = 50;
	animBlock.add_frame(recti(341, 681, 47, 104), vector2df(0, 0));

	loader.animations.push_back(&animStanding);
	loader.animations.push_back(&animBlock);

	JsonWriter writer;
	writer.write("nested_example.json", "SpriteSheetAnimation", loader);

	SpriteLoader loader2;

	JsonReader reader;
	reader.read("nested_example.json", "SpriteSheetAnimation", loader2);

	XmlWriter xml_writer;

	xml_writer.write("nested_example.xml", "SpriteSheetAnimation", loader);
	
	SpriteLoader loader3;

	XmlReader xml_reader;
	reader.read("nested_example.json", "SpriteSheetAnimation", loader3);

}

void persist_map_example()
{
	SpriteAnimation animStanding;
	animStanding.looped = true;
	animStanding.duration = 100;
	animStanding.add_frame(recti(489, 484, 98, 83), vector2df(0, 71));
	animStanding.add_frame(recti(23, 23, 47, 24), vector2df(14, 71));

	SpriteAnimation animBlock;
	animBlock.looped = false;
	animBlock.duration = 50;
	animBlock.add_frame(recti(341, 681, 47, 104), vector2df(0, 0));

	SpriteLoader2 loader;
	loader.animations["standing"] = animStanding;
	loader.animations["block1"] = animBlock;


	XmlWriter writer;
	writer.write("map_example.xml", "SpriteSheetAnimation", loader);

	SpriteLoader2 loader2;

	XmlReader reader;
	reader.read("map_example.xml", "SpriteSheetAnimation", loader2);

}


int main()
{
	persist_hello_world_example();
	persist_cyclic_references_example();
	persist_nested_example();
	persist_map_example();
    return 0;
}

