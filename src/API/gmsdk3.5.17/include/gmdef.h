#ifndef GMCSDK_DEF_H
#define GMCSDK_DEF_H


const unsigned int LEN_ID = 64;
const unsigned int LEN_NAME = 64;
const unsigned int LEN_SYMBOL = 32;
const unsigned int LEN_INFO = 128;
const unsigned int LEN_EXCHANGE = 8;
const unsigned int LEN_ISO_DATATIME = 36;
const unsigned int LEN_FREQUENCY = 8;
const unsigned int DEPTH_OF_QUOTE = 10;
const unsigned int LEN_PARAM = 256;
const unsigned int LEN_COMMENT = 256;
const unsigned int LEN_MAX_ORDER_QUEUE = 50;
const unsigned int LEN_TYPE = 8;
const unsigned int LEN_TIME_LIST = 128;

#define IN_AT_OUT_NO_SPECIFIED 2147483647

enum StrategyMode
{
	MODE_UNDEF = 0,          //未定义， 策略不会运行
	MODE_LIVE = 1,           //实盘与仿真模式
	MODE_BACKTEST = 2        //回测模式
};

enum OrderStatus
{
	OrderStatus_Unknown = 0,
	OrderStatus_New = 1,                   //已报
	OrderStatus_PartiallyFilled = 2,       //部成
	OrderStatus_Filled = 3,                //已成
	OrderStatus_DoneForDay = 4,            //
	OrderStatus_Canceled = 5,              //已撤
	OrderStatus_PendingCancel = 6,         //待撤
	OrderStatus_Stopped = 7,               //
	OrderStatus_Rejected = 8,              //已拒绝
	OrderStatus_Suspended = 9,             //挂起
	OrderStatus_PendingNew = 10,           //待报
	OrderStatus_Calculated = 11,           //
	OrderStatus_Expired = 12,              //已过期
	OrderStatus_AcceptedForBidding = 13,   //
	OrderStatus_PendingReplace = 14,       //
};

//执行回报类型
enum ExecType
{
	ExecType_Unknown = 0,
	ExecType_New = 1,                      //已报
	ExecType_DoneForDay = 4,               //
	ExecType_Canceled = 5,                 //已撤销
	ExecType_PendingCancel = 6,            //待撤销
	ExecType_Stopped = 7,                  //
	ExecType_Rejected = 8,                 //已拒绝
	ExecType_Suspended = 9,                //挂起
	ExecType_PendingNew = 10,              //待报
	ExecType_Calculated = 11,              //
	ExecType_Expired = 12,                 //过期
	ExecType_Restated = 13,                //
	ExecType_PendingReplace = 14,          //
	ExecType_Trade = 15,                   //成交
	ExecType_TradeCorrect = 16,            //
	ExecType_TradeCancel = 17,             //
	ExecType_OrderStatus = 18,             //委托状态
	ExecType_CancelRejected = 19,          //撤单被拒绝
};

//委托拒绝原因
enum OrderRejectReason
{
	OrderRejectReason_Unknown = 0,                           //未知原因
	OrderRejectReason_RiskRuleCheckFailed = 1,               //不符合风控规则 
	OrderRejectReason_NoEnoughCash = 2,                      //资金不足
	OrderRejectReason_NoEnoughPosition = 3,                  //仓位不足
	OrderRejectReason_IllegalAccountId = 4,                  //非法账户ID
	OrderRejectReason_IllegalStrategyId = 5,                 //非法策略ID
	OrderRejectReason_IllegalSymbol = 6,                     //非法交易代码
	OrderRejectReason_IllegalVolume = 7,                     //非法委托量
	OrderRejectReason_IllegalPrice = 8,                      //非法委托价
	OrderRejectReason_AccountDisabled = 10,                  //交易账号被禁止交易
	OrderRejectReason_AccountDisconnected = 11,              //交易账号未连接
	OrderRejectReason_AccountLoggedout = 12,                 //交易账号未登录
	OrderRejectReason_NotInTradingSession = 13,              //非交易时段
	OrderRejectReason_OrderTypeNotSupported = 14,            //委托类型不支持
	OrderRejectReason_Throttle = 15,                         //流控限制
	OrderRejectReason_SymbolSusppended = 16,                 //交易代码停牌
	OrderRejectReason_Internal = 999,                        //内部错误

	CancelOrderRejectReason_OrderFinalized = 101,            //委托已完成
	CancelOrderRejectReason_UnknownOrder = 102,              //未知委托
	CancelOrderRejectReason_BrokerOption = 103,              //柜台设置
	CancelOrderRejectReason_AlreadyInPendingCancel = 104,    //委托撤销中
};

//委托方向
enum OrderSide
{
	OrderSide_Unknown = 0,
	OrderSide_Buy = 1,    //买入
	OrderSide_Sell = 2,    //卖出
};

//委托类型
enum OrderType
{
	OrderType_Unknown = 0,
	OrderType_Limit = 1,    //限价委托
	OrderType_Market = 2,    //市价委托
	OrderType_Stop = 3,    //止损止盈委托
};

//委托时间属性
enum OrderDuration
{
	OrderDuration_Unknown = 0,
	OrderDuration_FAK = 1,  //即时成交剩余撤销(fill and kill)
	OrderDuration_FOK = 2,  //即时全额成交或撤销(fill or kill)
	OrderDuration_GFD = 3,  //当日有效(good for day)
	OrderDuration_GFS = 4,  //本节有效(good for section)
	OrderDuration_GTD = 5,  //指定日期前有效(goodl till date)
	OrderDuration_GTC = 6,  //撤销前有效(good till cancel)
	OrderDuration_GFA = 7,  //集合竞价前有效(good for auction)
};

