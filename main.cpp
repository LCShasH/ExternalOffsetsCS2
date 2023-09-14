#include <iostream>

#include "sdk.hpp"

auto schema_offset = 0x5C770;

int main() {
	g_pMemorySystem->Initialize();

	/*
		 movzx   ecx, ax <-- moving our index of module
		 mov     rax, [rbx+198h] <-- rbx is our base address for interface and 0x198 is 51 index in array of _this ptr.
		 mov     rax, [rax+rcx*8]  <-- rcx is index of module name in some array. For client.dll for 04.26.23 it's 0xF.
		 add     rsp, 20h
		 pop     rbx
		 retn
	*/

	// Getting base address of interface
	auto ptrToBaseInterface = g_pMemorySystem->GetModule(HASH("schemasystem.dll")) + schema_offset;

	// Getting 51's element's address in _this ptr. This just some offset for CSchemaSystem class by Valv3
	auto ptrToListElement = g_pMemorySystem->RPM<std::uintptr_t>(ptrToBaseInterface + 0x198);

	// Getting our type scope for "client.dll" module
	auto ptrToScope = g_pMemorySystem->RPM<CSchemaSystemTypeScope*>(ptrToListElement + (0x10 * 8));

	auto pTable = ptrToScope->GetBindingsTable();

	printf("\n count of elements -> %i\n", pTable.Count());

	auto pElements = std::make_unique_for_overwrite< UtlTSHashHandle_t[] >(pTable.Count());
	const auto nElements = pTable.GetElements(0, pTable.Count(), pElements.get());


	// Schema table contais table with name & offset
	std::unordered_map< std::uint64_t, std::unordered_map< std::uint64_t, std::uint32_t >  > umapTableOffsets;

	for (int nElementIndex = 0; nElementIndex < nElements; nElementIndex++)
	{
		const UtlTSHashHandle_t hElement = pElements[nElementIndex];

		if (!hElement)
			continue;

		CSchemaClassBindingBase* const pClassBinding = pTable.Element(hElement);

		if (!pClassBinding)
			continue;

		if (!pClassBinding->GetNumFields())
			continue;

		printf("%s ( %i ) :\n", pClassBinding->GetName().c_str(), pClassBinding->GetNumFields());

		CSchemaField* pSchemaField = &pClassBinding->GetFields()[0];


		std::unordered_map< std::uint64_t, std::uint32_t > umapOffsets;
		for (int nFieldIndex = 0; nFieldIndex < pClassBinding->GetNumFields(); nFieldIndex++)
		{
			if (!pSchemaField)
				continue;

			auto name = pSchemaField->GetName();

			umapOffsets.try_emplace(HASH(pSchemaField->GetName()), pSchemaField->GetOffset());

			printf("\t%s = 0x%p\n", pSchemaField->GetName().c_str(), pSchemaField->GetOffset());

			pSchemaField = reinterpret_cast<CSchemaField*>(std::uintptr_t(pSchemaField) + 0x20);
		}

		umapTableOffsets.try_emplace(HASH(pClassBinding->GetName()), umapOffsets);
	}

	auto uiHealthOffset = umapTableOffsets[HASH("C_BaseEntity")][HASH("m_iHealth")];

	printf("C_BaseEntity->m_iHealth offset is 0x%p\n", uiHealthOffset);
}
