/*!
 * \file ParserGM.cpp
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#include "ParserGM.h"
#include <time.h>

#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/IBaseDataMgr.h"

#include "../Share/TimeUtils.hpp"
#include "../Share/StrUtil.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/ModuleHelper.hpp"

#include <boost/bind.hpp>

//#ifdef _WIN32
//#pragma comment(lib, "../API/gmsdk3.5.17/lib/win32/gmsdk.lib")
//#elif  _WIN64
#pragma comment(lib, "../API/gmsdk3.5.17/lib/win64/gmsdk.lib")
//#endif

 //By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"
template<typename... Args>
inline void write_log(IParserSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	static thread_local char buffer[512] = { 0 };
	fmtutil::format_to(buffer, format, args...);

	sink->handleParserLog(ll, buffer);
}


extern "C"
{
	EXPORT_FLAG IParserApi* createParser()
	{
		//std::cout << "ParserGM create parser" << std::endl;

		std::string module = "gmsdk";
		std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "lib");
		DllHandle hInst = DLLHelper::load_library(dllpath.c_str());
		if (hInst == NULL)
		{
			int err = ::GetLastError();
			std::cout << "[ParserGM] load " << dllpath << " failed, err=" << err << std::endl;
		}
		else
		{
			std::cout << "[ParserGM] load gmsdk success" << std::endl;
		}

		ParserGM* parser = new ParserGM();
		return parser;
	}

	EXPORT_FLAG void deleteParser(IParserApi* &parser)
	{
		if (NULL != parser)
		{
			delete parser;
			parser = NULL;
		}
	}
};


inline double checkValid(double val)
{
	if (val == DBL_MAX || val == FLT_MAX)
		return 0;

	return val;
}

inline std::string toGmExchg(const std::string exchg)
{
	if ("SSE" == exchg)
	{
		return "SHSE";
	}
	else
	{
		return "SZSE";
	}
}

inline std::string fromGmExchg(const std::string emExchg)
{
	if ("SHSE" == emExchg)
	{
		return "SSE";
	}
	else
	{
		return "SZSE";
	}
}


ParserGM::ParserGM()
	: _sink(NULL)
	, _connected(false)
{
}


ParserGM::~ParserGM()
{
}

bool ParserGM::init( WTSVariant* config )
{
	//std::cout << "ParserGM init" << std::endl;

	_strategy_id = config->getCString("strategy_id");
	_token = config->getCString("token");
	//_mode = config->getInt32("mode");

	//_begin_time = config->getCString("begin_time");
	//_end_time = config->getCString("end_time");

	if (_sink)
	{
		write_log(_sink, LL_INFO, "[ParserGM] strategy id: {}", _strategy_id);
		write_log(_sink, LL_INFO, "[ParserGM] token: {}", _token);
		//write_log(_sink, LL_INFO, "[ParserGM] mode: {}", _mode);
		//write_log(_sink, LL_INFO, "[ParserGM] begin_time: {}", _begin_time);
		//write_log(_sink, LL_INFO, "[ParserGM] end_time: {}", _end_time);
	}

	Strategy::set_strategy_id(_strategy_id.c_str());
	Strategy::set_token(_token.c_str());
	//Strategy::set_mode(_mode); // MODE_LIVE MODE_BACKTEST
	Strategy::set_mode(MODE_LIVE);

	//Strategy::set_backtest_config(_begin_time.c_str(), _end_time.c_str(), 0, 
	//	1, 0, 0, 0, 1);

	return true;
}

void ParserGM::release()
{
	//std::cout << "ParserGM release" << std::endl;
}

void ParserGM::subscribe()
{
	//std::cout << "ParserGM subscribe" << std::endl;

	for (auto& code : _set_subs)
	{
		int iResult = Strategy::subscribe(code.c_str(), "tick");
		if (iResult != 0)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserGM] Sending subscribe request failed: {} {}", code.c_str(), iResult);
		}
		else
		{
			if (_sink)
				write_log(_sink, LL_INFO, "[ParserGM] Market data of {} instruments subscribed", code.c_str());
		}
	}
}

bool ParserGM::connect()
{
	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] connect");

	_thrd_parser.reset(new StdThread(boost::bind(&ParserGM::doWork, this)));
	return true;
}


void ParserGM::doWork()
{
	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] do work in thread");

	run();
}

bool ParserGM::disconnect()
{
	//std::cout << "ParserGM disconnect" << std::endl;
	Strategy::stop();
	return true;
}

bool ParserGM::isConnected()
{
	//std::cout << "ParserGM isConnect" << std::endl;
	return _connected;
}


void ParserGM::subscribe( const CodeSet &vecSymbols )
{
	auto cit = vecSymbols.begin();
	for(; cit != vecSymbols.end(); cit++)
	{
		const auto &code = *cit;
		if(_set_subs.find(code) == _set_subs.end())
		{
			_set_subs.insert(code);
		}
	}
}

void ParserGM::unsubscribe(const CodeSet &setSymbols)
{
	//std::cout << "ParserGM unsubscribe" << std::endl;
}

void ParserGM::registerSpi(IParserSpi* listener)
{
	//std::cout << "ParserGM registerSpi" << std::endl << std::flush;

	bool bReplaced = (_sink!=NULL);

	_sink = listener;
	if(_sink)
		_pBaseDataMgr = _sink->getBaseDataMgr();
	
	if(bReplaced && _sink)
	{
		write_log(_sink, LL_WARN, "Listener is replaced");
	}
}

void ParserGM::on_init()
{
	//std::cout << "ParserGM on_init" << std::endl;

	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] on init");
}


void ParserGM::on_market_data_connected()
{
	_connected = true;
	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] connected");
}

void ParserGM::on_market_data_disconnected()
{
	_connected = false;
	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] disconnected");
}

void ParserGM::on_tick(Tick* tick)
{
	//std::cout << "ParserGM on_tick" << std::endl;

	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] on tick");

	if (_pBaseDataMgr == NULL)
	{
		return;
	}

	uint64_t ltime = tick->created_at;
	time_t now = ltime / 1000;

	tm* tNow = localtime(&now);
	uint64_t date = (tNow->tm_year + 1900) * 10000 + (tNow->tm_mon + 1) * 100 + tNow->tm_mday;
	uint64_t time = tNow->tm_hour * 10000 + tNow->tm_min * 100 + tNow->tm_sec;
	int64_t data_time = (date * 1000000 + time) * 1000 + ltime % 1000;

	uint32_t actDate = (uint32_t)(data_time / 1000000000);
	uint32_t actTime = data_time % 1000000000;
	uint32_t actHour = actTime / 10000000;

	std::string code, exchg;
	std::string symbol = tick->symbol;

	StringVector ay = StrUtil::split(symbol, ".");
	exchg = fromGmExchg(ay[0]);
	code = ay[1];

	WTSContractInfo* ct = _pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
	if (ct == NULL)
	{
		if (_sink)
			write_log(_sink, LL_ERROR, "[ParserGM] Instrument {}.{} not exists...", exchg.c_str(), code.c_str());
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* wt_tick = WTSTickData::create(code.c_str());
	wt_tick->setContractInfo(ct);
	WTSTickStruct& quote = wt_tick->getTickStruct();
	strcpy(quote.exchg, commInfo->getExchg());

	quote.action_date = actDate;
	quote.action_time = actTime;

	quote.price = checkValid(tick->price);
	quote.open = checkValid(tick->open);
	quote.high = checkValid(tick->high);
	quote.low = checkValid(tick->low);
	quote.total_volume = tick->cum_volume;
	quote.trading_date = _uTradingDate;
	quote.total_turnover = tick->cum_amount;

	//Î¯Âô¼Û¸ñ
	for (int i = 0; i < 10; i++)
	{
		quote.ask_prices[i] = checkValid(tick->quotes[i].ask_price);
		quote.ask_qty[i] = tick->quotes[i].ask_volume;

		quote.bid_prices[i] = checkValid(tick->quotes[i].bid_price);
		quote.bid_qty[i] = tick->quotes[i].bid_volume;
	}

	if (_sink)
		_sink->handleQuote(wt_tick, 1);
}
