/*!
 * \file ParserGM.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "../Includes/IParserApi.h"
#include "../Share/StdUtils.hpp"
//#include "../API/gmsdk3.2.12/include/strategy.h"
#include "../API/gmsdk3.5.17/include/strategy.h"
#include <queue>

NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;

class ParserGM : public IParserApi, public Strategy
{
public:
	ParserGM();
	~ParserGM();


	//IParserApi 接口
public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;

	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;


	//GM Strategy 回调 
public:
	virtual void on_init() override;
	virtual void on_tick(Tick* tick) override;

	virtual void on_market_data_connected() override; // 数据服务已经连接上
	virtual void on_market_data_disconnected() override; // 数据连接断开了

private:
	void	subscribe();
	void	doWork();

private:
	IParserSpi*				_sink;
	IBaseDataMgr*			_pBaseDataMgr;
	uint32_t				_uTradingDate;

	bool					_connected;

	CodeSet					_set_subs;

	StdThreadPtr			_thrd_parser;

	StdUniqueMutex			_mtx_queue;
	std::queue<std::string>	_send_queue;

	// --------------------------------------------------------

	std::string		_strategy_id;
	std::string		_token;
	//int				_mode;
	//std::string		_begin_time; // 回测开始时间 2023-06-29 09:30:00
	//std::string		_end_time;   // 回测结束时间 2023-06-29 09:30:00

};

