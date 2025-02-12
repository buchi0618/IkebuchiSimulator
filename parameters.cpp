#include "simulator.hpp"
#include "cmdline.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>

using namespace boost::property_tree;
using namespace std;

Parameters::Parameters(std::string filename) {
	setFromIniFile(filename);
}

// コマンドライン引数をパース
void Parameters::parseCmdline(int argc, char* argv[]) {
#ifdef DEBUG
	cout << "\nstart: Parameters::parseCmdline()";
#endif

	cmdline::parser cmdParser;

	// パラメータ設定に関する引数を引っこ抜いてくる
	cmdParser.add<string>("ini", '\0', "parameter ini-file name", false);	// 環境設定ファイル
	//
	// 他に使いたい引数があればここに追記
	//
	cmdParser.parse_check(argc, argv);

	// コマンドライン引数でiniファイルが指定されていれば、その記載箇所のみデフォルト値から変更
	if (cmdParser.get<string>("ini") != "") { setFromIniFile(cmdParser.get<string>("ini")); }
	//
	// 他に使いたい引数があればここに追記
	//

#ifdef DEBUG
	cout << "\nend: Parameters::parseCmdline()";
#endif
}

// iniファイルから設定を読み込んで設定
void Parameters::setFromIniFile(std::string filename) {
#ifdef DEBUG
	cout << "\nstart: Parameters::setFromIniFile()";
#endif

	ptree pt;
	read_ini(filename, pt);

	// [sim]
	if (boost::optional<unsigned int> _simIteration = pt.get_optional<unsigned int>("sim.simIteration"))
		simIteration = _simIteration.get();
	if (boost::optional<unsigned int> _simDuration = pt.get_optional<unsigned int>("sim.simDuration"))
		simDuration = _simDuration.get();

	// [env]
	if (boost::optional<int> _uavNum = pt.get_optional<int>("env.uavNum"))
		uavNum = _uavNum.get();
	if (boost::optional<unsigned int> _uavMoveLogNum = pt.get_optional<unsigned int>("env.uavMoveLogNum"))
		uavMoveLogNum = _uavMoveLogNum.get();
	if (boost::optional<int> _cellCols = pt.get_optional<int>("env.cellCols"))
		cellCols = _cellCols.get();
	if (boost::optional<int> _cellRows = pt.get_optional<int>("env.cellRows"))
		cellRows = _cellRows.get();
	if (boost::optional<double> _covAttenuation = pt.get_optional<double>("env.coverageAttenuation"))
		coverageAttenuation = _covAttenuation.get();
	if (boost::optional<double> _threshold = pt.get_optional<double>("env.threshold"))
		threshold = _threshold.get();
	if (boost::optional<int> _impAreaNum = pt.get_optional<int>("env.impAreaNum"))
		impAreaNum = _impAreaNum.get();

	// [file]
	if (boost::optional<string> _coverageLogFilePath = pt.get_optional<string>("file.coverageLogFilePath"))
		coverageLogFilePath = _coverageLogFilePath.get();
	if (boost::optional<string> _uavLocLogFilePath = pt.get_optional<string>("file.uavLocLogFilePath"))
		uavLocLogFilePath = _uavLocLogFilePath.get();
	if (boost::optional<string> _uavcoverageFilePath = pt.get_optional<string>("file.uavcoverageFilePath"))
		uavcoverageFilePath = _uavcoverageFilePath.get();
	if (boost::optional<string> _coverageAveFilePath = pt.get_optional<string>("file.coverageAveFilePath"))
		coverageAveFilePath = _coverageAveFilePath.get();
	if (boost::optional<string> _coverageVariFilePath = pt.get_optional<string>("file.coverageVariFilePath"))
		coverageVariFilePath = _coverageVariFilePath.get();
	if (boost::optional<string> _impcoverageFilePath = pt.get_optional<string>("file.impcoverageFilePath"))
		impcoverageFilePath = _impcoverageFilePath.get();
	if (boost::optional<string> _impAreanumsFilePath = pt.get_optional<string>("file.impAreanumsFilePath"))
		impAreanumsFilePath = _impAreanumsFilePath.get();
	if (boost::optional<string> _visittimenumFilePath = pt.get_optional<string>("file.visittimenumFilePath"))
	visittimenumFilePath = _visittimenumFilePath.get();
#ifdef DEBUG
	cout << "\nend: Parameters::setFromIniFile()";
#endif
}