//委托成交属性
enum OrderQualifier
{
	OrderQualifier_Unknown = 0,
	OrderQualifier_BOC = 1,  //对方最优价格(best of counterparty)
	OrderQualifier_BOP = 2,  //己方最优价格(best of party)
	OrderQualifier_B5TC = 3,  //最优五档剩余撤销(best 5 then cancel)
	OrderQualifier_B5TL = 4,  //最优五档剩余转限价(best 5 then limit)
};

//持仓方向
enum PositionSide
{
	PositionSide_Unknown = 0,
	PositionSide_Long = 1,   //多方向
	PositionSide_Short = 2,   //空方向
};

//开平标志
enum PositionEffect
{
	PositionEffect_Unknown = 0,
	PositionEffect_Open = 1,     //开仓
	PositionEffect_Close = 2,     //平仓,具体语义取决于对应的交易所
	PositionEffect_CloseToday = 3,     //平今仓
	PositionEffect_CloseYesterday = 4,     //平昨仓
};

//资金持仓变更原因
enum CashPositionChangeReason
{
	CashPositionChangeReason_Unknown = 0,
	CashPositionChangeReason_Trade = 1,  //交易
	CashPositionChangeReason_Inout = 2,  //出入金/出入持仓
	CashPositionChangeReason_Dividend = 3,  //分红送股
};

enum AccountState
{
	State_UNKNOWN = 0,       //未知
	State_CONNECTING = 1,    //连接中
	State_CONNECTED = 2,     //已连接
	State_LOGGEDIN = 3,      //已登录
	State_DISCONNECTING = 4, //断开中
	State_DISCONNECTED = 5,  //已断开
	State_ERROR = 6          //错误
};

//算法单状态,暂停/恢复算法单时有效
enum AlgoOrderStatus 
{
	AlgoOrderStatus_Unknown = 0,
	AlgoOrderStatus_Resume = 1,                  //恢复母单
	AlgoOrderStatus_Pause = 2,                   //暂停母单
	AlgoOrderStatus_PauseAndCancelSubOrders = 3  //暂算母单并撤子单
};

//头寸来源(仅适用融券融券)
enum PositionSrc
{
	PositionSrc_Unknown = 0,
	PositionSrc_L1 = 1,          //普通沲
	PositionSrc_L2 = 2           //专项沲
};

//证券类型
enum SecurityType
{
	SecurityType_Unknown = 0,
	SecurityType_Stock = 1,  //股票
	SecurityType_Fund = 2,   //基金
	SecurityType_Index = 3,  //指数
	SecurityType_Future = 4,  //期货
	SecurityType_Option = 5,  //期权
	SecurityType_Credit = 6,  //两融
	SecurityType_Bond = 7,    //债券
	SecurityType_Bond_Convertible = 8 //可债券
};

//业务类型
enum OrderBusiness
{
	OrderBusiness_NORMAL = 0,                         //普通交易                                                           
	OrderBusiness_IPO_BUY = 100,                      //新股申购                                                         
	OrderBusiness_CREDIT_BOM = 200,                   //融资买入(buying on margin)                                       
	OrderBusiness_CREDIT_SS = 201,                    //融券卖出(short selling)                                          
	OrderBusiness_CREDIT_RSBBS = 202,                 //买券还券(repay share by buying share)                            
	OrderBusiness_CREDIT_RCBSS = 203,                 //卖券还款(repay cash by selling share)                            
	OrderBusiness_CREDIT_DRS = 204,                   //直接还券(directly repay share)                                   
	OrderBusiness_CREDIT_DRC = 211,                   //直接还款(directly repay cash)                                    
	OrderBusiness_CREDIT_CPOM = 205,                  //融资平仓(close position on margin)                               
	OrderBusiness_CREDIT_CPOSS = 206,                 //融券平仓(close position on short selling)                        
	OrderBusiness_CREDIT_BOC = 207,                   //担保品买入(buying on collateral)                                 
	OrderBusiness_CREDIT_SOC = 208,                   //担保品卖出(selling on collateral)                                
	OrderBusiness_CREDIT_CI = 209,                    //担保品转入(collateral in)                                        
	OrderBusiness_CREDIT_CO = 210,                    //担保品转出(collateral out)                                       
	OrderBusiness_ETF_BUY = 301,                      //ETF申购(purchase)                                                
	OrderBusiness_ETF_RED = 302,                      //ETF赎回(redemption)                                              
	OrderBusiness_FUND_SUB = 303,                     //基金认购(subscribing)                                            
	OrderBusiness_FUND_BUY = 304,                     //基金申购(purchase)                                               
	OrderBusiness_FUND_RED = 305,                     //基金赎回(redemption)                                             
	OrderBusiness_FUND_CONVERT = 306,                 //基金转换(convert)                                                
	OrderBusiness_FUND_SPLIT = 307,                   //基金分拆(split)                                                  
	OrderBusiness_FUND_MERGE = 308,                   //基金合并(merge)                                                  
	OrderBusiness_BOND_RRP = 400,                     //债券逆回购(reverse repurchase agreement (RRP) or reverse repo)   
	OrderBusiness_BOND_CONVERTIBLE_BUY = 401,         //可转债申购(purchase)                                             
	OrderBusiness_BOND_CONVERTIBLE_CALL = 402,        //可转债转股                                                       
	OrderBusiness_BOND_CONVERTIBLE_PUT = 403,         //可转债回售                                                       
	OrderBusiness_BOND_CONVERTIBLE_PUT_CANCEL = 404   //可转债回售撤销                                                   
};

