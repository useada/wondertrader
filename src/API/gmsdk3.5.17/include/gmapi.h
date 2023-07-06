#ifndef GMCPPSDK_H
#define GMCPPSDK_H

#include <iostream>
#include "gmdef.h"

#ifdef WIN32
#ifdef GMSDKC_EXPORTS  
#define GM_CXX_API __declspec(dllexport)
#else
#define GM_CXX_API __declspec(dllimport)
#endif
#else
#define GM_CXX_API
#endif

//结果集合
class DataSet
{
public:
	//获取api调用结果, 0: 成功， 非0: 错误码
	virtual int status() = 0;

	//当status() != 0 时，返回错误描述信息, utf-8编码
	virtual const char* errer_msg() = 0;

	//判断是否已经是到达结果集末尾
	virtual bool is_end() = 0;

	//移到下一条记录
	virtual void next() = 0;

	//获取整型值
	virtual int get_integer(const char *key) = 0;

	//获取长整型值
	virtual long long get_long_integer(const char *key) = 0;

	//获取浮点型值
	virtual double get_real(const char *key) = 0;

	//获取字符串型值
	virtual const char* get_string(const char *key) = 0;

	//释放数据集合
	virtual void release() = 0;

	//打印数据
	virtual const char* debug_string() = 0;
};

//结果数组
template <typename T>
class DataArray
{
public:
	//获取api调用结果, 0: 成功， 非0: 错误码
	virtual int status() = 0;

	//当status() != 0 时，返回错误描述信息, utf-8编码
	virtual const char* errer_msg() = 0;

	//返回结构数组的指针
	virtual T* data() = 0;

	//返回数据的长度
	virtual int count() = 0;

	//返回下标为i的结构引用，从0开始
	virtual T& at(int i) = 0;

	//释放数据集合
	virtual void release() = 0;
};

// 获取sdk版本号
GM_CXX_API const char* get_version();

// 设置token
GM_CXX_API void set_token(const char *token);

// 自定义服务地址
GM_CXX_API void set_serv_addr(const char *addr);

// 自定义服务地址v5版本
GM_CXX_API void set_serv_addr_v5(const char *addr, const char *orgcode, const char *site_id);

// 设置回测线程[1-32]
GM_CXX_API void set_backtest_threadnum(int n);

//第三方系统设置留痕信息
GM_CXX_API void set_mfp(const char *mfp);

GM_CXX_API void set_simtrade_only();

GM_CXX_API void set_account_id(const char *account_ids);

// 查询当前行情快照
GM_CXX_API DataArray<Tick>* current(const char *symbols);

// 查询历史Tick行情
GM_CXX_API DataArray<Tick>* history_ticks(const char *symbols, const char *start_time, const char *end_time, int adjust = 0, const char *adjust_end_time = NULL, bool skip_suspended = true, const char *fill_missing = NULL);

// 查询历史Bar行情
GM_CXX_API DataArray<Bar>* history_bars(const char *symbols, const char *frequency, const char *start_time, const char *end_time, int adjust = 0, const char *adjust_end_time = NULL, bool skip_suspended = true, const char *fill_missing = NULL);

// 查询最新n条Tick行情
GM_CXX_API DataArray<Tick>* history_ticks_n(const char *symbols, int n, const char *end_time = NULL, int adjust = 0, const char *adjust_end_time = NULL, bool skip_suspended = true, const char *fill_missing = NULL);

//查询最新n条Bar行情
GM_CXX_API DataArray<Bar>* history_bars_n(const char *symbols, const char *frequency, int n, const char *end_time = NULL, int adjust = 0, const char *adjust_end_time = NULL, bool skip_suspended = true, const char *fill_missing = NULL);

// 查询历史L2 Tick行情
GM_CXX_API DataArray<Tick>* history_l2ticks(const char *symbols, const char *start_time, const char *end_time, int adjust = 0, const char *adjust_end_time = NULL, bool skip_suspended = true, const char *fill_missing = NULL);

// 查询历史L2 Bar行情
GM_CXX_API DataArray<Bar>* history_l2bars(const char *symbols, const char *frequency, const char *start_time, const char *end_time, int adjust = 0, const char *adjust_end_time = NULL, bool skip_suspended = true, const char *fill_missing = NULL);

// 查询历史L2 逐笔成交
GM_CXX_API DataArray<L2Transaction>* history_l2transactions(const char *symbols, const char *start_time, const char *end_time);

// 查询历史L2 逐笔委托
GM_CXX_API DataArray<L2Order>* history_l2orders(const char *symbols, const char *start_time, const char *end_time);

// 查询历史L2 委托队列(最优价最大50笔委托量)
GM_CXX_API DataArray<L2OrderQueue>* history_l2orders_queue(const char *symbols, const char *start_time, const char *end_time);

// 查询基本面数据
GM_CXX_API DataSet* get_fundamentals(const char *table, const char *symbols, const char *start_date, const char *end_date, const char *fields, const char *filter = NULL, const char *order_by = NULL, int limit = 1000);

// 查询基本面数据最新n条
GM_CXX_API DataSet* get_fundamentals_n(const char *table, const char *symbols, const char *end_date, const char *fields, int n = 1, const char *filter = NULL, const char * order_by = NULL);

