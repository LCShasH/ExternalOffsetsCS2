#pragma once

/*
	MemorySystem class allows to work with game mory more simple.
	Contains arrays with .exe stuff ( like loaded modules, handles etc. )
*/

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include <string_view>

namespace FNV1A {
	inline constexpr auto uiSeed = 0xcbf29ce484222325;
	inline constexpr auto uiPrime = 0x1000193;

	inline std::uint64_t Hash(const char* txt) noexcept {
		auto hash = uiSeed;

		for (auto i = 0u; i < strlen(txt); i++) {
			hash ^= txt[i];
			hash *= uiPrime;
		}

		return hash;
	}

	inline std::uint64_t Hash(const std::string_view& str) noexcept {
		return Hash(str.data());
	}
}

#define HASH(str) FNV1A::Hash(str)

class CMemorySystem {
public:
	void Initialize() noexcept;
	void Shutdown() noexcept;

	HWND GetGameWindowHandle() { return m_hWnd; }

	//void WPM(DWORD addres, T buffer)
	//{
	//	WriteProcessMemory(m_hProcessHandle, LPVOID(addres), &buffer, sizeof(buffer), 0);
	//}

	template<typename T>
	T RPM(std::uintptr_t addres) {
		T buffer;
		ReadProcessMemory(m_hProcessHandle, LPVOID(addres), &buffer, sizeof(buffer), 0);
		return buffer;
	}

	std::string RPMString(std::uintptr_t address);

	auto GetModules() { return m_umModules; }
	std::uintptr_t GetModule(std::uint64_t uiHashName) { return (std::uintptr_t)(m_umModules[uiHashName]); }
private:
	void CollectModules() noexcept;
	void OpenGame() noexcept;

	// Map with modules and their base address
	std::unordered_map< std::uint64_t, std::uintptr_t > m_umModules;

	// Game id
	std::uint32_t m_uiId = 0;

	// Game window handle
	HWND m_hWnd = 0;
	HANDLE m_hProcessHandle = 0;
};

inline auto g_pMemorySystem = std::make_unique< CMemorySystem >();