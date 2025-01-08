#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip> 
#include <cstdlib> // system()を使用するため
#include "parameters.hpp"
#include "simulator.hpp"

//global関数
Parameters params(DEFAULT_PARAM_FILE);

int main(int argc, char* argv[]){
    //filePathリスト
    std::vector<std::string> filepaths;
    std::vector<std::string> ImpAreanumsfilePaths;
    std::vector<std::string> CellscoveragefilePaths;
    for(unsigned int i = 0;i < params.getSimIteration();i++){
        //
        std::string filepath = params.getCoverageAveFilePath() + std::to_string(i) + ".log";
        filepaths.push_back(filepath);
        filepath = params.getimpAreaNumsFilePath() + std::to_string(i) + ".log";
        ImpAreanumsfilePaths.push_back(filepath);
        filepath = params.getCoverageLogFilePath() + std::to_string(i) + ".log";
        CellscoveragefilePaths.push_back(filepath);
    }
    //debug
    // for(unsigned int i = 0;i < filepaths.size(); i++){
    //     //
    //     std::cout << "\n Filepath: " <<filepaths[i];
    // }
    std::vector<std::vector<double>> allData;
    for (const auto& filePath : filepaths) {
        std::ifstream file(filePath);
        if (!file) {
            std::cerr << "Error: Cannot open file " << filePath << std::endl;
            return 1;
        }

        std::vector<double> columnData;
        double value;
        while (file >> value) {
            columnData.push_back(value);
            //std::cout << "\n" << value;
        }
        allData.push_back(columnData);
    }
    // for (size_t i = 0; i < allData.size(); i++) { // 行ループ
    //     for (size_t j = 0; j < allData[i].size(); j++) { // 列ループ
    //         std::cout << "\n" << allData[i][j] << " ";
    //     }
    //     std::cout << std::endl; // 行の終わりで改行
    // }
    //重点探索エリアの番号を格納 | [round数][重点探索エリア番号]
    std::vector<std::vector<int>> impAreanums;
    for (const auto& filepath : ImpAreanumsfilePaths){
        std::ifstream file(filepath);
        std::vector<int> columnData;
        int value;
        while (file >> value) {
            columnData.push_back(value);
            //std::cout << "\n" << value;
            if (file.fail()) {
                std::cerr << "ファイル読み取り中にエラーが発生しました。" << std::endl;
            }
        }
        impAreanums.push_back(columnData);
        file.close();
    }

    // for (const auto& row : impAreanums) {
    //     for (const auto& cell : row) {
    //         std::cout << cell << "\t";
    //     }
    //     std::cout << std::endl;
    // }

    //重点探索エリアの平均を格納　｜　[timeslot]
    //重点探索エリアのカバレッジを格納 | [round][timeslot]
    std::vector<std::vector<double>> impAreacoverageAve;
    std::vector<std::vector<double>> nomalAreacoverageAve;
    int round = 0;
    for (const auto& filepath : CellscoveragefilePaths) {
        std::ifstream file(filepath);
        std::string line;
        std::vector<double> columnData;
        std::vector<double> columnData2;
        int lineNumber = 1; // 行番号をカウント
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string value;
            int columnIndex = 1;
            double sum = 0.0;
            double sum2 = 0.0;
            int count = 0;
            int count2 = 0;

            while (std::getline(ss, value, '\t')) { // タブ区切りで値を取得
                for(size_t i = 0;i < impAreanums[round].size();i++){
                    if(columnIndex == impAreanums[round][i]){
                        try {
                            sum += std::stod(value); // 値を数値に変換して合計に加える
                            count++;
                            //std::cout << value;
                        } catch (const std::invalid_argument& e) {
                            //std::cerr << "行 " << lineNumber << " に無効な値をスキップ: " << value << std::endl;
                        }
                        
                    }else{
                         try {
                            sum2 += std::stod(value); // 値を数値に変換して合計に加える
                            count2++;
                            //std::cout << value;
                        } catch (const std::invalid_argument& e) {
                            //std::cerr << "行 " << lineNumber << " に無効な値をスキップ: " << value << std::endl;
                        }
                    }
                    //std::cout << columnIndex << std::endl;
                }
                columnIndex++;
            }

            if (count == 0) {
                std::cout << "行 " << lineNumber << " に有効なデータがありませんでした。" << std::endl;
            } else {
                double average = sum / count;
                columnData.push_back(average);
                average = sum2 / count2;
                columnData2.push_back(average);
                //std::cout << "行 " << lineNumber << " の平均: " << std::fixed << std::setprecision(2) << average << std::endl;
            }
            lineNumber++;
        }       
        impAreacoverageAve.push_back(columnData);
        nomalAreacoverageAve.push_back(columnData2);
        round++;
        file.close();
    }

    // for (size_t i = 0; i < impAreacoverageAve.size(); i++) { // 行ループ
    //     for (size_t j = 0; j < impAreacoverageAve.size(); j++) { // 列ループ
    //         std::cout << impAreacoverageAve[i][j] << " ";
    //     }
    //     std::cout << std::endl; // 行の終わりで改行
    // }
    
    // 最大行数を確認
    size_t maxRows = 0;
    for (const auto& columnData : allData) {
        maxRows = std::max(maxRows, columnData.size());
    }

    // 各行ごとに平均を計算
    std::vector<double> rowAverages(maxRows, 0.0);
    std::vector<double> rowimpAverages(maxRows,0.0);
    std::vector<double> rownomalAverages(maxRows,0.0);
    std::vector<size_t> counts(maxRows, 0);

    for (const auto& columnData : allData) {
        for (size_t i = 0; i < columnData.size(); ++i) {
            rowAverages[i] += columnData[i];
            counts[i] += 1;
        }
    }

    for (const auto& columnData : impAreacoverageAve){
        for(size_t i = 0; i < columnData.size(); ++i) {
            rowimpAverages[i] += columnData[i];
        }
    }

    for (const auto& columnData : nomalAreacoverageAve){
        for(size_t i = 0; i < columnData.size(); ++i) {
            rownomalAverages[i] += columnData[i];
        }
    }

    // 平均値を出力
    //std::cout << "Row Averages:" << std::endl;
    for (size_t i = 0; i < maxRows; ++i) {
        if (counts[i] > 0) {
            rowAverages[i] /= counts[i];
            rowimpAverages[i] /= counts[i];
            rownomalAverages[i] /= counts[i];
        }
        //std::cout << "Row " << i + 1 << ": "  << rowAverages[i] << std::endl;
    }
    //    
    // 整形済みデータを一時ファイルに書き込む
    std::ofstream tempFile("temp_data.txt");
    if (!tempFile.is_open()) {
        std::cerr << "Failed to create temp file." << std::endl;
        return 1;
    }
    for (size_t i = 0; i < rowAverages.size(); ++i) {
        tempFile << i + 1 << " " << rowAverages[i] << " "<< rowimpAverages[i] << " "<< rownomalAverages[i]<<"\n"; // 行番号をX軸、値をY軸に
    }
    tempFile.close();

    // gnuplotスクリプトを作成
    std::ofstream gnuplotScript("gnuplot_script.gp");
    if (!gnuplotScript.is_open()) {
        std::cerr << "Failed to create gnuplot script." << std::endl;
        return 1;
    }

    // gnuplotスクリプトの内容
    gnuplotScript << "set terminal pdf\n";
    gnuplotScript << "set output 'graph/graphAve.pdf'\n";
    gnuplotScript << "set title 'Log File Data'\n";
    gnuplotScript << "set xlabel 'time slot'\n";
    gnuplotScript << "set ylabel 'Coverage Average'\n";
    gnuplotScript << "set yrange[0:1]\n";
    gnuplotScript << "set grid ytics lt 0 lc rgb '#7d7d7d'\n";
    gnuplotScript << "set ytics 0.2\n";
    gnuplotScript << "plot 'temp_data.txt' using 1:2 with lines title 'Nomal area and Important area'\n";//, 'temp_data.txt' using 1:3 with lines title 'Important area','temp_data.txt' using 1:4 with lines title 'Nomal area'\n";
    //gnuplotScript << "pause -1\n"; // グラフを表示したままにする
    gnuplotScript << "set output\n";
    gnuplotScript.close();

    // gnuplotを実行
    system("gnuplot gnuplot_script.gp");

    // 一時ファイルを削除（必要に応じて）
    std::remove("temp_data.txt");
    std::remove("gnuplot_script.gp");

    return 0;
}