typedef char Symbol[LEN_SYMBOL];
typedef char TradingDate[LEN_ISO_DATATIME];

struct Bar
{
	char                    symbol[LEN_SYMBOL];
	double                  bob;                          ///bar的开始时间
	double                  eob;                          ///bar的结束时间
	float                   open;                         ///<开盘价
	float                   close;                        ///<收盘价
	float                   high;                         ///<最高价
	float                   low;                          ///<最低价
	double                  volume;                       ///<成交量
	double                  amount;                       ///<成交金额
	float                   pre_close;                    ///昨收盘价，只有日频数据赋值

	long long               position;                     ///<持仓量
	char                    frequency[LEN_FREQUENCY];     ///bar频度
};

struct Quote
{
	float      bid_price;                            ///本档委买价
	long long  bid_volume;                           ///本档委买量
	float      ask_price;                            ///本档委卖价
	long long  ask_volume;                           ///本档委卖量
};

struct Tick
{
	char                   symbol[LEN_SYMBOL];
	double                 created_at;               ///<utc时间，精确到毫秒
	float                  price;                    ///<最新价
	float                  open;                     ///<开盘价
	float                  high;                     ///<最高价
	float                  low;                      ///<最低价
	double                 cum_volume;               ///<成交总量
	double                 cum_amount;               ///<成交总金额/最新成交额,累计值
	long long              cum_position;             ///<合约持仓量(期),累计值
	double                 last_amount;              ///<瞬时成交额
	int                    last_volume;              ///<瞬时成交量
	int                    trade_type;               ///(保留)交易类型,对应多开,多平等类型
	Quote                  quotes[DEPTH_OF_QUOTE];   ///报价, 下标从0开始，0-表示第一档，1-表示第二档，依次类推

};

struct L2Transaction
{
	char                   symbol[LEN_SYMBOL];
	double                 created_at;               ///成交时间，utc时间
	float                  price;                    ///成交价
	long long              volume;					 ///成交量
	char                   side;                     ///内外盘标记
	char                   exec_type;                ///成交类型
	long long              exec_index;               ///成交编号
	long long              ask_order_index;          ///叫卖委托编号
	long long              bid_order_index;          ///叫买委托编号
};

struct L2Order
{
	char                   symbol[LEN_SYMBOL];
	double                 created_at;               ///委托时间，utc时间
	float                  price;                    ///委托价
	long long              volume;					 ///委托量
	char                   side;                     ///买卖方向
	char                   order_type;               ///委托类型
	long long              order_index;              ///委托编号
};

struct L2OrderQueue
{
	char                   symbol[LEN_SYMBOL];
	double                 created_at;                                  ///行情时间，utc时间
	float                  price;                                       ///最优委托价
	long long              volume;					                    ///委托量
	char                   side;                                        ///买卖方向
	int                    queue_orders;                                ///委托量队列中元素个数(最多50)
	int                    queue_volumes[LEN_MAX_ORDER_QUEUE];          ///委托量队列(最多50个，有可能小于50, 有效数据长度取决于queue_orders)
};

struct VarietyInfo
{
	char     variety_name[LEN_NAME];          //品种名称（全部大写），不含交易所前缀。
	int      sec_type;                        //品种类型
	int      sec_type_ext;                    //品种扩展类型
	char     exchange[LEN_EXCHANGE];          //交易所代码
	char     quote_unit[LEN_TYPE];            //报价单位
	double   price_tick;                      //最小变动单位  
	double   multiplier;                      //"合约乘数 (即股票期权的合约单位，商品期权的交易单位，指数期权的合约乘数）"
	int      trade_n;                         //交易制度，0表示T+0，1表示T+1，2表示T+2
	char     option_type;                     //行权方式，E:欧式，A:美式 
};


struct Order
{
	char         strategy_id[LEN_ID];                     //策略ID                                      
	char         account_id[LEN_ID];                      //账号ID                                      
	char         account_name[LEN_NAME];                  //账户登录名                                  

	char         cl_ord_id[LEN_ID];                       //委托客户端ID                                
	char         order_id[LEN_ID];                        //委托柜台ID                     
	char         ex_ord_id[LEN_ID];                       //委托交易所ID   
	char         algo_order_id[LEN_ID];                   //算法母单ID
	int          order_business;                          //业务类型

	char         symbol[LEN_SYMBOL];                      //symbol                                      
	int          side;                                    //买卖方向，取值参考enum OrderSide            
	int          position_effect;                         //开平标志，取值参考enum PositionEffect       
	int          position_side;                           //持仓方向，取值参考enum PositionSide         

	int          order_type;                              //委托类型，取值参考enum OrderType            
	int          order_duration;                          //委托时间属性，取值参考enum OrderDuration    
	int          order_qualifier;                         //委托成交属性，取值参考enum OrderQualifier   
	int          order_src;                               //委托来源，取值参考enum OrderSrc   
	int          position_src;                            //头寸来源（仅适用融资融券），取值参考 enum PositionSrc

	int          status;                                  //委托状态，取值参考enum OrderStatus          
	int          ord_rej_reason;                          //委托拒绝原因，取值参考enum OrderRejectReason
	char         ord_rej_reason_detail[LEN_INFO];         //委托拒绝原因描述                            

	double       price;                                   //委托价格                                    
	double       stop_price;                              //委托止损/止盈触发价格                       

