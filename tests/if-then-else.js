/**
 * Demonstrtate that if-then-else statements don't ummm... crash. :-|
 **/

if (true) {
    100;
} else {
    200;
}

if (false) {
    100;
} else {
    200;
}

function f(x) {
    if (x) {
        return x * x;
    } else {
        return x;
    }
}

f(2);
f(0);
f(-1);
