cmd_/workspace/src/exercice07/modules.order := {   echo /workspace/src/exercice07/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/exercice07/modules.order
