BEGIN{
  found=0
}

{
  if (found) {
    print $0
    if (substr($0, length($0), 1) == ";")
      found = 0
  } else for (i = 1; i <= NF; i++) {
    if ($i == ITEM) {
      found=1
      print $0
      break
    }
  }
}
