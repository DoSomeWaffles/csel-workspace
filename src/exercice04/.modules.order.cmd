cmd_/workspace/src/exercice04/modules.order := {   echo /workspace/src/exercice04/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/exercice04/modules.order