	int          order_style;                             //委托风格，取值参考 enum OrderStyle          
	long long    volume;                                  //委托量                                      
	double       value;                                   //委托额                                      
	double       percent;                                 //委托百分比                                  
	long long    target_volume;                           //委托目标量                                  
	double       target_value;                            //委托目标额                                  
	double       target_percent;                          //委托目标百分比                              

	long long    filled_volume;                           //已成量                                      
	double       filled_vwap;                             //已成均价                                    
	double       filled_amount;                           //已成金额                                    
	double       filled_commission;                       //已成手续费                                  

	long long    created_at;                              //委托创建时间                                
	long long    updated_at;                              //委托更新时间  
};

struct AlgoOrder
{
	char         strategy_id[LEN_ID];                     //策略ID                                      
	char         account_id[LEN_ID];                      //账号ID                                      
	char         account_name[LEN_NAME];                  //账户登录名                                  

	char         cl_ord_id[LEN_ID];                       //委托客户端ID                                
	char         order_id[LEN_ID];                        //委托柜台ID                                  
	char         ex_ord_id[LEN_ID];                       //委托交易所ID
	int          order_business;                          //业务类型

	char         symbol[LEN_SYMBOL];                      //symbol                                      
	int          side;                                    //买卖方向，取值参考enum OrderSide            
	int          position_effect;                         //开平标志，取值参考enum PositionEffect       
	int          position_side;                           //持仓方向，取值参考enum PositionSide         

	int          order_type;                              //委托类型，取值参考enum OrderType            
	int          order_duration;                          //委托时间属性，取值参考enum OrderDuration    
	int          order_qualifier;                         //委托成交属性，取值参考enum OrderQualifier   
	int          order_src;                               //委托来源，取值参考enum OrderSrc    
	int          position_src;                            //头寸来源（仅适用融资融券），取值参考 enum PositionSrc

	int          status;                                  //委托状态，取值参考enum OrderStatus          
	int          ord_rej_reason;                          //委托拒绝原因，取值参考enum OrderRejectReason
	char         ord_rej_reason_detail[LEN_INFO];         //委托拒绝原因描述                            

	double       price;                                   //委托价格                                    
	double       stop_price;                              //委托止损/止盈触发价格                       

	int          order_style;                             //委托风格，取值参考 enum OrderStyle          
	long long    volume;                                  //委托量                                      
	double       value;                                   //委托额                                      
	double       percent;                                 //委托百分比                                  
	long long    target_volume;                           //委托目标量                                  
	double       target_value;                            //委托目标额                                  
	double       target_percent;                          //委托目标百分比                              

	long long    filled_volume;                           //已成量                                      
	double       filled_vwap;                             //已成均价                                    
	double       filled_amount;                           //已成金额                                    
	double       filled_commission;                       //已成手续费     

	char         algo_provider[LEN_NAME];                 //算法AppID
	char         algo_name[LEN_NAME];                     //算法策略名
	char         algo_param[LEN_PARAM];                   //算法策略参数
	int          algo_status;                             //算法策略状态,仅作为AlgoOrder Pause请求入参，取值参考 enum AlgoOrderStatus
	char         algo_comment[LEN_COMMENT];               //算法单备注

	long long    created_at;                              //委托创建时间                                
	long long    updated_at;                              //委托更新时间  
};

struct ExecRpt
{
	char               strategy_id[LEN_ID];                  //策略ID                                                                                                        
	char               account_id[LEN_ID];                   //账号ID                                                       
	char               account_name[LEN_NAME];               //账户登录名                                                                                                    

	char               cl_ord_id[LEN_ID];                    //委托客户端ID                                                                                                  
	char               order_id[LEN_ID];                     //委托柜台ID                                                                                                    
	char               exec_id[LEN_ID];                      //委托回报ID                                                                                                    

	char               symbol[LEN_SYMBOL];                   //symbol                                                                                                        

	int                position_effect;                      //开平标志，取值参考enum PositionEffect                                                                         
	int                side;                                 //买卖方向，取值参考enum OrderSide                                                                              
	int                ord_rej_reason;                       //委托拒绝原因，取值参考enum OrderRejectReason                                                                  
	char               ord_rej_reason_detail[LEN_INFO];      //委托拒绝原因描述                                                                                              
	int                exec_type;                            //执行回报类型, 取值参考enum ExecType                                                                           

	double             price;                                //委托成交价格                                                                                                  
	long long          volume;                               //委托成交量                                                                                                    
	double             amount;                               //委托成交金额                                                                                                  
	double             commission;                           //委托成交手续费                                                                                                
	double             cost;                                 //委托成交成本金额  
	long long          created_at;                           //回报创建时间

};

struct Cash
{
	char          account_id[LEN_ID];                        //账号ID               
	char          account_name[LEN_NAME];                    //账户登录名                                                                   

	int           currency;                                  //币种                                                                         

	double        nav;                                        //净值(cum_inout + cum_pnl + fpnl - cum_commission)                            
	double        pnl;                                        //净收益(nav-cum_inout)                                                        
	double        fpnl;                                       //浮动盈亏(sum(each position fpnl))                                            
	double        frozen;                                     //持仓占用资金                                                                 
	double        order_frozen;                               //挂单冻结资金                                                                 
	double        available;                                  //可用资金                                                                     
															  //no  leverage:  available=(cum_inout + cum_pnl - cum_commission - frozen - order_frozen)        
															  //has leverage:  fpnl     =(fpnl>0 ? fpnl : (frozen < |fpnl|) ? (frozen-|fpnl|) : 0)             
															  //               available=(cum_inout + cum_pnl - cum_commission - frozen - order_frozen + fpnl) 
	double        balance;                                    //资金余额                                                                     
	double        market_value;                               //持仓市值
	double        market_value_long;                          //权利仓市值(期权)
	double        market_value_short;                         //义务仓市值(期权)
	double        used_bail;                                  //已用保证金(期权)
	double        enable_bail;                                //可用保证金(期权)
	double        cum_inout;                                  //累计出入金                                                                   
	double        cum_trade;                                  //累计交易额                                                                   
	double        cum_pnl;                                    //累计平仓收益(没扣除手续费)                                                   
	double        cum_commission;                             //累计手续费                                                                   

