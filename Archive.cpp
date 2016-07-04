#include "Archive.h"

using namespace Pakal;


void* SimpleFactoyManager::create_object(const std::string& className)
{
	auto it = m_factories.find(className);
	return it != m_factories.end() ? it->second() : nullptr;
}

