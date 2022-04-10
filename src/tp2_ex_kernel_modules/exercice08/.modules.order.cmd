cmd_/workspace/src/exercice08/modules.order := {   echo /workspace/src/exercice08/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/exercice08/modules.order
