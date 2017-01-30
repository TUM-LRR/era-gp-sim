#!/bin/bash

read -d '' process_counts << "EOF"
{
  if (NR == 1)
  {
    cpp["comment"] += $4;
    cpp["code"] += $5;
    next;
  }

  if (NR == 2)
  {
    $1 = "C++";
    $4 += cpp["comment"];
    $5 += cpp["code"];
  }

  print $1, "&", $4, "&", $5, "&", $4 + $5, "\\\\\\\\";

  total["comment"] += $4;
  total["code"] += $5;
}

END {
  print "Alle", "&",
        total["comment"], "&",
        total["code"], "&",
        total["comment"] + total["code"];
}
EOF

cloc source include tests scripts/*.py docs themes \
  | awk 'NR>8 && NR < 19' \
  | awk -F'[[:space:]]{2,}' "$process_counts"
