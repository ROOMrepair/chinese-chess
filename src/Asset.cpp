#include "Asset.hpp"

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
	auto currentType = ResourceType::UNKNOWN;	

	// ! 注意标志位的设置时机
	// https://stackoverflow.com/questions/4324441/testing-stream-good-or-stream-eof-reads-last-line-twice
	while (conf_fs.getline(line, 1024))
	{
		std::string conf_line(line);
		// empty line
		if(conf_line.empty()){
			continue;	
		}	
		
		if(conf_line.rfind("TYPE=") == 0){
			std::string typeSign = conf_line.substr(5);
			if(typeSign == "IMAGE"){
				currentType = ResourceType::IMAGE;
			}else if(typeSign == "FONT"){
				currentType = ResourceType::FONT;
			}else{
				currentType = ResourceType::UNKNOWN;
  				std::cerr << "Unknown resource type: " << typeSign << std::endl;
				return false;
			}
			continue;
		}

		size_t deilm_pos = conf_line.find(':');
		
		std::string name = conf_line.substr(0, deilm_pos);
		trim(name);

		std::string path = conf_line.substr(deilm_pos + 1);
		trim(path);

		std::cout << "name: " << name << " path: " << path << std::endl;

		switch(currentType){
			case ResourceType::IMAGE:{
				Texture2D texture = LoadTexture(path.c_str());
				if (texture.id == 0)
				{
					std::cerr << "loading " << name << " to texture failed" << std::endl;
					return false;
				}

				// move ?
				pieceTextures[assetNum++] = {name, texture};
				break;
			}
			case ResourceType::FONT:{

				std::vector<int> charRange;

				// load default acsii
				int startPoint = 32;
				int endPoint = 126;
					
				for(int i = startPoint;i <= endPoint;++i){
					charRange.push_back(i);
				}

				// load chinese 
				startPoint = 19968;
				endPoint = 40869;
				
				for(int i = startPoint;i <= endPoint;++i){
					charRange.push_back(i);
				}
				
				// LoadFont
				Font font = LoadFontEx(path.c_str(), DEFAULT_FONT_SIZE,charRange.data(),charRange.size());
				// 怎么判断加载失败
				if(font.texture.id == 0){
					std::cerr << "loading " << name << " to texture failed" << std::endl;
					return false;
				}
				fonts[fontNum++] = {name,font};	
				
				DEBUG_("font ",font.glyphCount);		

				break;
			}
			default:{
  				std::cerr << "Unknown resource type" << std::endl;
				return false;
				break;
			}
		}

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
		// if(pieceTextures[i].texture.id > 0){
		UnloadTexture(pieceTextures[i].texture);
		// }
	}
	for(int i = 0;i < fontNum;++i){
		UnloadFont(fonts[i].font);
	}
}