// 查询最新交易标的信息
GM_CXX_API DataSet* get_instruments(const char *exchanges = NULL, const char *sec_types = NULL, const char* fields = NULL);

// 查询交易标的历史数据
GM_CXX_API DataSet* get_history_instruments(const char *symbols, const char *start_date, const char *end_date, const char *fields = NULL);

// 查询交易标的基本信息
GM_CXX_API DataSet* get_instrumentinfos(const char *symbols = NULL, const char *exchanges = NULL, const char * sec_types = NULL, const char *names = NULL, const char *fields = NULL);

// 查询指数成份股
GM_CXX_API DataSet* get_constituents(const char *index, const char *trade_date = NULL);

// 查询行业股票列表
GM_CXX_API DataArray<Symbol>* get_industry(const char *code);

// 查询概念板块股票列表
GM_CXX_API DataArray<Symbol>* get_concept(const char *code);

// 查询交易日列表
GM_CXX_API DataArray<TradingDate>* get_trading_dates(const char *exchange, const char *start_date, const char *end_date);

// 返回指定日期的上一个交易日
GM_CXX_API int get_previous_trading_date(const char *exchange, const char *date, char *output_date);

// 返回指定日期的下一个交易日
GM_CXX_API int get_next_trading_date(const char *exchange, const char *date, char *output_date);

// 查询分红送配
GM_CXX_API DataSet* get_dividend(const char *symbol, const char *start_date, const char *end_date);

// 获取连续合约
GM_CXX_API DataSet* get_continuous_contracts(const char *csymbol, const char *start_date, const char *end_date);

// 查询查询品种信息
GM_CXX_API DataArray<VarietyInfo>* get_varietyinfos(const char *variety_names);

// 查询交易时段
GM_CXX_API DataArray<TradingTimes>* get_trading_time(const char *variety_names);

// 查询期权合约
GM_CXX_API DataArray<Symbol>* option_get_symbols_by_exchange(const char *exchange = NULL, const char *trade_date = NULL, const char *call_or_put = NULL);

// 查询实平虚值某档合约
GM_CXX_API DataArray<Symbol>* option_get_symbols_by_in_at_out(const char *underlying_symbol = NULL, const char *trade_date = NULL, int excute_month = 0, const char *call_or_put = NULL, int in_at_out = IN_AT_OUT_NO_SPECIFIED, const char *price_type = NULL, double price = -1);

// 查询期权到期日列表
GM_CXX_API DataArray<TradingDate>* option_get_delisted_dates(const char *underlying_symbol = NULL, const char *trade_date = NULL, int excute_month = 0);

// 查询期权行权价列表
GM_CXX_API DataArray<double>* option_get_exercise_prices(const char *underlying_symbol = NULL, const char *trade_date = NULL, int excute_month = 0, const char *adjust_flag = NULL);

//查询行业分类
GM_CXX_API DataArray<IndustryCategory>* stk_get_industry_category(const char *source = NULL, int level = 0);
//查询行业成分股
GM_CXX_API DataArray<IndustryConstituent>* stk_get_industry_constituents(const char *industry_code, const char *date = NULL);
//查询股票的所属行业
GM_CXX_API DataArray<InstrumentIndustry>* stk_get_instrument_industry(const char *symbols, const char *source = NULL, int level = 0, const char *date = NULL);
//查询指数最新成分股 
GM_CXX_API DataArray<IndexConstituent>* stk_get_index_constituents(const char *index);
//查询指数历史成分股
GM_CXX_API DataArray<IndexConstituent>* stk_get_index_history_constituents(const char *index, const char *start_date = NULL, const char *end_date = NULL);
//查询股票分红送配信息
GM_CXX_API DataArray<StockDividend>* stk_get_dividend(const char *symbol, const char *start_date = NULL, const char *end_date = NULL, const char *date_type = NULL);
//查询股票的复权因子
GM_CXX_API DataArray<AdjFactor>* stk_get_adj_factor(const char *symbol, const char *start_date = NULL, const char *end_date = NULL, const char *base_date = NULL);
//查询连续合约对应的真实合约 
GM_CXX_API DataArray<ContinuousContractsInfo>* fut_get_continuous_contracts(const char *csymbol, const char *start_date = NULL, const char *end_date = NULL);
//查询期货标准品种信息
GM_CXX_API DataArray<FutContractInfo>* fut_get_contract_info(const char *product_codes);
//查询基金复权因子
GM_CXX_API DataArray<FndAdjFactorInfo>* fnd_get_adj_factor(const char *symbol, const char *start_date = NULL, const char *end_date = NULL, const char *base_date = NULL);
//查询基金分红信息
GM_CXX_API DataArray<FndDividendInfo>* fnd_get_dividend(const char *symbol, const char *start_date = NULL, const char *end_date = NULL, const char *date_type = NULL);
//查询基金拆分折算信息
GM_CXX_API DataArray<SplitInfo>* fnd_get_split(const char *symbol, const char *start_date = NULL, const char *end_date = NULL, const char *date_type = NULL);

#endif
