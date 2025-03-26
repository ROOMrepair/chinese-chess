#include "Asset.hpp"

bool Asset::loadFont(){
	
}

bool Asset::loadAssets(const char *assetsDir, const char *fileName)
{
	std::ifstream conf_fs(fileName, std::fstream::in);

	if (!conf_fs.is_open())
	{
		std::cerr << "Failed to open file " << fileName << std::endl;
		return false;
	}

	SearchAndSetResourceDir(assetsDir);

	char line[1024];

	// ! 注意标志位的设置时机
	// https://stackoverflow.com/questions/4324441/testing-stream-good-or-stream-eof-reads-last-line-twice
	while (conf_fs.getline(line, 1024))
	{
		std::string conf_line(line);
		size_t deilm_pos = conf_line.find(':');

		std::string name = conf_line.substr(0, deilm_pos);
		trim(name);

		std::string path = conf_line.substr(deilm_pos + 1);
		trim(path);

		std::cout << "name: " << name << " path: " << path << std::endl;

		Texture2D texture = LoadTexture(path.c_str());

		// texture 加载失败
		if (texture.id == 0)
		{
			std::cout << "loading " << name << " to texture failed" << std::endl;
			return false;
		}
		
		pieceTextures[assetNum] = {name, texture};
		assetNum += 1;				
	}

	if (conf_fs.bad())
	{
		std::cerr << "Critical stream error: " << fileName << std::endl;
		return false;
	}
	else if (conf_fs.fail() && !conf_fs.eof())
	{
		std::cerr << "Failed to read line: line too long or other error in file: " << fileName << std::endl;
		return false;
	}
	else if (conf_fs.eof())
	{
		std::cout << "Reached end of file: " << fileName << std::endl;
	}

	return true;
}

void Asset::unloadAssets()
{
	for (int i = 0; i < assetNum; ++i)
	{
		if(pieceTextures[i].texture.id > 0){
			UnloadTexture(pieceTextures[i].texture);
		}
	}
}