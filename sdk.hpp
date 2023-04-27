#pragma once

#include "memory.hpp"
#include "memory.hpp"
#include "utlthash.hpp"

class CSchemaField
{
public:
	std::string GetName() {
		auto pAddr = g_pMemorySystem->RPM<std::uintptr_t>(std::uintptr_t(this));
		return g_pMemorySystem->RPMString(pAddr);
	}

	std::uint32_t GetOffset() {
		return g_pMemorySystem->RPM< std::uint32_t >(std::uintptr_t(this) + 0x10);
	}
};

class CSchemaClassBindingBase
{
public:
	std::string GetName() {
		auto pAddr = g_pMemorySystem->RPM<std::uintptr_t>(std::uintptr_t(this) + 0x8);
		return g_pMemorySystem->RPMString(pAddr);
	}

	std::string GetProjectName() {
		return g_pMemorySystem->RPMString(reinterpret_cast<std::uintptr_t>(this) + 0x10);
	}

	std::uint32_t GetSize() {
		return g_pMemorySystem->RPM< std::uint32_t >(std::uintptr_t(this) + 0x18);
	}

	std::uint16_t GetNumFields() {
		return g_pMemorySystem->RPM< std::uint16_t >(std::uintptr_t(this) + 0x1C);
	}

	CSchemaField* GetFields() {
		return g_pMemorySystem->RPM< CSchemaField* >(std::uintptr_t(this) + 0x28);
	}
};

class CSchemaSystemTypeScope
{
public:
	CUtlTSHash< CSchemaClassBindingBase*, 256, unsigned int > GetBindingsTable() {
		return g_pMemorySystem->RPM< CUtlTSHash< CSchemaClassBindingBase*, 256, unsigned int > >(std::uintptr_t(this) + 0x558);
	};
};