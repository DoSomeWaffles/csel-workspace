cmd_/workspace/src/pilotes/exercice05/modules.order := {   echo /workspace/src/pilotes/exercice05/mymodule.ko; :; } | awk '!x[$$0]++' - > /workspace/src/pilotes/exercice05/modules.order
