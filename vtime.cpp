#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> // system()を使用するため
#include "parameters.hpp"
#include "simulator.hpp"

//global関数
Parameters params(DEFAULT_PARAM_FILE);

int main(int argc, char* argv[]){
     //
    std::string filepath = params.getVisittimenumFilePath() + "1" + ".log";
    std::ofstream tempFile("output/vtime.txt");
    std::ifstream logfile(filepath);
    //debug
    // for(unsigned int i = 0;i < filepaths.size(); i++){
    //     //
    //     std::cout << "\n Filepath: " <<filepaths[i];
    // }
        // 条件に基づいて行をフィルタリングして出力
    std::string line;
    int lineNumber = 0; // 行番号を追跡 (必要に応じて)
    while (std::getline(logfile, line)) {
        lineNumber++;

        // 条件をここに記述 (例: "ERROR" を含む行を抽出)
        if (lineNumber == 200) {
            tempFile << line << std::endl;
        }
    }
    filepath = params.getimpAreaNumsFilePath() + "1" + ".log";
    std::ifstream file(filepath);

    while (std::getline(file, line)) {
        tempFile << line << std::endl;
        
    }

    // ファイルを閉じる

    tempFile.close();
    logfile.close();
    file.close();

    return 0;
}