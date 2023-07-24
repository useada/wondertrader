/*!
 * \file TraderGM.cpp
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#include "TraderGM.h"

#include "../Includes/IBaseDataMgr.h"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSSessionInfo.hpp"
#include "../Includes/WTSTradeDef.hpp"
#include "../Includes/WTSError.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Share/ModuleHelper.hpp"

#include <boost/bind.hpp>

#include <boost/filesystem.hpp>

//#ifdef _WIN32
//#pragma comment(lib, "../API/gmsdk3.5.17/lib/win32/gmsdk.lib")
//#elif  _WIN64
#pragma comment(lib, "../API/gmsdk3.5.17/lib/win64/gmsdk.lib")
//#endif

 //By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"
template<typename... Args>
inline void write_log(ITraderSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	const char* buffer = fmtutil::format(format, args...);
	sink->handleTraderLog(ll, buffer);
}


extern "C"
{
	EXPORT_FLAG ITraderApi* createTrader()
	{
		//std::cout << "TraderGM create trader" << std::endl;
		std::string module = "gmsdk";
		std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "lib");
		DllHandle hInst = DLLHelper::load_library(dllpath.c_str());
		if (hInst == NULL)
		{
			int err = ::GetLastError();
			std::cout << "[TraderGM] load " << dllpath << " failed, err=" << err << std::endl;
		}
		else
		{
			std::cout << "[TraderGM] load " << dllpath << " success" << std::endl;
		}


		TraderGM *instance = new TraderGM();
		return instance;
	}

	EXPORT_FLAG void deleteTrader(ITraderApi* &trader)
	{
		if (NULL != trader)
		{
			delete trader;
			trader = NULL;
		}
	}
}


struct TimeData
{
	uint32_t date;
	uint32_t time;
	uint32_t hour;
};


inline TimeData getTimeDateFromTimeStamp(uint64_t ts, bool isSeconds = false)
{
	time_t now = ts;
	if (! isSeconds)
	{
		now = now / 1000;
	}

	tm* tNow = localtime(&now);
	uint64_t date = (tNow->tm_year + 1900) * 10000 + (tNow->tm_mon + 1) * 100 + tNow->tm_mday;
	uint64_t time = tNow->tm_hour * 10000 + tNow->tm_min * 100 + tNow->tm_sec;
	int64_t data_time = (date * 1000000 + time) * 1000; // 毫秒
	if (! isSeconds)
	{
		data_time += ts % 1000;
	}

	TimeData td;

	td.date = uint32_t(data_time / 1000000000);
	td.time = data_time % 1000000000;
	td.hour = td.time / 10000000;

	return td;
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


inline WTSDirectionType wrapPosDirection(PositionSide side)
{
	switch (side)
	{
	case PositionSide_Short: return WDT_SHORT;
	default:
		return WDT_LONG;
	}
}

inline PositionSide wrapDirectionType(WTSDirectionType dirType, WTSOffsetType offsetType)
{
	if (WDT_LONG == dirType)
		if (offsetType == WOT_OPEN)
			return PositionSide_Long;
		else
			return PositionSide_Short;
	else
		if (offsetType == WOT_OPEN)
			return PositionSide_Short;
		else
			return PositionSide_Long;
}

inline WTSDirectionType wrapDirectionType(PositionSide side, PositionEffect pe = PositionEffect_Unknown)
{
	if (PositionSide_Long == side)
		if (pe == PositionEffect_Open)
			return WDT_LONG;
		else
			return WDT_SHORT;
	else
		if (pe == PositionEffect_Open)
			return WDT_SHORT;
		else
			return WDT_LONG;
}

inline PositionEffect wrapOffsetType(WTSOffsetType offType)
{
	switch (offType)
	{
	case WOT_OPEN:
		return PositionEffect_Open;
	case WOT_CLOSE:
		return PositionEffect_Close;
	case WOT_CLOSETODAY:
		return PositionEffect_CloseToday;
	case WOT_CLOSEYESTERDAY:
		return PositionEffect_CloseYesterday;
	default:
		return PositionEffect_Close;
	}
}

inline WTSOffsetType wrapOffsetType(PositionSide side, PositionEffect offType = PositionEffect_Unknown)
{
	if (PositionEffect_Open == offType)
		return WOT_OPEN;
	else if (PositionEffect_Close == offType)
		return WOT_CLOSE;
	else if (PositionEffect_CloseToday == offType)
		return WOT_CLOSETODAY;
	else if (PositionEffect_CloseYesterday == offType)
		return WOT_CLOSEYESTERDAY;
	else
		return WOT_FORCECLOSE;
}

inline WTSPriceType wrapPriceType(OrderType orderType)
{
	if (OrderType_Limit == orderType)
		return WPT_LIMITPRICE;
	else 
		return WPT_ANYPRICE;
}

inline WTSOrderState wrapOrderState(OrderStatus orderStatus)
{
	switch (orderStatus)
	{
	case OrderStatus_Suspended:
	case OrderStatus_PendingNew:
		return WOS_NotTraded_NotQueuing;
	case OrderStatus_Filled:
		return WOS_AllTraded;
	case OrderStatus_PartiallyFilled:
		return WOS_PartTraded_Queuing;
	case OrderStatus_New:
		return WOS_NotTraded_Queuing;
	case OrderStatus_Expired:
	case OrderStatus_Canceled:
		return WOS_Canceled;
	default:
		return WOS_Nottouched;
	}
}

inline uint32_t makeRefID()
{
	static std::atomic<uint32_t> auto_refid(0);
	if(auto_refid == 0)
		auto_refid = (uint32_t)((TimeUtils::getLocalTimeNow() - TimeUtils::makeTime(20220101, 0)) / 1000 * 100);
	return auto_refid.fetch_add(1);
}

// ============================================================================

TraderGM::TraderGM()
	: _sink(NULL)
	, _ordref(makeRefID())
	, _reqid(1)
	, _orders(NULL)
	, _trades(NULL)
	, _positions(NULL)
	, _bd_mgr(NULL)
	, _tradingday(0)
	, _connected(false)
	, _ready(false)
{
}


TraderGM::~TraderGM()
{
}

bool TraderGM::init(WTSVariant *config)
{
	//std::cout << "[TraderGM] init" << std::endl << std::flush;

	_strategy_id = config->getCString("strategy_id");
	_token = config->getCString("token");
	//_mode = config->getInt32("mode");
	//_initial_cash = config->getDouble("initial_cash");

	write_log(_sink, LL_INFO, "[TraderGM] strategy id: {}", _strategy_id);
	write_log(_sink, LL_INFO, "[TraderGM] token: {}", _token);
	//write_log(_sink, LL_INFO, "[TraderGM] mode: {}", _mode);
	//write_log(_sink, LL_INFO, "[TraderGM] initial_cash: {}", _initial_cash);

	Strategy::set_strategy_id(_strategy_id.c_str());
	Strategy::set_token(_token.c_str());

	//Strategy::set_mode(_mode);
	//Strategy::set_mode(MODE_BACKTEST);
	Strategy::set_mode(MODE_LIVE);

	//Strategy::set_backtest_config("2023-07-07 09:30:00", "2023-07-07 15:00:00", 1000000, 1, 0, 0, 0, 1);

	std::string user = _strategy_id;

	{
		// 初始化订单标记缓存器
		std::stringstream ss;
		ss << "./gmdata/local/";
		std::string path = StrUtil::standardisePath(ss.str());
		if (!StdFile::exists(path.c_str()))
			boost::filesystem::create_directories(path.c_str());
		ss << user << "_eid.sc";

		_oidCache.init(ss.str().c_str(), _tradingday, [this](const char* message) {
			write_log(_sink, LL_WARN, message);
		});
	}

	{
		// 初始化委托单缓存器
		std::stringstream ss;
		ss << "./gmdata/local/";
		std::string path = StrUtil::standardisePath(ss.str());
		if (!StdFile::exists(path.c_str()))
			boost::filesystem::create_directories(path.c_str());
		ss << user << "_oid.sc";

		_eidCache.init(ss.str().c_str(), _tradingday, [this](const char* message) {
			write_log(_sink, LL_WARN, message);
		});
	}


	return true;
}

void TraderGM::release()
{
	if (_orders)
		_orders->clear();

	if (_positions)
		_positions->clear();

	if (_trades)
		_trades->clear();
}

void TraderGM::registerSpi(ITraderSpi *listener)
{
	_sink = listener;
	if (_sink)
	{
		_bd_mgr = listener->getBaseDataMgr();
	}
}

void TraderGM::connect()
{
	write_log(_sink, LL_INFO, "[TraderGM] connect");

	// start api thread
	_thrd_api.reset(new StdThread([this]() {

			write_log(_sink, LL_DEBUG, "[TraderGM] start api thread ...");
			run();

		}));
}

void TraderGM::disconnect()
{
	release();
}

bool TraderGM::isConnected()
{
	return _connected;
}

int TraderGM::login(const char* user, const char* pass, const char* productInfo)
{
	return 0;
}

int TraderGM::logout()
{
	return 0;
}

bool TraderGM::makeEntrustID(char* buffer, int length)
{
	if (buffer == NULL || length == 0)
		return false;

	try
	{
		uint32_t orderref = _ordref.fetch_add(1) + 1;
		//fmtutil::format_to(buffer, "{}#{}#{}", _user, _tradingday, orderref);

		//std::string user = _token;
		std::string user = _strategy_id;
		fmtutil::format_to(buffer, "{}#{}#{}", user, _tradingday, orderref);

		write_log(_sink, LL_DEBUG, "make entrust id: {}", buffer);
		return true;
	}
	catch (...)
	{

	}

	return false;
}

int TraderGM::orderInsert(WTSEntrust* entrust)
{
	write_log(_sink, LL_DEBUG, "[TraderGM] to order insert: entrust id={}", entrust->getEntrustID());

	if (! (_ready && _connected))
	{
		write_log(_sink, LL_ERROR, "[TraderGM] Order inserting failed: not connected");
		return -1;
	}

	std::string symbol = toGmExchg(entrust->getExchg());
	symbol += ".";
	symbol += entrust->getCode();

	auto side = wrapDirectionType(entrust->getDirection(), entrust->getOffsetType());
	auto orderType = OrderType_Limit;
	auto positionEffect = wrapOffsetType(entrust->getOffsetType());

	write_log(_sink, LL_DEBUG, "[TraderGM] order insert: entrust id={}, side={}, orderType={}, positionEffect={}",
		entrust->getEntrustID(), side, orderType, positionEffect);

	Order order = place_order(symbol.c_str(), entrust->getVolume(), side, orderType, positionEffect, entrust->getPrice());
	if (order.status == OrderStatus_Rejected)
	{
		write_log(_sink, LL_ERROR, "[TraderGM] Order inserting failed: entrust id={}, reason={}",
			entrust->getEntrustID(), order.ord_rej_reason_detail);
		return -1;
	}

	std::string uniq_id = genOrderUniqID(order.cl_ord_id);
	_oidCache.put(uniq_id.c_str(), entrust->getEntrustID(), 0, [this](const char* message) {
		write_log(_sink, LL_WARN, message);
	});

	if (strlen(entrust->getUserTag()) > 0)
	{
		_eidCache.put(entrust->getEntrustID(), entrust->getUserTag(), 0, [this](const char* message) {
			write_log(_sink, LL_WARN, message);
		});
	}

	write_log(_sink, LL_DEBUG, "[TraderGM] order insert success: entrust id={}, order id={}, status={}, usertag={}",
		entrust->getEntrustID(), order.cl_ord_id, order.status, entrust->getUserTag());
	return 0;
}

int TraderGM::orderAction(WTSEntrustAction* action)
{
	write_log(_sink, LL_DEBUG, "[TraderGM] order action: order id={}", action->getOrderID());

	if (! (_ready && _connected))
	{
		write_log(_sink, LL_ERROR, "[TraderGM] Order cancelling failed: not connected");
		return -1;
	}

	int ret = order_cancel(action->getOrderID());
	if (ret != 0)
	{
		write_log(_sink, LL_ERROR, "[TraderGM] Order cancelling failed: order id={}, ret={}", action->getOrderID(), ret);
		return -1;
	}

	return 0;
}

int TraderGM::queryAccount()
{
	write_log(_sink, LL_INFO, "[TraderGM] to query account ...");

	if (! (_ready && _connected))
	{
		write_log(_sink, LL_ERROR, "[TraderGM] query account failed: not connected");
		return -1;
	}


	{
		StdUniqueLock lock(_mtxQuery);
		_queQuery.push([this]() {
			DataArray<Cash>* cashes = get_cash();
			if (cashes == NULL)
			{
				write_log(_sink, LL_ERROR, "[TraderGM] Account querying failed");
				return -1;
			}

			write_log(_sink, LL_DEBUG, "[TraderGM] Account querying count={}", cashes->count());
			WTSArray * ay = WTSArray::create();

			for (auto i=0; i<cashes->count(); i++)
			{
				WTSAccountInfo* accountInfo = makeAccountInfo(&cashes->at(i));
				if (accountInfo != nullptr)
				{
					write_log(_sink, LL_DEBUG, "[TraderGM] Account querying available={}, balance={}",
						accountInfo->getAvailable(), accountInfo->getBalance());
					ay->append(accountInfo, false);
				}
			}

			if (_sink)
				_sink->onRspAccount(ay);

			ay->release();
		});
	}

	return 0;
}

int TraderGM::queryPositions()
{
	write_log(_sink, LL_DEBUG, "[TraderGM] to query positions ...");

	if (! (_ready && _connected))
	{
		write_log(_sink, LL_ERROR, "[TraderGM] query position failed: not connected");
		return -1;
	}

	{
		StdUniqueLock lock(_mtxQuery);

		_queQuery.push([this]() {

			DataArray<Position>* positions = get_position();
			if (positions == nullptr)
			{
				write_log(_sink, LL_ERROR, "[TraderGM] Positions querying failed");
				return -1;
			}

			write_log(_sink, LL_DEBUG, "[TraderGM] Positions querying count={}", positions->count());
			for (auto i=0; i<positions->count(); i++)
			{
				OnQueryPosition(&positions->at(i));
			}

			WTSArray* ayPos = WTSArray::create();
			if (_positions && _positions->size() > 0)
			{
				for (auto it = _positions->begin(); it != _positions->end(); it++)
				{
					WTSPositionItem* tmp = (WTSPositionItem*)it->second;
					ayPos->append(it->second, true);

					write_log(_sink, LL_DEBUG, "[TraderGM] Positions querying code={}, pos={}", tmp->getCode(), tmp->getTotalPosition());
				}
			}

			if (_sink)
				_sink->onRspPosition(ayPos);

			ayPos->release();
		});
	}

	return 0;
}

int TraderGM::queryOrders()
{
	write_log(_sink, LL_DEBUG, "[TraderGM] to query orders ...");

	if (! (_ready && _connected))
	{
		write_log(_sink, LL_ERROR, "[TraderGM] query order failed: not connected");
		return -1;
	}

	{
		StdUniqueLock lock(_mtxQuery);

		_queQuery.push([this]() {
			DataArray<Order>* gm_orders = get_orders();
			if (gm_orders == nullptr)
			{
				write_log(_sink, LL_ERROR, "[TraderGM] Orders querying failed");
				return -1;
			}

			write_log(_sink, LL_DEBUG, "[TraderGM] Orders querying count={}", gm_orders->count());

			if (nullptr == _orders)
				_orders = WTSArray::create();
			//WTSArray* orders = WTSArray::create();

			for (auto i=0; i< gm_orders->count(); i++)
			{
				Order& gm_order = gm_orders->at(i);


				WTSOrderInfo* orderInfo = makeOrderInfo(&gm_order);
				if (orderInfo)
				{
					_orders->append(orderInfo, false);

					write_log(_sink, LL_DEBUG, "[TraderGM] query order: order id={}, status={}",
						orderInfo->getOrderID(), orderInfo->getOrderState());
				}
			}

			if (_sink)
				_sink->onRspOrders(_orders);

			if (_orders)
				_orders->clear();
		});
	}

	return 0;
}

int TraderGM::queryTrades()
{
	write_log(_sink, LL_DEBUG, "[TraderGM] to query trades ...");

	if (! (_ready && _connected))
	{
		write_log(_sink, LL_ERROR, "[TraderGM] query trade failed: not connected");
		return -1;
	}

	{
		StdUniqueLock lock(_mtxQuery);
		_queQuery.push([this]() {
			DataArray<ExecRpt>* trades = get_execution_reports();
			if (trades == nullptr)
			{
				write_log(_sink, LL_ERROR, "[TraderGM] Trades querying failed");
				return -1;
			}

			write_log(_sink, LL_DEBUG, "[TraderGM] Trades querying count={}", trades->count());

			if (nullptr == _trades)
				_trades = WTSArray::create();

			for (auto i=0; i<trades->count(); i++)
			{
				WTSTradeInfo* trdInfo = makeTradeInfo(&trades->at(i));
				if (trdInfo)
				{
					_trades->append(trdInfo, false);

					write_log(_sink, LL_DEBUG, "[TraderGM] query trade: code={}, trade id={}, volume={}",
						trdInfo->getCode(), trdInfo->getTradeID(), trdInfo->getVolume());
				}
			}

			if (_sink)
				_sink->onRspTrades(_trades);

			if (_trades)
				_trades->clear();
		});
	}


	return 0;
}


// ============================================================================
// ============================================================================
// ============================================================================
// ============================================================================


// not in using at now
WTSEntrust* TraderGM::makeEntrust(Order* order)
{
	std::string code, exchg;
	std::string symbol = order->symbol;
	StringVector ay = StrUtil::split(symbol, ".");

	exchg = fromGmExchg(ay[0]);
	code = ay[1];

	WTSContractInfo* ct = _bd_mgr->getContract(code.c_str(), exchg.c_str());
	if (ct == nullptr)
		return nullptr;

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSEntrust* pRet = WTSEntrust::create( code.c_str(), (uint32_t)order->volume, order->price, ct->getExchg());

	pRet->setContractInfo(ct);
	if (commInfo->isStock())
		pRet->setDirection(WDT_LONG);
	else
		pRet->setDirection(wrapDirectionType(PositionSide(order->position_side), PositionEffect(order->position_effect)));

	pRet->setPriceType(wrapPriceType(OrderType(order->order_type)));

	if (!commInfo->isOption())
		pRet->setOffsetType((order->position_side == PositionSide_Long) ? WOT_OPEN : WOT_CLOSE);
	else
		pRet->setOffsetType(wrapOffsetType(PositionSide(order->position_side), PositionEffect(order->position_effect)));

	pRet->setOrderFlag(WOF_NOR);

	std::string uniq_id = genOrderUniqID(order->cl_ord_id);
	const char* eid = _oidCache.get(uniq_id.c_str());

	//genEntrustID(pRet->getEntrustID(), order->cl_ord_id);
	fmtutil::format_to(pRet->getEntrustID(), "{}", eid);

	const char* usertag = _eidCache.get(pRet->getEntrustID());
	if (strlen(usertag) > 0)
		pRet->setUserTag(usertag);

	return pRet;
}

WTSOrderInfo* TraderGM::makeOrderInfo(Order *order)
{
	std::string code, exchg;
	std::string symbol = order->symbol;

	StringVector ay = StrUtil::split(symbol, ".");
	exchg = fromGmExchg(ay[0]);
	code = ay[1];

	WTSContractInfo* ct = _bd_mgr->getContract(code.c_str(), exchg.c_str());
	if (ct == nullptr)
		return nullptr;

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSOrderInfo* pRet = WTSOrderInfo::create();
	pRet->setContractInfo(ct);

	//pRet->setPrice(order_info->price);
	pRet->setPrice(order->price);

	//pRet->setVolume((uint32_t)order_info->quantity);
	pRet->setVolume((uint32_t)order->volume);

	//pRet->setPriceType(wrapPriceType(order_info->price_type));
	pRet->setPriceType(wrapPriceType(OrderType(order->order_type)));

	pRet->setOrderFlag(WOF_NOR);

	if (commInfo->isStock())
		pRet->setDirection(WDT_LONG);
	else
		pRet->setDirection(wrapDirectionType(PositionSide(order->position_side), PositionEffect(order->position_effect)));

	if (!commInfo->isOption())
		pRet->setOffsetType((order->position_side == PositionSide_Long) ? WOT_OPEN : WOT_CLOSE);
	else
		pRet->setOffsetType(wrapOffsetType(PositionSide(order->position_side), PositionEffect(order->position_effect)));

	//pRet->setVolTraded((uint32_t)order_info->qty_traded);
	pRet->setVolTraded((uint32_t)order->filled_volume);

	//pRet->setVolLeft((uint32_t)order_info->qty_left);
	uint32_t left = order->volume - order->filled_volume;
	pRet->setVolLeft((uint32_t)left);

	pRet->setCode(code.c_str());
	pRet->setExchange(ct->getExchg());

	auto td = getTimeDateFromTimeStamp(order->created_at);

	//pRet->setOrderDate((uint32_t)(order_info->insert_time / 1000000000));
	pRet->setOrderDate(td.date);

	//uint32_t uTime = order_info->insert_time % 1000000000;
	pRet->setOrderTime(TimeUtils::makeTime(pRet->getOrderDate(), td.time));

	//pRet->setOrderState(wrapOrderState(order_info->order_status));
	pRet->setOrderState(wrapOrderState(OrderStatus(order->status)));

	if (order->status == OrderStatus_Rejected)
		pRet->setError(true);

	fmtutil::format_to(pRet->getOrderID(), "{}", order->cl_ord_id);
	//wt_strcpy(pRet->getOrderID(), order->cl_ord_id, 0);

	pRet->setStateMsg("");

	std::string uniq_id = genOrderUniqID(order->cl_ord_id);
	const char* eid = _oidCache.get(uniq_id.c_str());

	//genEntrustID(pRet->getEntrustID(), order->cl_ord_id);
	//fmtutil::format_to(pRet->getEntrustID(), "{}", eid);
	wt_strcpy(pRet->getEntrustID(), eid, 0);

	const char* usertag = _eidCache.get(pRet->getEntrustID());
	write_log(_sink, LL_DEBUG, "order fetch usertag: entrust id={}, order id={}, tag={}",
		pRet->getEntrustID(), order->cl_ord_id, usertag);

	if (strlen(usertag) == 0)
	{
		pRet->setUserTag(pRet->getEntrustID());
	}
	else
	{
		pRet->setUserTag(usertag);
	}

	return pRet;
}

WTSTradeInfo* TraderGM::makeTradeInfo(ExecRpt* trade)
{
	std::string code, exchg;
	std::string symbol = trade->symbol;

	StringVector ay = StrUtil::split(symbol, ".");
	exchg = fromGmExchg(ay[0]);
	code = ay[1];

	WTSContractInfo* ct = _bd_mgr->getContract(code.c_str(), exchg.c_str());
	if (ct == nullptr)
		return nullptr;

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTradeInfo *pRet = WTSTradeInfo::create(code.c_str(), exchg.c_str());
	//pRet->setVolume((uint32_t)trade_info->quantity);
	pRet->setVolume((uint32_t)trade->volume);

	//pRet->setPrice(trade_info->price);
	pRet->setPrice(trade->price);

	//pRet->setTradeID(trade_info->exec_id);
	pRet->setTradeID(trade->exec_id);

	pRet->setContractInfo(ct);

	auto td = getTimeDateFromTimeStamp(trade->created_at);
	pRet->setTradeDate(td.date);
	pRet->setTradeTime(TimeUtils::makeTime(td.date, td.time));


	if (commInfo->isStock())
		pRet->setDirection(WDT_LONG);
	else
		pRet->setDirection(wrapDirectionType(PositionSide(trade->side), PositionEffect(trade->position_effect)));

	if (!commInfo->isOption())
		pRet->setOffsetType((trade->side == PositionSide_Long) ? WOT_OPEN : WOT_CLOSE);
	else
		pRet->setOffsetType(wrapOffsetType(PositionSide(trade->side), PositionEffect(trade->position_effect)));


	fmtutil::format_to(pRet->getRefOrder(), "{}", trade->cl_ord_id);
	pRet->setTradeType(WTT_Common);

	double amount = trade->volume*pRet->getPrice();
	pRet->setAmount(amount);

	std::string uniq_id = genOrderUniqID(trade->cl_ord_id);
	const char* eid = _oidCache.get(uniq_id.c_str());

	const char* usertag = _eidCache.get(eid);
	write_log(_sink, LL_DEBUG, "trade fetch usertag: entrust id={}, order id={}, tag={}",
		eid, trade->cl_ord_id, usertag);

	if (strlen(usertag))
		pRet->setUserTag(usertag);

	return pRet;
}



void TraderGM::OnCancelOrderError(std::string err_info)
{
	WTSError* error = WTSError::create(WEC_ORDERCANCEL, err_info.c_str());
	if (_sink != nullptr)
		_sink->onTraderError(error);
	error->release();
}


void TraderGM::OnQueryPosition(Position *pos)
{
	if (nullptr == _positions)
		_positions = PositionMap::create();

	std::string code, exchg;
	std::string symbol = pos->symbol;

	StringVector ay = StrUtil::split(symbol, ".");
	exchg = fromGmExchg(ay[0]);
	code = ay[1];

	WTSContractInfo* contract = _bd_mgr->getContract(code.c_str(), exchg.c_str());
	if (contract)
	{
		WTSCommodityInfo* commInfo = contract->getCommInfo();
		//std::string key = fmt::format("{}-{}", code.c_str(), position->position_direction);
		std::string key = fmt::format("{}-{}", code.c_str(), pos->side);

		WTSPositionItem* tmp = (WTSPositionItem*)_positions->get(key);
		if (tmp == nullptr)
		{
			tmp = WTSPositionItem::create(code.c_str(), commInfo->getCurrency(), commInfo->getExchg());
			tmp->setContractInfo(contract);
			_positions->add(key, tmp, false);
		}

		//tmp->setDirection(wrapPosDirection(position->position_direction));
		tmp->setDirection(wrapPosDirection(PositionSide(pos->side)));

		//tmp->setNewPosition((double)(position->total_qty - position->yesterday_position));
		tmp->setNewPosition((double)(pos->volume_today));

		//tmp->setPrePosition((double)position->yesterday_position);
		tmp->setPrePosition((double)(pos->volume - pos->volume_today));

		//tmp->setMargin(position->total_qty*position->avg_price);
		tmp->setMargin(pos->amount);

		//tmp->setDynProfit(0);
		tmp->setDynProfit(pos->fpnl);

		//tmp->setPositionCost(position->total_qty*position->avg_price);
		tmp->setPositionCost(pos->cost);

		//tmp->setAvgPrice(position->avg_price);
		tmp->setAvgPrice(pos->vwap);

		tmp->setAvailNewPos(0);

		//tmp->setAvailPrePos((double)position->sellable_qty);
		tmp->setAvailPrePos((double)pos->available_now);
	}
}

WTSAccountInfo* TraderGM::makeAccountInfo(Cash *cash)
{
	WTSAccountInfo* accountInfo = WTSAccountInfo::create();

	//accountInfo->setPreBalance(asset->orig_banlance);
	accountInfo->setPreBalance(0);

	accountInfo->setCloseProfit(0);
	accountInfo->setDynProfit(0);
	accountInfo->setMargin(0);

	//accountInfo->setAvailable(asset->buying_power);
	accountInfo->setAvailable(cash->available);

	//accountInfo->setCommission(asset->fund_sell_fee);
	accountInfo->setCommission(cash->cum_commission);

	//accountInfo->setFrozenMargin(asset->withholding_amount);
	accountInfo->setFrozenMargin(cash->order_frozen);

	accountInfo->setFrozenCommission(0);

	//if (asset->deposit_withdraw > 0)
	//	accountInfo->setDeposit(asset->deposit_withdraw);
	//else if (asset->deposit_withdraw < 0)
	//	accountInfo->setWithdraw(0);

	//accountInfo->setBalance(asset->total_asset);
	accountInfo->setBalance(cash->nav);

	accountInfo->setCurrency("CNY");

	return accountInfo;
}

//void TraderGM::genEntrustID(char* buffer, char* orderRef)
//{
//	//这里不再使用sessionid，因为每次登陆会不同，如果使用的话，可能会造成不唯一的情况
//	//fmtutil::format_to(buffer, "{}#{}#{}", _user, _tradingday, orderRef);
//
//	std::string user = _token;
//	fmtutil::format_to(buffer, "{}#{}#{}", user.c_str(), _tradingday, orderRef);
//}

//bool TraderGM::extractEntrustID(const char* entrustid, uint32_t &orderRef)
//{
//	auto idx = StrUtil::findLast(entrustid, '#');
//	if (idx == std::string::npos)
//		return false;
//
//	orderRef = strtoul(entrustid + idx + 1, NULL, 10);
//	return true;
//}

uint32_t TraderGM::genRequestID()
{
	return _reqid.fetch_add(1) + 1;
}

std::string	TraderGM::genOrderUniqID(std::string orderID)
{
	char order_id[64];
	wt_strcpy(order_id, orderID.c_str(), 8);
	return order_id;
}

// ============================================================================
// ============================================================================
// ============================================================================
// ============================================================================

void TraderGM::on_init()
{
	write_log(_sink, LL_INFO, "[TraderGM] on init");

	//time_t t = now();
	//tm* tNow = localtime(&t);
	//uint64_t date = (tNow->tm_year + 1900) * 10000 + (tNow->tm_mon + 1) * 100 + tNow->tm_mday;
	//_tradingday = date;
	uint64_t ts = now();
	TimeData td = getTimeDateFromTimeStamp(ts, true);
	_tradingday = td.date;

	if (_thrd_worker == NULL)
	{
		_thrd_worker.reset(new StdThread([this]() {
			//while (!_bStopped)
			while (true)
			{
				if (_queQuery.empty() || !_connected)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					continue;
				}

				CommonExecuter& handler = _queQuery.front();
				handler();

				{
					StdUniqueLock lock(_mtxQuery);
					_queQuery.pop();
				}
			}
		}));
	}

	_ready = true;
	write_log(_sink, LL_INFO, "[TraderGM] on init tradingDate={}", _tradingday);

	if (_sink)
	{
		_sink->onLoginResult(true, "", _tradingday);
	}
}

void TraderGM::on_trade_data_connected()
{
	_connected = true;
	if (_sink)
	{
		_sink->handleEvent(WTE_Connect, 0);
	}

	write_log(_sink, LL_INFO, "[TraderGM] trade data connected");
}

//void TraderGM::OnDisconnected(uint64_t session_id, int reason)
//{
//	if (_sink)
//		_sink->handleEvent(WTE_Close, reason);
//
//	_asyncio.post([this](){
//		write_log(_sink, LL_WARN, "[TraderXTP] Connection lost, relogin in 2 seconds...");
//		std::this_thread::sleep_for(std::chrono::seconds(2));
//		doLogin();
//	});
//}
void TraderGM::on_trade_data_disconnected()
{
	_connected = false;
	if (_sink)
	{
		_sink->handleEvent(WTE_Close, -1);
	}

	write_log(_sink, LL_WARN, "[TraderGM] trade data disconnected");
}

//void TraderGM::OnError(XTPRI *error_info)
//{
//	if (_sink && error_info)
//		write_log(_sink,LL_ERROR, error_info->error_msg);
//}
void TraderGM::on_error(int error_code, const char *error_msg)
{
	write_log(_sink, LL_ERROR, "on_error code={}, msg={}", error_code, error_msg);
}


//void TraderGM::OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id)
//{
//	if(IsErrorInfo(error_info))
//	{
//		WTSEntrust* entrust = makeEntrust(order_info);
//
//		WTSError* error = WTSError::create(WEC_ORDERINSERT, error_info->error_msg);
//		_sink->onRspEntrust(entrust, error);
//		error->release();
//
//		entrust->release();
//	}
//	else
//	{
//		WTSOrderInfo *orderInfo = makeOrderInfo(order_info);
//		if (orderInfo)
//		{
//			if (_sink)
//				_sink->onPushOrder(orderInfo);
//
//			orderInfo->release();
//		}
//	}
//}

// gm
void TraderGM::on_order_status(Order *order)
{
	write_log(_sink, LL_DEBUG, "on order: order id={}", order->cl_ord_id);

	WTSOrderInfo *orderInfo = makeOrderInfo(order);
	if (orderInfo != nullptr)
	{
		if (_sink)
			_sink->onPushOrder(orderInfo);

		orderInfo->release();
	}
}

//void TraderGM::OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id)
//{
//	WTSTradeInfo *trdInfo = makeTradeInfo(trade_info);
//	if (trdInfo)
//	{
//		if (_sink)
//			_sink->onPushTrade(trdInfo);
//
//		trdInfo->release();
//	}
//}

void TraderGM::on_execution_report(ExecRpt* trade)
{
	write_log(_sink, LL_DEBUG, "on trade: trade id={}, order id={}", trade->exec_id, trade->cl_ord_id);

	WTSTradeInfo *trdInfo = makeTradeInfo(trade);
	if (trdInfo != nullptr)
	{
		if (_sink)
			_sink->onPushTrade(trdInfo);

		trdInfo->release();
	}
}


