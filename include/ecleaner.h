//
// Created by yuhang on 2025/4/27.
//
#pragma once

#include <endstone/plugin/plugin.h>
#include <string>
#include <nlohmann/json.hpp>
#include "translate.hpp"
#include <endstone/endstone.hpp>

using json = nlohmann::json;
extern translate Tran;

extern const string data_path;
extern const std::string config_path;
extern bool next_clean;
extern shared_ptr<endstone::Task> auto_clean_task;

//config
extern bool auto_item_clean;
extern bool auto_entity_clean;
extern bool item_clean_whitelist;
extern vector<string> item_clean_list;
extern bool entity_clean_whitelist;
extern vector<string> entity_clean_list;
extern int clean_tps;
extern int clean_time;
extern int last_entity;

class ECleaner : public endstone::Plugin {
public:
    //数据目录和配置文件检查
    void datafile_check() const;

    // 读取配置文件
    [[nodiscard]] json read_config() const;

    //清理掉落物
    [[nodiscard]] int clean_item() const;

    //清理实体
    [[nodiscard]] int clean_entity() const;

    //自动检查当前服务器状态是否可执行清理
    void check_server_run_clean() const;

    //执行清理
    void run_clean() const;

    //手动执行掉落物清理
    void run_clean_item() const;

    //手动执行实体清理
    void run_clean_entity() const;

    //定期清理
    void auto_clean();

    //Endstone
    void onLoad() override;

    void onEnable() override;

    void onDisable() override;

    bool onCommand(endstone::CommandSender &sender, const endstone::Command &command, const std::vector<std::string> &args) override;

    //ecl菜单
    void ecl_main_menu(endstone::Player& player);
};