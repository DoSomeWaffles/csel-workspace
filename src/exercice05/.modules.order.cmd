cmd_/workspace/src/exercice05/modules.order := {   echo /workspace/src/exercice05/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/exercice05/modules.order
