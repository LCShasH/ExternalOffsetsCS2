#include "memory.hpp"

#include <TlHelp32.h>
#include <iostream>



void CMemorySystem::Initialize() noexcept {
	//printf("%s -> %s", "CMemorySystem", "Start initializing...");

	OpenGame();

	if (!m_uiId)
		throw std::exception("Unable to get game id");

	//printf("%s -> %i", "Game id found", m_uiId);

	CollectModules();

	if (m_umModules.empty())
		throw std::exception("Unable to get game modules");

	printf("Game modules colledcted -> %i",m_umModules.size());

	//printf("%s -> %s", "CMemorySystem", "End of initializing");
}

void CMemorySystem::Shutdown() noexcept {

}

void CMemorySystem::OpenGame() noexcept {
	auto pEntry = PROCESSENTRY32{ };
	pEntry.dwSize = sizeof(PROCESSENTRY32);

	const auto pSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// loop through processes
	do {
		if (!strcmp(pEntry.szExeFile, "cs2.exe")) {
			m_uiId = pEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(pSnap, &pEntry));

	if (pSnap) {
		CloseHandle(pSnap);
	}

	m_hWnd = FindWindowA(0, "Counter-Strike 2");
	m_hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, m_uiId);
}

void CMemorySystem::CollectModules() noexcept {
	auto pEntry = MODULEENTRY32{ };
	pEntry.dwSize = sizeof(MODULEENTRY32);

	const auto pSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_uiId);

	// loop through modules
	do {
		m_umModules.emplace(HASH(pEntry.szModule), reinterpret_cast<std::uintptr_t>(pEntry.modBaseAddr));
	} while (Module32Next(pSnap, &pEntry));

	if (pSnap) {
		CloseHandle(pSnap);
	}
}

std::string CMemorySystem::RPMString(std::uintptr_t address) {
	char* pBuffer = new char[128];

	ReadProcessMemory(m_hProcessHandle, LPVOID(address), pBuffer, 128, 0);
	pBuffer[127] = '\0';

	return std::string(pBuffer);
}