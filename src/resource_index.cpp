#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../include/util.hpp"

// 注意运行时路径相对于工作目录
const char *conf_file = "./resources/source.conf";
const char *conf_file_index = "./include/piece_index.hpp";

enum class ResourceType{
    IMAGE,
    FONT,
    UNKNOWN
};

int main()
{
	std::ifstream conf_fs(conf_file);

	std::ofstream conf_fs_out(conf_file_index);

	if (!conf_fs.is_open())
	{
		std::cerr << "Failed to open file " << conf_file << std::endl;
		return 1;
	}

	if (!conf_fs_out.is_open())
	{
		std::cerr << "Failed to open output file: " << conf_file_index << std::endl;
		conf_fs.close();
		return 1;
	}

	char line[1024];
	int assetNum = 0;
	auto currentType = ResourceType::UNKNOWN;	

	while (conf_fs.getline(line, 1024))
	{
		std::string conf_line(line);
		// add type filter
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

		if(currentType == ResourceType::IMAGE){
			conf_fs_out << "#define " << name << "_INDEX " << assetNum << "\n";
			assetNum += 1;
		}
	}

	if (conf_fs.bad() || (conf_fs.fail() && !conf_fs.eof()))
	{
		std::cerr << "Failed to read line: line too long or other error in file: " << conf_file << std::endl;
		conf_fs.close();
		conf_fs_out.close();
		return 1;
	}

	if (conf_fs_out.bad() || (conf_fs_out.fail() && !conf_fs_out.eof()))
	{
		std::cerr << "Failed to read line: line too long or other error in file: " << conf_file_index << std::endl;
		conf_fs.close();
		conf_fs_out.close();
		return 1;
	}

	conf_fs.close();
	conf_fs_out.close();

	return 0;
}