	double        last_trade;                                 //上一次交易额                                                                 
	double        last_pnl;                                   //上一次收益                                                                   
	double        last_commission;                            //上一次手续费                                                                 
	double        last_inout;                                 //上一次出入金                                                                 
	int           change_reason;                              //资金变更原因，取值参考enum CashPositionChangeReason                          
	char          change_event_id[LEN_ID];                    //触发资金变更事件的ID     

	long long     created_at;                                 //资金初始时间
	long long     updated_at;                                 //资金变更时间

};

struct Position
{
	char                 account_id[LEN_ID];              //账号ID                                       
	char                 account_name[LEN_NAME];          //账户登录名                                                                                       

	char                 symbol[LEN_SYMBOL];              //symbol                                       
	int                  side;                            //持仓方向，取值参考enum PositionSide          
	long long            volume;                          //总持仓量; 昨持仓量(volume-volume_today)                                                          
	long long            volume_today;                    //今日持仓量                                                                                       
	double               vwap;                            //持仓均价(股票为基于开仓价的持仓均价，期货为基于结算价的持仓均价)  
	double               vwap_diluted;                    //摊薄成本价
	double               vwap_open;                       //基于开仓价的持仓均价(期货)
	double               amount;                          //持仓额(volume*vwap*multiplier)                                                                   

	double               price;                           //当前行情价格                                                                                     
	double               fpnl;                            //持仓浮动盈亏((price-vwap)*volume*multiplier) 
	double               fpnl_diluted;                    //持仓浮动盈亏,基于持仓均价((price-vwap_diluted)*volume*multiplier)
	double               fpnl_open;                       //持仓浮动盈亏,基于开仓均价，适用于期货((price-vwap_open)*volume*multiplier)
	double               cost;                            //持仓成本(vwap*volume*multiplier*margin_ratio)                                                    
	long long            order_frozen;                    //挂单冻结仓位                                                                                     
	long long            order_frozen_today;              //挂单冻结今仓仓位                                                                                 
	long long            available;                       //可用总仓位(volume-order_frozen); 可用昨仓位(available-available_today)                           
	long long            available_today;                 //可用今仓位(volume_today-order_frozen_today)
	long long            available_now;                   //当前可平仓位
	long long            credit_position_sellable_volume; //担保品可卖仓位(两融)
	double               market_value;                    //持仓市值

	double               last_price;                      //上一次成交价                                                                                     
	long long            last_volume;                     //上一次成交量                                                                                     
	long long            last_inout;                      //上一次出入持仓量                                                                                 
	int                  change_reason;                   //仓位变更原因，取值参考enum CashPositionChangeReason                                              
	char                 change_event_id[LEN_ID];         //触发资金变更事件的ID                                                                             

	int                  has_dividend;                    //持仓区间有分红配送   
	long long            created_at;                      //建仓时间
	long long            updated_at;                      //仓位变更时间

};

struct Account 
{
	char            account_id[LEN_ID];           //账号ID
	char            account_name[LEN_NAME];       //账户登录名
	char            title[LEN_NAME];              //账号名称
	char            intro[LEN_INFO];              //账号描述
	char            comment[LEN_INFO];            //账号备注
};

struct AccountStatus
{
	char            account_id[LEN_ID];           //账号ID
	char            account_name[LEN_NAME];       //账户登录名
	int             state;                        //账户状态
	int             error_code;                   //错误码
	char            error_msg[LEN_INFO];          //错误信息
};

struct Parameter
{
	char   key[LEN_ID];          //参数键
	double value;                //参数值
	double min;                  //可设置的最小值
	double max;                  //可设置的最大值
	char   name[LEN_NAME];       //参数名
	char   intro[LEN_INFO];      //参数说明
	char   group[LEN_NAME];      //组名
	bool   readonly;             //是否只读
};

struct Indicator
{
	char      account_id[LEN_ID];             //账号ID
	double    pnl_ratio;                      //累计收益率(pnl/cum_inout)
	double    pnl_ratio_annual;               //年化收益率
	double    sharp_ratio;                    //夏普比率
	double    max_drawdown;                   //最大回撤
	double    risk_ratio;                     //风险比率
	int       open_count;                     //开仓次数
	int       close_count;                    //平仓次数
	int       win_count;                      //盈利次数
	int       lose_count;                     //亏损次数
	double    win_ratio;                      //胜率

	long long  created_at;                    //指标创建时间
	long long  updated_at;                    //指标变更时间
};

struct CollateralInstrument
{
	char                 symbol[LEN_SYMBOL];  //担保证券标的
	char                 name[LEN_NAME];      //名称
	double               pledge_rate;         //折算率
};

struct BorrowableInstrument
{
	char                 symbol[LEN_SYMBOL];        //可融证券标的
	char                 name[LEN_NAME];            //名称
	double               margin_rate_for_cash;      //融资保证金比率
	double               margin_rate_for_security;  //融券保证金比率
};

