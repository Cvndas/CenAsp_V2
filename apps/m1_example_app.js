/*
============================================================
 ASP M1 Example App: Basic Print and Error Handling
============================================================

  This example demonstrates the basic usage of the ASP print function:

  - Printing different types of values (numbers, objects, functions)
  - Handling errors when calling print incorrectly

  Example output:
  ---------------
    ~~ wow, this is really simple, but it works! ~~
    42
    [object Object]
    function (){}
    Printing nothing fails!
    Printing multiple arguments fails!

  The print function only accepts a single argument. Passing no arguments or multiple arguments will throw an error.
============================================================
*/

print(`
    ~~ wow, this is really simple, but it works! ~~
`);

print(42);

print({foo: 'bar'});

print(function (){});

try {
    print();
} catch (e) {
    print('Printing nothing fails!');
}

try {
    print('more', 32);
} catch (e) {
    print('Printing multiple arguments fails!');
}