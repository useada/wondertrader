/*!
 * \file TraderGM.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once

#include <stdint.h>
#include <atomic>
#include <boost/asio/io_service.hpp>

//#include "../API/XTP2.2.32.2/xtp_trader_api.h"
#include "../API/gmsdk3.5.17/include/strategy.h"

#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/WtKVCache.hpp"

USING_NS_WTP;

class TraderGM: public Strategy, public ITraderApi
{
public:
	TraderGM();
	virtual ~TraderGM();


public:
	virtual void on_trade_data_connected() override;	// 交易已经连接上

	virtual void on_trade_data_disconnected() override; // 交易连接断开了

	//virtual void OnError(XTPRI *error_info) override;
	virtual void on_error(int error_code, const char *error_msg) override;

	//virtual void OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id) override;
	virtual void on_order_status(Order *order) override;

	//virtual void OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id) override;
	virtual void on_execution_report(ExecRpt* trade) override;

	//virtual void OnCancelOrderError(XTPOrderCancelInfo *cancel_info, XTPRI *error_info, uint64_t session_id) override;
	virtual void OnCancelOrderError(std::string err_info);

	//virtual void OnQueryOrder(XTPQueryOrderRsp *order_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) override;
	virtual void OnQueryOrder(Order* order);

	//virtual void OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) override;
	virtual void OnQueryTrade(ExecRpt *trade_info);

	//virtual void OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) override;
	virtual void OnQueryPosition(Position *pos);

	//virtual void OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) override;
	virtual void OnQueryAsset(Cash *cash);

public:
	//////////////////////////////////////////////////////////////////////////
	//ITraderApi 接口
	virtual bool init(WTSVariant *params) override;

	virtual void release() override;

	virtual void registerSpi(ITraderSpi *listener) override;

	virtual void connect() override;

	virtual void disconnect() override;

	virtual bool isConnected() override;

	virtual bool makeEntrustID(char* buffer, int length) override;

	virtual int login(const char* user, const char* pass, const char* productInfo) override;

	virtual int logout() override;

	virtual int orderInsert(WTSEntrust* eutrust) override;

	virtual int orderAction(WTSEntrustAction* action) override;

	virtual int queryAccount() override;

	virtual int queryPositions() override;

	virtual int queryOrders() override;

	virtual int queryTrades() override;

private:
	inline uint32_t			genRequestID();
	void					doLogin();
	void					doWork();

	inline WTSOrderInfo*	makeOrderInfo(Order *order);
	inline WTSEntrust*		makeEntrust(Order *order);
	inline WTSTradeInfo*	makeTradeInfo(ExecRpt* trade);

	inline bool	extractEntrustID(const char* entrustid, uint32_t &orderRef);
	inline void	genEntrustID(char* buffer, char* orderRef);

private:
	ITraderSpi*				_sink;

	typedef WTSHashMap<std::string> PositionMap;
	PositionMap*			_positions;
	WTSArray*				_trades;
	WTSArray*				_orders;

	IBaseDataMgr*			_bd_mgr;

	bool					_connected;
	uint32_t				_tradingday;
	std::atomic<uint32_t>		_reqid;
	std::atomic<uint32_t>		_ordref;		//报单引用

	boost::asio::io_service		_asyncio;
	StdThreadPtr				_thrd_worker;

	//委托单标记缓存器
	WtKVCache		m_eidCache;
	//订单标记缓存器
	WtKVCache		m_oidCache;

	std::string		_strategy_id;
	std::string		_token;
	int				_mode;
	double			_initial_cash;  // 回测初始资金
};

