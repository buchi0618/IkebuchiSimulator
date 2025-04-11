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

    // gnuplotスクリプトを作成
    std::ofstream gnuplotScript("gnuplot_script.gp");
    if (!gnuplotScript.is_open()) {
        std::cerr << "Failed to create gnuplot script." << std::endl;
        return 1;
    }

    // gnuplotスクリプトの内容
    gnuplotScript << "set terminal pngcairo size 360,492\n";
    gnuplotScript << "set output 'graph/mergegraph.png'\n";
    //gnuplotScript << "set title '分散'\n";
    gnuplotScript << "set size 1, 1\n";
    // gnuplotScript << "set lmargin 20\n";
    // gnuplotScript << "set bmargin 10\n";
    //gnuplotScript << "set rmargin 10\n";
    gnuplotScript << "set key font 'Arial, 20'\n";
    gnuplotScript << "set xlabel 'Elapsed Time'font 'Arial, 20' \n";
    gnuplotScript << "set xtics 40 font 'Arial, 20' \n";
    gnuplotScript << "set ylabel 'Average Coverage' font 'Arial,20' \n";
    gnuplotScript << "set yrange[0:1]\n";
    gnuplotScript << "set grid ytics lt 0 lc rgb '#7d7d7d'\n";
    gnuplotScript << "set ytics 0.2 font 'Arial, 20'\n";
    // gnuplotScript << "set yrange[0:0.3]\n";
    gnuplotScript << "plot 'output/Ave_data_IKEBUCHI_s0.6.txt' using 1:3 with lines lw 2 title '提案手法', 'output/Ave_data_ITO_1.25.txt' using 1:3 with lines dashtype '--' lw 2  title '先行手法'\n";
    //gnuplotScript << "pause -1\n"; // グラフを表示したままにする
    gnuplotScript << "set output\n";
    gnuplotScript << "set terminal pngcairo size 360,492\n";
    gnuplotScript << "set output 'graph/mergegraph2.png'\n";
    //gnuplotScript << "set title '分散'\n";
    gnuplotScript << "set xlabel 'Elapsed Time'font 'Arial, 20' \n";
    gnuplotScript << "set xtics 40 font 'Arial, 20'\n";
    gnuplotScript << "set ylabel 'Average Coverage' font 'Arial,20'\n";
    gnuplotScript << "set yrange[0:1]\n";
    gnuplotScript << "set grid ytics lt 0 lc rgb '#7d7d7d'\n";
    gnuplotScript << "plot'output/Ave_data_IKEBUCHI_s0.6.txt' using 1:2 with lines lw 2 title '提案手法', 'output/Ave_data_ITO_1.25.txt' using 1:2 with lines dashtype '--' lw 2 title '先行手法'\n";
    gnuplotScript << "set output\n";
    gnuplotScript.close();

    // gnuplotを実行
    system("gnuplot gnuplot_script.gp");
    return 0;
}