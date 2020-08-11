/**
 * @file module_def.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-07-19
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_app_MODULE_DEF_H__
#define __wotsen_app_MODULE_DEF_H__

#include <inttypes.h>
#include <limits.h>

typedef uint64_t appmodule_err_t;
typedef struct appmodule_info_s appmodule_info_t;
typedef struct appmodule_base_info_s appmodule_base_info_t;
typedef enum app_module_status_e app_module_status_t;
typedef enum app_module_run_status_e app_module_run_status_t;

typedef appmodule_err_t (*app_module_fun_t)(appmodule_info_t*);
typedef app_module_run_status_t (*app_module_status_fun_t)(appmodule_info_t*);

///< 起始错误码
#define APP_MODULE_OK 0

///< 最大错误码
#define APP_MODULE_ERROR UINT64_MAX

///< 非法模块标识
#define APP_MODULE_INVALID_IDENTIFIER 0

/**
 * @brief 模块状态码
 * 
 */
typedef enum app_module_status_e {
	e_module_uninit,
	e_module_inited,
	e_module_unruning,
	e_module_runing,
	e_module_bad,
};

/**
 * @brief 模块运行状态码
 * 
 */
typedef enum app_module_run_status_e {
	e_module_run_st_ok,
	e_module_run_st_err,
	e_module_run_st_unknown,
};

/**
 * @brief 模块权限
 * 
 */
typedef enum {
	e_module_permission_cfg_en,
	e_module_permission_cfg_disen,
} app_module_permission_t;

/**
 * @brief 模块类型
 * 
 */
typedef enum {
	e_module_core_type = 0,		///< 系统核心模块
	e_module_other_type,		///< 其他模块
	e_module_pri_ext_type,		///< 私有扩展
	e_module_pub_ext_type,		///< 开放扩展
	e_module_pri_app_type,		///< 私有app
	e_module_pub_app_type,		///< 开放app
} app_module_type_t;

/**
 * @brief 基本信息基类
 * 
 */
#define APP_MODULE_BASE_INFO \
char name[128];			 /* 模块名称 */	\
bool enable;		 /* 使能 */ \
uint64_t identifer;	 /* 唯一标识 */ \
app_module_type_t type;		 /* 类别 */ \
app_module_status_t status;	 /* 状态 */ \
app_module_permission_t permission; /* 权限 */

/**
 * @brief 模块基本信息
 * 
 */
struct appmodule_base_info_s
{
	APP_MODULE_BASE_INFO
};

/**
 * @brief 模块信息
 * 
 */
struct appmodule_info_s {
	APP_MODULE_BASE_INFO
	void *configure;		///< 配置
	void *handle;			///< 句柄
	void *param;			///< 配置，临时变量，每次调用配置修改时会置位
	void *data;				///< 私有数据
};

#endif // !__wotsen_app_MODULE_DEF_H__
