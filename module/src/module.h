/**
 * @file module.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-07-19
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_app_MODULE_H__
#define __wotsen_app_MODULE_H__

#include <mutex>
#include <vector>
#include <memory>
#include <exception>
#include "module_def.h"

namespace wotsen
{

using appmodule_t = AppModuleConfigure;

/**
 * @brief 模块配置
 * 
 */
class AppModuleConfigure {
public:
	/**
	 * @brief Construct a new App Module Configure object
	 * @details 构造模块配置
	 * 
	 * @param info 模块信息，传引用，需要持久内存
	 * @param init_fn 初始化接口
	 * @param run_fn 启动接口
	 * @param chg_fn 变更接口
	 * @param stop_fn 停止接口
	 * @param exit_fn 退出接口
	 * @param run_st_fn 运行时状态
	 */
	AppModuleConfigure(appmodule_info_t *info,
					   app_module_fun_t init_fn,
					   app_module_fun_t run_fn,
					   app_module_fun_t chg_fn,
					   app_module_fun_t stop_fn,
					   app_module_fun_t exit_fn,
					   app_module_status_fun_t run_st_fn) throw(std::logic_error);
	~AppModuleConfigure();

public:
	// 初始化
	appmodule_err_t init(void);
	// 启动
	appmodule_err_t run(void);
	// 修改参数
	appmodule_err_t change(void*);
	// 停止
	appmodule_err_t stop(void);
	// 退出
	appmodule_err_t exit(void);
	// 运行时状态查询
	app_module_run_status_t run_status(void); 
	// 查询模块信息
	const appmodule_base_info_t& module_info(void) const;
	// 模块匹配
	bool match(uint64_t identifier) const;

private:
	std::mutex mtx_;					///< 操作锁
	// TODO:改为只能指针?
	appmodule_info_t *info_;			///< 模块信息
	app_module_fun_t init_;				///< 模块初始化
	app_module_fun_t run_;				///< 模块启动
	app_module_fun_t change_;			///< 模块配置变更
	app_module_fun_t stop_;				///< 模块停止
	app_module_fun_t exit_;				///< 模块卸载
	app_module_status_fun_t run_status_;	///< 模块运行时状态
};

/**
 * @brief 模块管理
 * 
 */
class AppModule
{
public:
	AppModule(const std::initializer_list<std::shared_ptr<appmodule_t>> &li);
	~AppModule();

public:
	// 动态注册模块
	int register_module(const std::shared_ptr<appmodule_t> &module);
	// 动态卸载模块
	void unregister_module(uint64_t identifier);

public:
	// 初始化模块，可指定
	appmodule_err_t init_module(uint64_t identifier = APP_MODULE_INVALID_IDENTIFIER);
	// 运行模块，可指定
	appmodule_err_t run_module(uint64_t identifier = APP_MODULE_INVALID_IDENTIFIER);
	// 停止模块，可指定
	appmodule_err_t stop_module(uint64_t identifier = APP_MODULE_INVALID_IDENTIFIER);
	// 反初始化模块，可指定
	appmodule_err_t finit_module(uint64_t identifier = APP_MODULE_INVALID_IDENTIFIER);
	// 修改模块参数
	appmodule_err_t change_module(uint64_t identifier, void *param);
	// 查询模块信息
	bool query_module(uint64_t identifier, appmodule_info_t &info);
	// 查询所有模块信息
	bool query_module(std::vector<appmodule_info_t> &info);

public:
	std::mutex mtx_;
	std::vector<std::shared_ptr<appmodule_t>> modules_;	///< 模块列表，改为二叉搜索树?
};

#define APP_MODULE(info, init, run, chg, stop, exit, run_st) \
std::make_shared<AppModuleConfigure>(info, init, run, chg, stop, exit, run_st)

} // !wotsen

#endif // !endif __wotsen_app_MODULE_h_