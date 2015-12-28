/* Comments don't crash ! */
var a = 1; // here
/* or here */
// or here
function f(x) {
    // or here
    /* or // here // */
    return x + a; /* here */ // or here
    // yep, still not crashing
}// weeeeee
