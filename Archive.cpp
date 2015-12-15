#include "Archive.h"

using namespace Pakal;

IFactoryManager* Archive::ObjectFactory = nullptr;


void* SimpleFactoyManager::create_object(const std::string& className)
{
	auto it = m_factories.find(className);
	return it != m_factories.end() ? it->second() : nullptr;
}

