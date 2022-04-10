cmd_/workspace/src/exercice01/modules.order := {   echo /workspace/src/exercice01/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/exercice01/modules.order
