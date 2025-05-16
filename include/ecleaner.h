//
// Created by yuhang on 2025/4/27.
//
#pragma once

#include <endstone/plugin/plugin.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "translate.h"
#include <endstone/endstone.hpp>

using json = nlohmann::json;
inline translate Tran;

const string data_path = "plugins/ecleaner";
const std::string config_path = "plugins/ecleaner/config.json";
inline bool next_clean = false;
inline shared_ptr<endstone::Task> auto_clean_task;

//config
inline bool auto_item_clean;
inline bool auto_entity_clean;
inline bool item_clean_whitelist;
inline vector<string> item_clean_list;
inline bool entity_clean_whitelist;
inline vector<string> entity_clean_list;
inline int clean_tps;
inline int clean_time;

class ECleaner : public endstone::Plugin {
public:
    //数据目录和配置文件检查
    void datafile_check() const {
        json df_config = {
                {"auto_item_clean", true},
                {"auto_entity_clean", true},
                {"item_clean_whitelist", true},
                {"item_clean_list", {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                                     "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                                     "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                                     "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box", "Magenta Shulker Box",
                                     "Pink Shulker Box"}},
                {"entity_clean_whitelist", false},
                {"entity_clean_list", {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"}},
                {"clean_tps", 16},
                {"clean_time", 15}
        };

        if (!(std::filesystem::exists(data_path))) {
            getLogger().info(Tran.getLocal("No data path,auto create"));
            std::filesystem::create_directory(data_path);
            if (!(std::filesystem::exists(config_path))) {
                std::ofstream file(config_path);
                if (file.is_open()) {
                    file << df_config.dump(4);
                    file.close();
                    getLogger().info(Tran.getLocal("Config file created"));
                }
            }
        } else if (std::filesystem::exists(data_path)) {
            if (!(std::filesystem::exists(config_path))) {
                std::ofstream file(config_path);
                if (file.is_open()) {
                    file << df_config.dump(4);
                    file.close();
                    getLogger().info(Tran.getLocal("Config file created"));
                }
            } else {
                bool need_update = false;
                json loaded_config;

                // 加载现有配置文件
                std::ifstream file(config_path);
                file >> loaded_config;

                // 检查配置完整性并更新
                for (auto& [key, value] : df_config.items()) {
                    if (!loaded_config.contains(key)) {
                        loaded_config[key] = value;
                        getLogger().info(Tran.getLocal("Config '{}' has update with default config")+","+key);
                        need_update = true;
                    }
                }

                // 如果需要更新配置文件，则进行写入
                if (need_update) {
                    std::ofstream outfile(config_path);
                    if (outfile.is_open()) {
                        outfile << loaded_config.dump(4);
                        outfile.close();
                        getLogger().info(Tran.getLocal("Config file update over"));
                    }
                }
            }
        }
    }

    // 读取配置文件
    [[nodiscard]] json read_config() const {
        std::ifstream i(config_path);
        try {
            json j;
            i >> j;
            return j;
        } catch (json::parse_error& ex) { // 捕获解析错误
            getLogger().error( ex.what());
            json error_value = {
                    {"error","error"}
            };
            return error_value;
        }
    }

    //clean function


    //清理掉落物
    [[nodiscard]] int clean_item() const {
        int total_clean_num = 0;
        for (const auto& one_actor:getServer().getLevel()->getActors()) {
            if (one_actor->getType() == "minecraft:item") {
                //白名单模式
                if (item_clean_whitelist) {
                    //物品不在白名单里,杀
                    if (ranges::find(item_clean_list,one_actor->getName()) == item_clean_list.end()) {
                        one_actor->remove();
                        total_clean_num +=1;
                    }
                }
                //黑名单模式
                else {
                    //物品在黑名单里,杀
                    if (ranges::find(item_clean_list,one_actor->getName()) != item_clean_list.end()) {
                        one_actor->remove();
                        total_clean_num +=1;
                    }
                }
            }
        }
        return total_clean_num;
    }

    //清理实体
    [[nodiscard]] int clean_entity() const {
        int total_clean_num = 0;
        for (const auto& one_actor:getServer().getLevel()->getActors()) {
            if (one_actor->getType() != "minecraft:item") {
                //检查有没有名字
                if (!one_actor->getNameTag().empty()) {
                    continue;
                }
                //白名单模式
                if (entity_clean_whitelist) {
                    //实体不在白名单里,杀
                    if (ranges::find(entity_clean_list,one_actor->getType()) == entity_clean_list.end()) {
                        one_actor->remove();
                        total_clean_num +=1;
                    }
                }
                //黑名单模式
                else {
                    //实体在黑名单里,杀
                    if (ranges::find(entity_clean_list,one_actor->getType()) != entity_clean_list.end()) {
                        one_actor->remove();
                        total_clean_num +=1;
                    }
                }
            }
        }
        return total_clean_num;
    }

    //自动检查当前服务器状态是否可执行清理
    void check_server_run_clean() const {
        if (getServer().getOnlinePlayers().empty()) {
            return;
        }
        //tps比清理触发tps高,不清理
        if (static_cast<int>(getServer().getAverageTicksPerSecond()) >= clean_tps) {
            //清理状态为false
            if (!next_clean) {
                return;
            }
        }
        //达到触发清理tps,设置下次清理状态
        if (!next_clean) {
            next_clean = true;
            endstone::CommandSenderWrapper commandSenderWrapper(getServer().getCommandSender());
            if (getServer().dispatchCommand(commandSenderWrapper,"playsound note.banjo @a")) {
            }
            getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("The server's current average TPS has fallen below the set value, and entity cleanup will begin soon."));
            return;
        }

        run_clean();

        //清理完毕
        next_clean = false;
    }

    //执行清理
    void run_clean() const {
        int clean_item_num = 0;
        int clean_entity_num = 0;
        //清理掉落物
        if (auto_item_clean) {
            clean_item_num = clean_item();
            getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("Number of dropped items cleaned up: ") + to_string(clean_item_num));
        }
        //清理实体
        if (auto_entity_clean) {
            clean_entity_num = clean_entity();
            getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("Number of entities cleaned up: ") + to_string(clean_entity_num));
        }
        if (clean_entity_num == 0 && clean_item_num == 0) {
            getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("No entities were cleaned up."));
        }
    }

    //手动执行掉落物清理
    void run_clean_item() const {
        //清理掉落物
        int clean_item_num = clean_item();
        getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("Number of dropped items cleaned up: ") + to_string(clean_item_num));
    }

    //手动执行实体清理
    void run_clean_entity() const {
        //清理实体
        int clean_entity_num = clean_entity();
        getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("Number of entities cleaned up: ") + to_string(clean_entity_num));
    }

    //定期清理
    void auto_clean() {
        if (getServer().getOnlinePlayers().empty()) {
            return;
        }
        endstone::CommandSenderWrapper commandSenderWrapper(getServer().getCommandSender());
        if (getServer().dispatchCommand(commandSenderWrapper,"playsound note.banjo @a")) {
        }
        getServer().broadcastMessage("§l§2 [ECleaner] §r"+endstone::ColorFormat::Yellow+Tran.getLocal("There are 30 seconds remaining until the server entity cleanup begins."));
        getServer().getScheduler().runTaskTimer(*this,[&]() { run_clean(); }, 600, 0);
    }


    //Endstone
    void onLoad() override
    {
        getLogger().info("onLoad is called");
        Tran.loadLanguage();
        datafile_check();
    }

    void onEnable() override
    {
        getLogger().info("onEnable is called");
        getLogger().info(endstone::ColorFormat::Yellow+Tran.getLocal("ECleaner has been enable,version: ")+getServer().getPluginManager().getPlugin("ecleaner")->getDescription().getVersion());

        //进行一个配置文件的读取
        json json_msg = read_config();
        try {
            if (!json_msg.contains("error")) {
                auto_item_clean = json_msg["auto_item_clean"];
                auto_entity_clean = json_msg["auto_entity_clean"];
                item_clean_whitelist = json_msg["item_clean_whitelist"];
                entity_clean_whitelist = json_msg["entity_clean_whitelist"];
                item_clean_list = json_msg["item_clean_list"];
                entity_clean_list = json_msg["entity_clean_list"];
                clean_tps = json_msg["clean_tps"];
                clean_time = json_msg["clean_time"];
            } else {
                getLogger().error(Tran.getLocal("Config file error!Use default config"));
                auto_item_clean = true;
                auto_entity_clean = true;
                item_clean_whitelist = true;
                entity_clean_whitelist = false;
                item_clean_list = {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                                   "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                                   "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                                   "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box",
                                   "Magenta Shulker Box", "Pink Shulker Box"};
                entity_clean_list = {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"};
                clean_tps = 16;
                clean_time = 15;
            }
        } catch (const std::exception& e) {
            auto_item_clean = true;
            auto_entity_clean = true;
            item_clean_whitelist = true;
            entity_clean_whitelist = false;
            item_clean_list = {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                               "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                               "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                               "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box",
                               "Magenta Shulker Box", "Pink Shulker Box"};
            entity_clean_list = {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"};
            clean_tps = 16;
            clean_time = 15;
            getLogger().error(Tran.getLocal("Config file error!Use default config")+","+e.what());
        }
        //5秒检查一次tps,延迟30秒
        getServer().getScheduler().runTaskTimer(*this,[&]() { check_server_run_clean(); }, 0, 100);
        //定时清理
        if (clean_time >= 1) {
            std::uint64_t clean_pre_time = clean_time*60*20;
            auto_clean_task = getServer().getScheduler().runTaskTimer(*this, [&]() { auto_clean(); }, 0, clean_pre_time);
        }
    }

    void onDisable() override
    {
        getLogger().info("onDisable is called");
    }

    bool onCommand(endstone::CommandSender &sender, const endstone::Command &command, const std::vector<std::string> &args) override
    {
        if (command.getName() == "ecl")
        {
            if (!sender.asPlayer()) {
                if (!args.empty()) {
                    if (args[0] == "clean") {
                        if (args[1] == "entity") {
                            run_clean_entity();
                        } else if (args[1] == "item") {
                            run_clean_item();
                        } else {
                            run_clean();
                        }
                    }
                    else if (args[0] == "reload") {
                        //进行一个配置文件的读取
                        json json_msg = read_config();
                        try {
                            if (!json_msg.contains("error")) {
                                auto_item_clean = json_msg["auto_item_clean"];
                                auto_entity_clean = json_msg["auto_entity_clean"];
                                item_clean_whitelist = json_msg["item_clean_whitelist"];
                                entity_clean_whitelist = json_msg["entity_clean_whitelist"];
                                item_clean_list = json_msg["item_clean_list"];
                                entity_clean_list = json_msg["entity_clean_list"];
                                clean_tps = json_msg["clean_tps"];
                                clean_time = json_msg["clean_time"];
                            } else {
                                getLogger().error(Tran.getLocal("Config file error!Use default config"));
                                auto_item_clean = true;
                                auto_entity_clean = true;
                                item_clean_whitelist = true;
                                entity_clean_whitelist = false;
                                item_clean_list = {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                                                   "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                                                   "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                                                   "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box",
                                                   "Magenta Shulker Box", "Pink Shulker Box"};
                                entity_clean_list = {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"};
                                clean_tps = 16;
                                clean_time = 15;
                            }
                        } catch (const std::exception& e) {
                            auto_item_clean = true;
                            auto_entity_clean = true;
                            item_clean_whitelist = true;
                            entity_clean_whitelist = false;
                            item_clean_list = {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                                               "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                                               "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                                               "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box",
                                               "Magenta Shulker Box", "Pink Shulker Box"};
                            entity_clean_list = {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"};
                            clean_tps = 16;
                            clean_time = 15;
                            getLogger().error(Tran.getLocal("Config file error!Use default config")+","+e.what());
                        }
                        //定时清理
                        if (clean_time >= 1) {
                            auto_clean_task->cancel();
                            std::uint64_t clean_pre_time = clean_time*60*20;
                            auto_clean_task = getServer().getScheduler().runTaskTimer(*this, [&]() { auto_clean(); }, 0, clean_pre_time);
                        }
                    }
                }
            }
            else {
                auto player = sender.asPlayer();
                if (args.empty()) {
                    ecl_main_menu(*player);
                }
                else if (args[0] == "clean") {
                    if (args[1] == "entity") {
                        run_clean_entity();
                    } else if (args[1] == "item") {
                        run_clean_item();
                    } else {
                        run_clean();
                    }
                } else if (args[0] == "reload") {
                    //进行一个配置文件的读取
                    json json_msg = read_config();
                    try {
                        if (!json_msg.contains("error")) {
                            auto_item_clean = json_msg["auto_item_clean"];
                            auto_entity_clean = json_msg["auto_entity_clean"];
                            item_clean_whitelist = json_msg["item_clean_whitelist"];
                            entity_clean_whitelist = json_msg["entity_clean_whitelist"];
                            item_clean_list = json_msg["item_clean_list"];
                            entity_clean_list = json_msg["entity_clean_list"];
                            clean_tps = json_msg["clean_tps"];
                            clean_time = json_msg["clean_time"];
                            sender.sendMessage(Tran.getLocal("Reload completed."));
                        } else {
                            sender.sendErrorMessage(Tran.getLocal("Config file error!Use default config"));
                            auto_item_clean = true;
                            auto_entity_clean = true;
                            item_clean_whitelist = true;
                            entity_clean_whitelist = false;
                            item_clean_list = {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                                               "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                                               "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                                               "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box",
                                               "Magenta Shulker Box", "Pink Shulker Box"};
                            entity_clean_list = {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"};
                            clean_tps = 16;
                            clean_time = 15;
                        }
                    } catch (const std::exception& e) {
                        auto_item_clean = true;
                        auto_entity_clean = true;
                        item_clean_whitelist = true;
                        entity_clean_whitelist = false;
                        item_clean_list = {"Shulker Box", "White Shulker Box", "Light Gray Shulker Box", "Gray Shulker Box",
                                           "Black Shulker Box", "Brown Shulker Box", "Red Shulker Box", "Orange Shulker Box",
                                           "Yellow Shulker Box", "Lime Shulker Box", "Green Shulker Box", "Cyan Shulker Box",
                                           "Light Blue Shulker Box", "Blue Shulker Box", "Purple Shulker Box",
                                           "Magenta Shulker Box", "Pink Shulker Box"};
                        entity_clean_list = {"minecraft:zombie_pigman","minecraft:zombie","minecraft:skeleton","minecraft:bogged","minecraft:slime"};
                        clean_tps = 16;
                        clean_time = 15;
                        sender.sendErrorMessage(Tran.getLocal("Config file error!Use default config")+","+e.what());
                    }
                    //定时清理
                    if (clean_time >= 1) {
                        auto_clean_task->cancel();
                        std::uint64_t clean_pre_time = clean_time*60*20;
                        auto_clean_task = getServer().getScheduler().runTaskTimer(*this, [&]() { auto_clean(); }, 0, clean_pre_time);
                    }
                }
            }
        }
        return true;
    }

    //ecl菜单
    void ecl_main_menu(endstone::Player& player) {
        endstone::ModalForm menu;
        menu.setTitle(Tran.getLocal("ECL Config Menu"));
        endstone::Toggle AutoEntityClean;
        endstone::Toggle AutoItemClean;
        endstone::Toggle EntityWhiteList;
        endstone::Toggle ItemWhiteList;
        endstone::Slider CleanTime;
        endstone::Slider CleanTps;

        AutoEntityClean.setLabel(Tran.getLocal("Auto clean entity"));
        AutoEntityClean.setDefaultValue(auto_entity_clean);

        AutoItemClean.setLabel(Tran.getLocal("Auto clean item"));
        AutoItemClean.setDefaultValue(auto_item_clean);

        EntityWhiteList.setLabel(Tran.getLocal("Entity whitelist mode"));
        EntityWhiteList.setDefaultValue(entity_clean_whitelist);

        ItemWhiteList.setLabel(Tran.getLocal("Item whitelist mode"));
        ItemWhiteList.setDefaultValue(item_clean_whitelist);

        CleanTime.setLabel(Tran.getLocal("Scheduled cleanup interval time(min)"));
        CleanTime.setMin(0);
        CleanTime.setMax(60);
        CleanTime.setStep(1);
        CleanTime.setDefaultValue(static_cast<float>(clean_time));

        CleanTps.setLabel(Tran.getLocal("Minimum TPS to trigger automatic cleanup"));
        CleanTps.setMin(1);
        CleanTps.setMax(20);
        CleanTps.setDefaultValue(static_cast<float>(clean_tps));
        CleanTps.setStep(1);

        menu.setControls({AutoEntityClean,AutoItemClean,EntityWhiteList,ItemWhiteList,CleanTime,CleanTps});
        menu.setOnSubmit([=](endstone::Player* p, const string& response) {
            json response_json = json::parse(response);
            bool auto_entity_clean_new = response_json[0].get<bool>();
            bool auto_item_clean_new = response_json[1].get<bool>();
            bool entity_clean_whitelist_new = response_json[2].get<bool>();
            bool item_clean_whitelist_new = response_json[3].get<bool>();
            int clean_time_new = response_json[4].get<int>();
            int clean_tps_new = response_json[5].get<int>();

            bool need_update = false;
            auto tasks = getServer().getScheduler().getPendingTasks();
            json updated_config;

            // 加载现有配置文件
            std::ifstream file(config_path);
            file >> updated_config;

            // 比较并更新全局变量和配置文件
            if (auto_entity_clean != auto_entity_clean_new) {
                auto_entity_clean = auto_entity_clean_new;
                updated_config["auto_entity_clean"] = auto_entity_clean;
                need_update = true;
            }
            if (auto_item_clean != auto_item_clean_new) {
                auto_item_clean = auto_item_clean_new;
                updated_config["auto_item_clean"] = auto_item_clean;
                need_update = true;
            }
            if (entity_clean_whitelist != entity_clean_whitelist_new) {
                entity_clean_whitelist = entity_clean_whitelist_new;
                updated_config["entity_clean_whitelist"] = entity_clean_whitelist;
                need_update = true;
            }
            if (item_clean_whitelist != item_clean_whitelist_new) {
                item_clean_whitelist = item_clean_whitelist_new;
                updated_config["item_clean_whitelist"] = item_clean_whitelist;
                need_update = true;
            }
            if (clean_time != clean_time_new) {
                clean_time = clean_time_new;
                updated_config["clean_time"] = clean_time;
                need_update = true;
                if (clean_time >= 1) {
                    auto_clean_task->cancel();
                    std::uint64_t clean_pre_time = clean_time*60*20;
                    auto_clean_task = getServer().getScheduler().runTaskTimer(*this, [&]() { auto_clean(); }, 0, clean_pre_time);
                }
            }
            if (clean_tps != clean_tps_new) {
                clean_tps = clean_tps_new;
                updated_config["clean_tps"] = clean_tps;
                need_update = true;
            }

            // 如果需要更新配置文件，则进行写入
            if (need_update) {
                std::ofstream outfile(config_path);
                if (outfile.is_open()) {
                    outfile << updated_config.dump(4);
                    outfile.close();
                    p->sendMessage(Tran.getLocal("Config file update over"));
                }
            }
        });
        player.sendForm(menu);
    }
};