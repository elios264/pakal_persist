#include "Attribute.h"
#include <sstream>


using namespace Pakal;


const std::string& Attribute::name() const
{
	return m_name;
}

void* Attribute::address() const
{
	void* value = nullptr;
	std::istringstream stream(m_value);
	stream >> value;
	return value;
}

bool Attribute::boolean() const
{
	return m_value == "1" || m_value == "true";
}

int Attribute::integer() const
{
	int value;
	std::istringstream stream(m_value);
	stream >> value;
	return value;
}

unsigned Attribute::unsigned_integer() const
{
	unsigned value;
	std::istringstream stream(m_value);
	stream >> value;
	return value;
}

double Attribute::real() const
{
	double value;
	std::istringstream stream(m_value);
	stream >> value;
	return value;
}

const std::string& Attribute::string() const
{
	return m_value;
}