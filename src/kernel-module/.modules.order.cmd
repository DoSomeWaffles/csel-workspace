cmd_/workspace/src/kernel-module/modules.order := {   echo /workspace/src/kernel-module/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/kernel-module/modules.order
