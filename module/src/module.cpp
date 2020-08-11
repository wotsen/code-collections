/**
 * @file module.cpp
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-07-19
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */

#include "module.h"

namespace wotsen
{

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
AppModuleConfigure::AppModuleConfigure(appmodule_info_t *info,
					app_module_fun_t init_fn,
					app_module_fun_t run_fn,
					app_module_fun_t chg_fn,
					app_module_fun_t stop_fn,
					app_module_fun_t exit_fn,
					app_module_status_fun_t run_st_fn) throw(std::logic_error) :
					info_(info),
					init_(init_fn),
					run_(run_fn),
					change_(chg_fn),
					stop_(stop_),
					exit_(exit_fn),
					run_status_(run_st_fn)
{
	if (!info_) {
		throw std::logic_error("module info null");
	} else if (!init_ || !run_ || !change_ || !stop_ || !exit_ || !run_status_) {
		throw std::logic_error("module function null");
	}

	// TODO:更多校验
}

AppModuleConfigure::~AppModuleConfigure()
{
	if (info_->status == e_module_runing)
	{
		stop_(info_);
		info_->status = e_module_inited;
	}

	if (info_->status == e_module_inited)
	{
		exit_(info_);
	}
}


// 初始化
appmodule_err_t AppModuleConfigure::init(void)
{
	appmodule_err_t ret = init_(info_);

	if (APP_MODULE_OK == ret)
	{
		info_->status = e_module_inited;
	}
	else
	{
		info_->status = e_module_uninit;
	}
	

	return ret;
}

// 启动
appmodule_err_t AppModuleConfigure::run(void)
{
	appmodule_err_t ret = run_(info_);

	if (APP_MODULE_OK == ret)
	{
		info_->status = e_module_runing;
	}
	else
	{
		info_->status = e_module_unruning;
	}

	return ret;
}

// 修改参数
appmodule_err_t AppModuleConfigure::change(void *param)
{
	// 设置配置参数
	info_->param = param;

	appmodule_err_t ret = change_(info_);

	// 重置该变量
	info_->param = nullptr;

	return ret;
}

// 停止
appmodule_err_t AppModuleConfigure::stop(void)
{
	appmodule_err_t ret = stop_(info_);

	info_->status = e_module_unruning;

	return ret;
}

// 退出
appmodule_err_t AppModuleConfigure::exit(void)
{
	appmodule_err_t ret = exit_(info_);

	info_->status = e_module_uninit;

	return ret;
}

// 运行时状态查询
app_module_run_status_t AppModuleConfigure::run_status(void)
{
	return run_status_(info_);
}

// 查询模块信息
const appmodule_base_info_t& AppModuleConfigure::module_info(void) const
{
	return *((appmodule_base_info_t*)info_);
}

// 模块匹配
bool AppModuleConfigure::match(uint64_t identifier) const
{
	return identifier == info_->identifer;
}

/**
 * @brief Construct a new App Module:: App Module object
 * 
 * @param li 
 */
AppModule::AppModule(const std::initializer_list<std::shared_ptr<appmodule_t>> &li) : modules_(li)
{

}

AppModule::~AppModule()
{
	modules_.clear();
}

// 动态注册模块
int AppModule::register_module(const std::shared_ptr<appmodule_t> &module)
{
	modules_.push_back(module);
}

// 动态卸载模块
void AppModule::unregister_module(uint64_t identifier)
{
	// modules_.erase()
}

}
