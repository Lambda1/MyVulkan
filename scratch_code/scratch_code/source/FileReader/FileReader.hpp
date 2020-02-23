#ifndef __FILE_READER_HPP__
#define __FILE_READER_HPP__

/*
	�t�@�C���ǂݍ��݃w���p�[�֐�
*/

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

// �o�C�i���`���Ńt�@�C���ǂݍ���
inline std::vector<char> ReadFile(const std::string &file_name)
{
	// NOTE: �t�@�C���T�C�Y�擾���y�ɂ��邽��, �I�[����ǂݎ��.
	std::ifstream file(file_name, std::ios::ate | std::ios::binary);
	if (!file.is_open()) { throw std::runtime_error("FAILED TO OPEN FILE."); }

	size_t file_size = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(file_size);

	// �擪�փV�[�N����, �t�@�C���ǂݎ��
	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

#endif