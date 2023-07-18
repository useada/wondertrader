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

inline std::string toGmCode(const std::string &code)
{
	auto tmp = code;
	StrUtil::replace(tmp, "SSE", "SHSE");
	//StrUtil::replace(code, "SZSE", "SZSE");

	return tmp;
}

ParserGM::ParserGM()
	: _sink(NULL)
	, _uTradingDate(0)
	, _connected(false)
	, _ready(false)
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
	//Strategy::set_mode(MODE_BACKTEST);

	//Strategy::set_backtest_config(_begin_time.c_str(), _end_time.c_str(), 0, 
	//	1, 0, 0, 0, 1);
	//Strategy::set_backtest_config("2023-07-07 09:30:00", "2023-07-07 15:00:00",
	//	1000000, 1, 0, 0, 0, 1);

	return true;
}

void ParserGM::release()
{
	//std::cout << "ParserGM release" << std::endl;
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
		//if (_sink)
		//	write_log(_sink, LL_DEBUG, "[ParserGM] {} to subscribe", code.c_str());

		if(_set_subs.find(code) == _set_subs.end())
		{
			_set_subs.insert(code);
			//if (_sink)
			//	write_log(_sink, LL_INFO, "[ParserGM] {} add to subscribe list", code.c_str());

			//if (_uTradingDate != 0)
			if (_ready)
			{
				auto gmCode = toGmCode(code.c_str());
				int iResult = Strategy::subscribe(gmCode.c_str(), "tick");
				if (iResult != 0)
				{
					if (_sink)
						write_log(_sink, LL_ERROR, "[ParserGM] Sending subscribe request failed: code={}, gmCode={}, error={}",
							code.c_str(), gmCode.c_str(), iResult);
				}
				else
				{
					if (_sink)
						write_log(_sink, LL_INFO, "[ParserGM] Market data of {} instruments subscribed", code.c_str());
				}
			}
		}
	}
}

void ParserGM::subscribeOnInit()
{
	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] to subscribe on init");

	for (auto& code : _set_subs)
	{
		auto gmCode = toGmCode(code.c_str());
		int iResult = Strategy::subscribe(gmCode.c_str(), "tick");
		if (iResult != 0)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserGM] Sending subscribe request failed: code={}, gmCode={}, error={}",
					code.c_str(), gmCode.c_str(), iResult);
		}
		else
		{
			if (_sink)
				write_log(_sink, LL_INFO, "[ParserGM] Market data of {} instruments subscribed", code.c_str());
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

	//time_t t = now() / 1000;
	const time_t t = now();
	const tm* tNow = localtime(&t);
	const uint64_t date = (tNow->tm_year + 1900) * 10000 + (tNow->tm_mon + 1) * 100 + tNow->tm_mday;
	_uTradingDate = date;
	_ready = true;

	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] on init tradingDate={}", _uTradingDate);

	_sink->handleEvent(WPE_Login, 0);

	subscribeOnInit();
}


void ParserGM::on_market_data_connected()
{
	_connected = true;
	if (_sink)
	{
		_sink->handleEvent(WPE_Connect, 0);
		write_log(_sink, LL_INFO, "[ParserGM] connected");
	}
}

void ParserGM::on_market_data_disconnected()
{
	_connected = false;
	if (_sink)
	{
		write_log(_sink, LL_ERROR, "[ParserGM] Market data server disconnected");
		_sink->handleEvent(WPE_Close, 0);
	}
}

void ParserGM::on_tick(Tick* tick)
{
	if (_sink)
		write_log(_sink, LL_INFO, "[ParserGM] on time={}, tick={}, price={}", tick->created_at, tick->symbol, tick->price);

	if (_pBaseDataMgr == NULL)
	{
		return;
	}

	//uint64_t ltime = tick->created_at;
	//time_t now = ltime / 1000;

	double ts = tick->created_at;
	time_t seconds = time_t(ts);
	int micro_seconds = uint64_t(ts * 1000) % 1000;

	tm* tNow = localtime(&seconds);
	uint64_t date = (tNow->tm_year + 1900) * 10000 + (tNow->tm_mon + 1) * 100 + tNow->tm_mday;
	uint64_t time = tNow->tm_hour * 10000 + tNow->tm_min * 100 + tNow->tm_sec;
	int64_t data_time = (date * 1000000 + time) * 1000 + micro_seconds;

	uint32_t actDate = (uint32_t)(data_time / 1000000000);
	uint32_t actTime = data_time % 1000000000;
	//uint32_t actHour = actTime / 10000000;

	//if (_sink)
	//	write_log(_sink, LL_INFO, "[ParserGM] on tick action_date={}, action_time={}, create_at={}, seconds={}, micro_seconds={}",
	//		actDate, actTime, tick->created_at, seconds, micro_seconds);

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

	quote.volume = tick->last_volume;
	quote.turn_over = tick->last_amount;

	quote.total_volume = tick->cum_volume;
	quote.total_turnover = tick->cum_amount;
	quote.trading_date = _uTradingDate;

	//quote.upper_limit = checkValid(market_data->upper_limit_price);
	//quote.lower_limit = checkValid(market_data->lower_limit_price);

	//quote.pre_close = checkValid(market_data->pre_close_price);	

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

	wt_tick->release();
}