struct BorrowableInstrumentPosition
{
	char                 symbol[LEN_SYMBOL];        //可融证券标的
	char                 name[LEN_NAME];            //名称
	double               balance;                   //证券余额
	double               available;                 //证券可用
};

struct CreditContract
{
	char symbol[LEN_SYMBOL];              //证券代码 stkcode    
	char name[LEN_NAME];                  //名称
	int orderdate;                        //委托日期                   
	char ordersno[LEN_ID];                //委 托 号                   
	char creditdirect;                    //融资融券方向               
	double orderqty;                      //委托数量                   
	double matchqty;                      //成交数量                   
	double orderamt;                      //委托金额                   
	double orderfrzamt;                   //委托冻结金额               
	double matchamt;                      //成交金额                   
	double clearamt;                      //清算金额                   
	char lifestatus;                      //合约状态                   
	int enddate;                          //负债截止日期               
	int oldenddate;                       //原始的负债截止日期         
	double creditrepay;                   //T日之前归还金额            
	double creditrepayunfrz;              //T日归还金额                
	double fundremain;                    //应还金额                   
	double stkrepay;                      //T日之前归还数量            
	double stkrepayunfrz;                 //T日归还数量                
	double stkremain;                     //应还证券数量               
	double stkremainvalue;                //应还证券市值               
	double fee;                           //融资融券息、费             
	double overduefee;                    //逾期未偿还息、费           
	double fee_repay;                     //己偿还息、费               
	double punifee;                       //利息产生的罚息             
	double punifee_repay;                 //己偿还罚息                 
	double rights;                        //未偿还权益金额             
	double overduerights;                 //逾期未偿还权益             
	double rights_repay;                  //己偿还权益                 
	double lastprice;                     //最新价                     
	double profitcost;                    //浮动盈亏                   
	int sysdate;                          //系统日期                   
	char sno[LEN_ID];                     //合约编号                   
	int lastdate;                         //最后一次计算息费日期       
	int closedate;                        //合约全部偿还日期           
	double punidebts;                     //逾期本金罚息               
	double punidebts_repay;               //本金罚息偿还               
	double punidebtsunfrz;                //逾期本金罚息               
	double punifeeunfrz;                  //逾期息费罚息               
	double punirights;                    //逾期权益罚息               
	double punirights_repay;              //权益罚息偿还               
	double punirightsunfrz;               //逾期权益罚息               
	double feeunfrz;                      //实时偿还利息               
	double overduefeeunfrz;               //实时偿还逾期利息           
	double rightsqty;                     //未偿还权益数量             
	double overduerightsqty;              //逾期未偿还权益数量    
};


struct CreditCash
{
	double fundintrrate;                        //融资利率                 
	double stkintrrate;                         //融券利率                 
	double punishintrrate;                      //罚息利率                 
	char   creditstatus;                        //信用状态                 
	double marginrates;                         //维持担保比例             
	double realrate;                            //实时担保比例             
	double asset;                               //总资产                   
	double liability;                           //总负债                   
	double marginavl;                           //保证金可用数             
	double fundbal;                             //资金余额                 
	double fundavl;                             //资金可用数               
	double dsaleamtbal;                         //融券卖出所得资金         
	double guaranteeout;                        //可转出担保资产           
	double gagemktavl;                          //担保证券市值             
	double fdealavl;                            //融资本金                 
	double ffee;                                //融资息费                 
	double ftotaldebts;                         //融资负债合计             
	double dealfmktavl;                         //应付融券市值             
	double dfee;                                //融券息费                 
	double dtotaldebts;                         //融券负债合计             
	double fcreditbal;                          //融资授信额度             
	double fcreditavl;                          //融资可用额度             
	double fcreditfrz;                          //融资额度冻结             
	double dcreditbal;                          //融券授信额度             
	double dcreditavl;                          //融券可用额度             
	double dcreditfrz;                          //融券额度冻结             
	double rights;                              //红利权益                 
	double serviceuncomerqrights;               //红利权益(在途)           
	double rightsqty;                           //红股权益                 
	double serviceuncomerqrightsqty;            //红股权益(在途)           
	double acreditbal;                          //总额度                   
	double acreditavl;                          //总可用额度               
	double acashcapital;                        //所有现金资产（所有资产、包括融券卖出）     
	double astkmktvalue;                        //所有证券市值（包含融资买入、非担保品）     
	double withdrawable;                        //可取资金                                   
	double netcapital;                          //净资产                                     
	double fcreditpnl;                          //融资盈亏                                   
	double dcreditpnl;                          //融券盈亏                                   
	double fcreditmarginoccupied;               //融资占用保证金                             
	double dcreditmarginoccupied;               //融券占用保证金                             
	double collateralbuyableamt;                //可买担保品资金                             
	double repayableamt;                        //可还款金额                                 
	double dcreditcashavl;                      //融券可用资金     
};

//新股申购额度
struct IPOQI
{
	char exchange[LEN_TYPE];        //市场代码
	double quota;                   //市场配额
	double sse_star_quota;          //上海科创板配额
};

struct IPOInstruments
{
	char     symbol[LEN_SYMBOL];     //申购新股symbol
	double   price;                  //申购价格
	int      min_vol;                //申购最小数量
	int      max_vol;                //申购最大数量
};

struct IPOMatchNumber
{
	char order_id[LEN_ID];           //委托号
	char symbol[LEN_SYMBOL];         //新股symbol
	int   volume;                    //成交数量
	char match_number[LEN_ID];       //申购配号
	int  order_at;                   //委托日期
	int  match_at;                    //配号日期
};

