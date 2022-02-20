#pragma once
#include <filesystem>

namespace OGLRT
{
	inline std::filesystem::path get_binary_directory()
	{
		return std::filesystem::path(INSTALL_BINDIR);
	}

	inline std::filesystem::path get_assets_directory()
	{
		return std::filesystem::path(INSTALL_ASSETSDIR);
	}

	inline std::filesystem::path get_shaders_directory()
	{
		return std::filesystem::path(INSTALL_SHADERSDIR);
	}
}