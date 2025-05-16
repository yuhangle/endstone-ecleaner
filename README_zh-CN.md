![header](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ECleaner)

 [English](README.md)

## 介绍

ECleaner为一款使用C++编写的实体清理插件，轻量便捷。

## 如何使用

> 安装&配置

* 安装Endstone

此步请查看endstone文档

* 下载&安装ECleaner插件

> Windows平台

前往Releases处下载最新的Windows版本的压缩包,然后解压其中的文件到服务端的plugins目录

> Linux平台

前往Releases处下载最新的Linux版本的压缩包,然后解压其中的文件到服务端的plugins目录

* 配置

首次运行插件后将自动在plugins目录创建ecleaner文件夹,里面会生成配置文件config.json
配置文件的默认配置如下:

```bash
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

`auto_entity_clean` 为自动实体清理项，此项为true时为开启，为false时为关闭。开启时，定时清理和TPS过低触发清理以及执行命令/ecl clean时将会清理实体，关闭时则不清理。

`auto_item_clean` 为自动掉落物清理项，此项为true时为开启，为false时为关闭。开启时，定时清理和TPS过低触发清理以及执行命令/ecl clean时将会清理掉落物，关闭时则不清理。

`clean_time` 为定时清理实体的时间间隔，单位为分钟，设置为0时为关闭定时清理。

`clean_tps` 为触发自动清理的服务器最低平均TPS值，当服务器平均TPS值低于此值时，触发一次自动清理。

`entity_clean_list` 为实体清理名单，其为白名单或黑名单取决于实体清理名单模式，名单内的内容为实体的ID。

`entity_clean_whitelist` 为实体清理模式，其值为true时为实体清理白名单模式，其值为false时为实体清理黑名单模式。当实体清理名单模式为白名单时，实体清理名单以外的实体将会被自动清理；为黑名单时，只有实体清理名单内的实体会被清理。默认配置为黑名单。

`item_clean_list` 为掉落物清理名单，其为白名单或黑名单取决于掉落物清理名单模式，名单内的内容为掉落物的英文名，而非ID。

`item_clean_whitelist` 为掉落物清理模式，其值为true时为掉落物清理白名单模式，其值为false时为掉落物清理黑名单模式。当掉落物清理名单模式为白名单时，掉落物清理名单以外的掉落物将会被自动清理；为黑名单时，只有掉落物清理名单内的实体会被清理。默认配置为白名单。

> 命令用法

**命令列表(命令均为仅管理员可用)**

打开领地菜单

```shell
/ecl
```

打开插件配置管理菜单，可以在此管理插件除实体与掉落物名单外的配置。

```shell
/ecl clean
```

手动执行一次清理，清理配置与自动清理配置相同。

```shell
/ecl clean item
```

手动执行掉落物清理

```shell
/ecl clean entity
```

手动执行实体清理

```shell
/ecl reload
```

重载插件配置
