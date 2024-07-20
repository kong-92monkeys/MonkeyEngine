export module ntmonkeys.com.Lib.AssetManager;

import ntmonkeys.com.Lib.Unique;
import <filesystem>;
import <fstream>;
import <sstream>;
import <memory>;
import <format>;
import <string_view>;
import <vector>;

namespace Lib
{
	export class AssetManager : public Unique
	{
	public:
		AssetManager() noexcept;
		AssetManager(const std::string_view &rootPath) noexcept;

		void setRootPath(const std::string_view &rootPath) noexcept;

		[[nodiscard]]
		std::string readString(const std::string_view &path) const;

		[[nodiscard]]
		std::vector<std::byte> readBinary(const std::string_view &path) const;

	private:
		std::filesystem::path __rootPath;
	};
}

module: private;

namespace Lib
{
	AssetManager::AssetManager() noexcept
	{}

	AssetManager::AssetManager(const std::string_view &rootPath) noexcept
	{
		setRootPath(rootPath);
	}

	void AssetManager::setRootPath(const std::string_view &rootPath) noexcept
	{
		__rootPath = rootPath;
	}

	std::string AssetManager::readString(const std::string_view &path) const
	{
		const auto filePath{ __rootPath / path };

		std::ifstream fin{ filePath };
		if (!fin)
			throw std::runtime_error{ std::format("Cannot open file: {}", filePath.string()) };

		std::ostringstream oss;
		oss << fin.rdbuf();

		return oss.str();
	}

	std::vector<std::byte> AssetManager::readBinary(const std::string_view &path) const
	{
		const auto filePath{ __rootPath / path };

		std::ifstream fin{ filePath, std::ios_base::binary };
		if (!fin)
			throw std::runtime_error{ std::format("Cannot open file: {}", filePath.string()) };

		fin.unsetf(std::ios_base::skipws);

		fin.seekg(0, std::ios::end);
		const auto memSize{ fin.tellg() };
		fin.seekg(0, std::ios::beg);

		std::vector<std::byte> retVal;
		retVal.resize(memSize);

		fin.read(reinterpret_cast<char *>(retVal.data()), memSize);
		return retVal;
	}
}