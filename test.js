var b = 99;
function recurse(a) {
  b = a;
  if (a) {
    recurse(a - 1)
  } else {
    a
  }
};
recurse(b);
