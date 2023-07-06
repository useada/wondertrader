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

//第三方系统设置留痕信息
GM_CXX_API void set_mfp(const char *mfp);

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


#endif
