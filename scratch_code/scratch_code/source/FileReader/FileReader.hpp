#ifndef __FILE_READER_HPP__
#define __FILE_READER_HPP__

/*
	ファイル読み込みヘルパー関数
*/

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

// バイナリ形式でファイル読み込み
inline std::vector<char> ReadFile(const std::string &file_name)
{
	// NOTE: ファイルサイズ取得を楽にするため, 終端から読み取る.
	std::ifstream file(file_name, std::ios::ate | std::ios::binary);
	if (!file.is_open()) { throw std::runtime_error("FAILED TO OPEN FILE."); }

	size_t file_size = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(file_size);

	// 先頭へシークして, ファイル読み取り
	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

#endif