struct IPOLotInfo
{
	char symbol[LEN_SYMBOL];   //新股symbol   
	int order_at;              //委托日期     
	int lot_at;                //中签日期     
	int lot_volume;            //中签数量     
	int give_up_volume;        //放弃数量     
	double price;              //中签价格     
	double amount;             //中签金额     
	double pay_volume;         //已缴款数量   
	double pay_amount;         //已缴款金额   

};

struct SmartReorderParameter
{
	char      symbol[LEN_SYMBOL];
	double    price;
	int       volume;
	int       side;
	int       order_type;
	int       position_effect;
	int       repeat_n;
	int       max_price_offset;
	int       time_out;
	int       time_wait;
	char      account_id[LEN_ID];             //账号ID
	int       reorder_id;                     //出参:返回本次智能单的ID, 可用于撤销智能单
};

struct OptionPreorderValidVolumeRequest
{
	char      symbol[LEN_SYMBOL];
	double    price;
	int       side;
	int       covered_flag;
	char      account_id[LEN_ID];             //账号ID
};

struct OptionPreorderSellMarginRequest
{
	char      symbol[LEN_SYMBOL];
	char      account_id[LEN_ID];             //账号ID
};

struct TradingTimes
{
	char       variety_name[LEN_ID];                     //品种ID
	char       time_trading[LEN_TIME_LIST];              //连续竞价时间
	char       time_callauction[LEN_TIME_LIST];          //集合竞价时间
};

struct BacktestConfig 
{
	BacktestConfig()
	{
		initial_cash = 1000000;
		transaction_ratio = 1;
		commission_ratio = 0;
		commission_unit = 0;
		slippage_ratio = 0;
		option_float_margin_ratio1 = 0.2;
		option_float_margin_ratio2 = 0.4;
		adjust = 0;
		match_mode = 0;
		check_cache = 1;
	}

	char   start_time[LEN_ISO_DATATIME];
	char   end_time[LEN_ISO_DATATIME];
	double initial_cash;
	double transaction_ratio;
	double commission_ratio;
	double commission_unit;
	double slippage_ratio;
	double option_float_margin_ratio1;
	double option_float_margin_ratio2;
	int    adjust;
	int    match_mode;
	int    check_cache;
};


// 基础数据结构 ////////////////////////////////////////////

struct IndustryCategory
{
	// 行业代码
	char industry_code[LEN_ID];
	// 行业名称
	char industry_name[LEN_NAME];
};

struct IndustryConstituent
{
	// 行业代码
	char industry_code[LEN_ID];
	// 行业名称
	char industry_name[LEN_NAME];
	// 成分股票代码, 格式: exchange.sec_id
	char symbol[LEN_SYMBOL];
	// 成分股票名称
	char sec_name[LEN_NAME];
	// 纳入日期, 本地时间, 格式为: YYYY-MM-DD
	char date_in[LEN_ISO_DATATIME];
	// 剔除日期, 本地时间, 格式为: YYYY-MM-DD
	char date_out[LEN_ISO_DATATIME];
};

struct InstrumentIndustry
{
	// 股票代码, 格式: exchange.sec_id
	char symbol[LEN_SYMBOL];
	// 股票名称
	char sec_name[LEN_NAME];
	// 行业代码
	char industry_code[LEN_ID];
	// 行业名称
	char industry_name[LEN_NAME];
};

struct IndexConstituent
{
	// 指数代码
	char index[LEN_SYMBOL];
	// 成分股代码
	char symbol[LEN_SYMBOL];
	// 成分股权重
	double weight;
	// 交易日期, 本地时间, 格式为: YYYY-MM-DD
	char date[LEN_ISO_DATATIME];
};

struct StockDividend
{
	// 股票代码
	char symbol[LEN_SYMBOL];
	// 分配方案, 如现金分红, 送股, 配股, 转增
	char scheme_type[LEN_TYPE];
	// 公告日, 本地时间, 格式为: YYYY-MM-DD
	char pub_date[LEN_ISO_DATATIME];
	// 除权除息日, 本地时间, 格式为: YYYY-MM-DD
	char ex_date[LEN_ISO_DATATIME];
	// 股权登记日, 本地时间, 格式为: YYYY-MM-DD
	char equity_reg_date[LEN_ISO_DATATIME];
	// 现金红利发放日(派息日), 本地时间, 格式为: YYYY-MM-DD
	char cash_pay_date[LEN_ISO_DATATIME];
	// 送（转增）股份到账日, 本地时间, 格式为: YYYY-MM-DD
	char share_acct_date[LEN_ISO_DATATIME];
	// 红股上市日, 送（转增）股份上市交易日, 本地时间, 格式为: YYYY-MM-DD
	char share_lst_date[LEN_ISO_DATATIME];
	// 税后红利（元/10股）
	double cash_af_tax;
	// 税前红利（元/10股）
	double cash_bf_tax;
	// 送股比例, 10:X
	double bonus_ratio;
	// 转增比例, 10:X
	double convert_ratio;
	// 盈余公积金转增比例, 10:X
	double sur_rsv_ratio;
	// 资本公积金转增比例, 10:X
	double cap_rsv_ratio;
	// 股本基准日
	char base_date[LEN_ISO_DATATIME];
	// 股本基数(基准股本)
	double base_share;
	// 配股比例
	double ration_ratio;
	// 配股价格
	double ration_price;
};

