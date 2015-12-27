var b = 0;

function recurse(a) {
  b = a;
  if (99 - a) {
    recurse(a + 1);
  } else {
    return a;
  }
};

recurse(b);
