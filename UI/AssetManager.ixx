module;

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

export module AssetManager;

export class AssetManager
{
	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::Font> fonts;

public:
	static AssetManager& Instance()
	{
		static AssetManager instance;
		return instance;
	}

	sf::Texture& GetTexture(const std::string& path)
	{
		if (!textures.contains(path))
		{
			textures[path].loadFromFile(path);
		}
		return textures[path];
	}

	sf::Font& GetFont(const std::string& path)
	{
		if (!fonts.contains(path))
		{
			fonts[path].openFromFile(path);
		}
		return fonts[path];
	}
};