struct AdjFactor
{
	// 交易日期
	char trade_date[LEN_ISO_DATATIME];
	// 当日后复权因子, T日后复权因子=T-1日的收盘价/T日前收价
	double adj_factor_bwd;
	// 当日累计后复权因子, T日累计后复权因子=T日后复权因子*T-1日累计后复权因子
	// （第一个累计后复权因子=第一个后复权因子）
	double adj_factor_bwd_acc;
	// 当日前复权因子, T日前复权因子=T日后复权因子/复权基准日后复权因子
	double adj_factor_fwd;
	// 当日累计前复权因子, T日累计前复权因子=T日后复权因子
	// T-1日累计前复权因子=T日后复权因子*T-1日后复权因子
	// （第一个累计前复权因子=最新累计后复权因子）
	double adj_factor_fwd_acc;
};

struct ContinuousContractsInfo
{
	// 标的代码
	char symbol[LEN_SYMBOL];
	// 交易日期
	char trade_date[LEN_ISO_DATATIME];
};

struct FutContractInfo
{
	// 交易品种  --交易品种名称，如：沪深300指数，铝
	char  product_name[LEN_NAME];
	// 交易代码  --交易品种代码，如：IF，AL
	char  product_code[LEN_TYPE];
	// 合约标的 --如：SHSE.000300， AL
	char  underlying_symbol[LEN_SYMBOL];
	// 合约乘数  --如：200，5
	int   multiplier;
	// 交易单位  --如：每点人民币200元，5吨/手
	char  trade_unit[LEN_INFO];
	// 报价单位   --如：指数点，元（人民币）/吨
	char  price_unit[LEN_INFO];
	// 价格最小变动单位  --如：0.2点，5元/吨
	char  price_tick[LEN_INFO];
	// 合约月份  --如：当月、下月及随后两个季月，1～12月
	char  delivery_month[LEN_INFO];
	// 交易时间  --如：“9:30-11:30，13:00-15:00”，“上午9:00－11:30 ，下午1:30－3:00和交易所规定的其他交易时间”
	char  trade_time[LEN_INFO];
	// 涨跌停板幅度  --每日价格最大波动限制，如：“上一个交易日结算价的±10%”，“上一交易日结算价±3%”
	char  price_range[LEN_INFO];
	// 最低交易保证金  --交易所公布的最低保证金比例，如：“合约价值的8%”，“合约价值的5%”
	char  minimum_margin[LEN_INFO];
	// 最后交易日   -- 如：“合约到期月份的第三个星期五，遇国家法定假日顺延”，“合约月份的15日（遇国家法定节假日顺延，春节月份等最后交易日交易所可另行调整并通知）”
	char  last_trade_date[LEN_COMMENT];
	// 交割日期  --如：“同最后交易日”，“最后交易日后连续三个工作日”
	char  delivery_date[LEN_INFO];
	// 交割方式  --如：现金交割，实物交割
	char  delivery_method[LEN_INFO];
	// 交易所名称 --上市交易所名称，如：中国金融期货交易所，上海期货交易所
	char  exchange_name[LEN_INFO];
	// 交易所代码  --上市交易所代码，如：CFFEX，SHFE
	char  exchange[LEN_SYMBOL];

};

struct FndAdjFactorInfo
{
	// 交易日期    --最新交易日的日期
	char trade_date[LEN_ISO_DATATIME];
	// 当日后复权因子 --T日后复权因子=T-1日的收盘价/T日前收价
	double adj_factor_bwd_acc;
	// 当日前复权因子   --T日前复权因子=T日后复权因子/复权基准日后复权因子
	double adj_factor_fwd;
};

struct FndDividendInfo
{
	// 基金代码   --查询分红信息的基金代码
	char fund[LEN_SYMBOL];
	// 公告日
	char pub_date[LEN_ISO_DATATIME];
	// 方案进度
	char event_progress[LEN_COMMENT];
	// 派息比例 --10:X，每10份税前分红
	double dvd_ratio;
	// 分配收益基准日
	char dvd_base_date[LEN_ISO_DATATIME];
	// 权益登记日
	char rt_reg_date[LEN_ISO_DATATIME];
	// 实际除息日
	char ex_act_date[LEN_ISO_DATATIME];
	// 场内除息日
	char ex_dvd_date[LEN_ISO_DATATIME];
	// 场内红利发放日
	char pay_dvd_date[LEN_ISO_DATATIME];
	// 场内红利款账户划出日
	char trans_dvd_date[LEN_ISO_DATATIME];
	// 红利再投资确定日
	char reinvest_cfm_date[LEN_ISO_DATATIME];
	// 红利再投资份额到账日
	char ri_shr_arr_date[LEN_ISO_DATATIME];
	// 红利再投资赎回起始日
	char ri_shr_rdm_date[LEN_ISO_DATATIME];
	// 可分配收益 --单位：元
	double earn_distr;
	// 本期实际红利发放 --单位：元
	double cash_pay;
	// 基准日基金份额净值
	double base_unit_nv;
};

struct SplitInfo
{
	// 基金代码
	char  fund[LEN_SYMBOL];
	// 公告日
	char pub_date[LEN_ISO_DATATIME];
	// 拆分折算类型
	char split_type[LEN_TYPE];
	// 拆分折算比例
	double split_ratio;
	// 拆分折算基准日
	char base_date[LEN_ISO_DATATIME];
	// 拆分折算场内除权日
	char ex_date[LEN_ISO_DATATIME];
	// 基金份额变更登记日
	char share_change_reg_date[LEN_ISO_DATATIME];
	// 基金披露净值拆分折算日
	char nv_split_pub_date[LEN_ISO_DATATIME];
	// 权益登记日
	char rt_reg_date[LEN_ISO_DATATIME];
	// 场内除权日(收盘价)
	char ex_date_close[LEN_ISO_DATATIME];
};

#endif
