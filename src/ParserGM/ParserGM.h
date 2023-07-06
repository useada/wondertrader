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


	//IParserApi �ӿ�
public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;

	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;


	//GM Strategy �ص� 
public:
	virtual void on_init() override;
	virtual void on_tick(Tick* tick) override;

	virtual void on_market_data_connected() override; // ���ݷ����Ѿ�������
	virtual void on_market_data_disconnected() override; // �������ӶϿ���

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
	//std::string		_begin_time; // �ز⿪ʼʱ�� 2023-06-29 09:30:00
	//std::string		_end_time;   // �ز����ʱ�� 2023-06-29 09:30:00

};

