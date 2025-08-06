//
// Created by yuhang on 2025/4/27.
//

#include "ecleaner.h"

ENDSTONE_PLUGIN("ecleaner", "0.1.3", ECleaner)
{
    description = "a plugin for endstone to clean entity";

    command("ecl")
            .description("ECleaner")
            .usages("/ecl",
                    "/ecl clean [entity|item]",
                    "/ecl reload"
                    )
            .permissions("ecleaner.command.op");

    permission("ecleaner.command.op")
            .description("ecl op command")
            .default_(endstone::PermissionDefault::Operator);
}