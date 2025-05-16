![header](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ECleaner)

[简体中文](README_zh-CN.md)

## Introduction

ECleaner is a lightweight and convenient entity cleaning plugin written in C++.

## How to Use

> Installation & Configuration

* Install Endstone

Please refer to the Endstone documentation for this step.

* Download & Install ECleaner Plugin

> Windows Platform

Go to the Releases page to download the latest Windows version compressed package, and then extract the files in it to the server's `plugins` directory.

> Linux Platform

Go to the Releases page to download the latest Linux version compressed package, and then extract the files in it to the server's `plugins` directory.

* Configuration

After running the plugin for the first time, an `ecleaner` folder will be automatically created in the `plugins` directory, containing the configuration file `config.json`.
The default configuration of the configuration file is as follows:

```json
{
    "auto_entity_clean": true,
    "auto_item_clean": true,
    "clean_time": 15,
    "clean_tps": 16,
    "entity_clean_list": [
        "minecraft:zombie_pigman",
        "minecraft:zombie",
        "minecraft:skeleton",
        "minecraft:bogged",
        "minecraft:slime"
    ],
    "entity_clean_whitelist": false,
    "item_clean_list": [
        "Shulker Box",
        "White Shulker Box",
        "Light Gray Shulker Box",
        "Gray Shulker Box",
        "Black Shulker Box",
        "Brown Shulker Box",
        "Red Shulker Box",
        "Orange Shulker Box",
        "Yellow Shulker Box",
        "Lime Shulker Box",
        "Green Shulker Box",
        "Cyan Shulker Box",
        "Light Blue Shulker Box",
        "Blue Shulker Box",
        "Purple Shulker Box",
        "Magenta Shulker Box",
        "Pink Shulker Box"
    ],
    "item_clean_whitelist": true
}
```

`auto_entity_clean`: This option controls automatic entity cleaning. When set to `true`, automatic entity cleaning is enabled. When set to `false`, it is disabled. When enabled, entities will be cleaned during scheduled cleaning, when the TPS is too low, and when the `/ecl clean` command is executed. When disabled, entities will not be cleaned automatically.

`auto_item_clean`: This option controls automatic item cleaning. When set to `true`, automatic item cleaning is enabled. When set to `false`, it is disabled. When enabled, dropped items will be cleaned during scheduled cleaning, when the TPS is too low, and when the `/ecl clean` command is executed. When disabled, dropped items will not be cleaned automatically.

`clean_time`: This is the time interval for scheduled entity cleaning, in minutes. Setting it to `0` disables scheduled cleaning.

`clean_tps`: This is the minimum average TPS value of the server that triggers automatic cleaning. When the server's average TPS drops below this value, an automatic cleaning will be triggered.

`entity_clean_list`: This is the entity cleaning list. It acts as either a whitelist or a blacklist depending on the entity cleaning list mode. The content of the list is the entity ID.

`entity_clean_whitelist`: This option determines the entity cleaning mode. When set to `true`, it is the entity cleaning whitelist mode. When set to `false`, it is the entity cleaning blacklist mode. When the entity cleaning list mode is whitelist, entities not in the list will be automatically cleaned; when it is blacklist, only entities in the list will be cleaned. The default configuration is blacklist.

`item_clean_list`: This is the dropped item cleaning list. It acts as either a whitelist or a blacklist depending on the dropped item cleaning list mode. The content of the list is the English name of the dropped item, not the ID.

`item_clean_whitelist`: This option determines the dropped item cleaning mode. When set to `true`, it is the dropped item cleaning whitelist mode. When set to `false`, it is the dropped item cleaning blacklist mode. When the dropped item cleaning list mode is whitelist, dropped items not in the list will be automatically cleaned; when it is blacklist, only dropped items in the list will be cleaned. The default configuration is whitelist.

> Command Usage

**Command List (All commands are administrator-only)**

Open the territory menu

```shell
/ecl
```

Open the plugin configuration management menu, where you can manage plugin configurations other than the entity and item lists.

```shell
/ecl clean
```

Manually execute a cleaning operation. The cleaning configuration is the same as the automatic cleaning configuration.

```shell
/ecl clean item
```

Manually execute dropped item cleaning.

```shell
/ecl clean entity
```

Manually execute entity cleaning.
