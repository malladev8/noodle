#pragma once
#include "Core/NoodleCore.h"
#include <filesystem>

class RedAngelApp
{
public:
	RedAngelApp();
	void Run();

protected:

private:
	void Initialize();
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render();
	void EndFrame();
	void Shutdown();

	std::filesystem::path m_ProjectRoot;
	std::filesystem::path m_CookedAssetRoot;
};