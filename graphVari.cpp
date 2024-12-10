#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> // system()を使用するため
#include "parameters.hpp"
#include "simulator.hpp"

//global関数
Parameters params(DEFAULT_PARAM_FILE);

int main(int argc, char* argv[]){
    //filePathリスト
    std::vector<std::string> filepaths;
    for(unsigned int i = 0;i < params.getSimIteration();i++){
        //
        std::string filepath = "logs/coverageVari" + std::to_string(i) + ".log";
        filepaths.push_back(filepath);
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
        }
        allData.push_back(columnData);
    }
    
    // 最大行数を確認
    size_t maxRows = 0;
    for (const auto& columnData : allData) {
        maxRows = std::max(maxRows, columnData.size());
    }

    // 各行ごとに平均を計算
    std::vector<double> rowAverages(maxRows, 0.0);
    std::vector<size_t> counts(maxRows, 0);

    for (const auto& columnData : allData) {
        for (size_t i = 0; i < columnData.size(); ++i) {
            rowAverages[i] += columnData[i];
            counts[i] += 1;
        }
    }

    // 平均値を出力
    //std::cout << "Row Averages:" << std::endl;
    for (size_t i = 0; i < maxRows; ++i) {
        if (counts[i] > 0) {
            rowAverages[i] /= counts[i];
        }
        //std::cout << "Row " << i + 1 << ": "  << rowAverages[i] << std::endl;
    }
    //
    // std::vector<double> rowVariance(maxRows, 0.0);

    // for (const auto& columnData : allData) {
    //     for (size_t i = 0; i < columnData.size(); ++i) {
    //         double diff = columnData[i] - rowAverages[i];
    //         rowVariance[i] += diff * diff;
    //     }
    // }
    
        // 行ごとの分散を計算
    // for (size_t i = 0; i < maxRows; ++i) {
    //     if (counts[i] > 1) { // データが1件以上ある場合に分散を計算
    //         rowVariance[i] /= counts[i]; // 平均平方偏差（分散）
    //     }
    // }

    std::ofstream tempFile("temp_data.txt");
    if (!tempFile.is_open()) {
        std::cerr << "Failed to create temp file." << std::endl;
        return 1;
    }
    for (size_t i = 0; i < rowAverages.size(); ++i) {
        tempFile << i + 1 << " " << rowAverages[i] << "\n"; // 行番号をX軸、値をY軸に
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
    gnuplotScript << "set output 'graph/graphVari.pdf'\n";
    gnuplotScript << "set title 'Log File Data'\n";
    gnuplotScript << "set xlabel 'time slot'\n";
    gnuplotScript << "set ylabel 'Coverage Variance'\n";
    gnuplotScript << "set yrange[0:0.3]\n";
    gnuplotScript << "set xtics 50\n";
    gnuplotScript << "set grid ytics lt 0 lc rgb '#7d7d7d'\n";
    gnuplotScript << "set ytics 0.1\n";
    gnuplotScript << "plot 'temp_data.txt' using 1:2 with lines title 'Log Data'\n";
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