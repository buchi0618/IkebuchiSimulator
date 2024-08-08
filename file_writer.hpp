#pragma once
/**
 * @file file_writer.hpp
 * @brief ファイル書込み操作関係ひとまとめ
 * @date 2017/04/27
 */
#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>

using namespace std;

class FileWriter
{
	map<string, ofstream*> fileMap;

public:
	FileWriter() = default;
	~FileWriter() = default;

	/**
	* @brief 書込み用ファイルオープン
	* @param filename ファイル名
	* @return false:オープン失敗、true：オープン成功
	* @detail 書込みファイルリスト writeFileMap にエントリがなければストリームを新規作成してオープン
	*/
	bool openFile(std::string filename) {
		if (fileMap.find(filename) == fileMap.end()) {
			fileMap[filename] = new ofstream(filename);
			if (!fileMap[filename] || !*fileMap[filename]) {
				std::cout << "\nError: can't open writeFile " << filename << " (at FileWriter::openFile())";
			}
			return true;
		}
		else { return false; }
	}

	/**
	* @brief 書込み用ファイルクローズ
	* @param filename ファイル名
	* @detail 書込みファイルリスト writeFileMap にエントリがあれば、クローズしてストリームを破棄
	*/
	void closeFile(std::string filename) {
		if (fileMap.find(filename) == fileMap.end()) {
			std::cout << "\nError: file handler to " << filename << " not found. (at FileWriter::closeFile())";
		}
		else {
			fileMap[filename]->close();
			fileMap.erase(filename);
		}
	}

	/**
	* @brief ファイルに1行書込み
	* @param filename ファイル名
	* @param str 書き込む内容
	* @detail 書込みファイルリスト writeFileMap 内にある対象ストリームに対して1行追記。最後に改行。
	*/
	void writeLine(std::string filename, std::string str) {
		if (fileMap.find(filename) == fileMap.end()) {
			std::cout << "\nError: file handler to " << filename << " not found (at FileWriter::writeLine())";
		}
		else {
			*fileMap[filename] << str << std::endl;
		}
	}
};