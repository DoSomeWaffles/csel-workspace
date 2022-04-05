cmd_/workspace/src/exercice06/modules.order := {   echo /workspace/src/exercice06/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/exercice06/modules.order
