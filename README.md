# auto-object

[![Version](http://img.shields.io/npm/v/auto-object.svg)](https://www.npmjs.com/package/auto-object)
[![Downloads](http://img.shields.io/npm/dm/auto-object.svg)](https://www.npmjs.com/package/auto-object)
[![License](https://img.shields.io/npm/l/auto-object.svg?style=flat)](https://opensource.org/licenses/MIT)
[![TravisCI](https://travis-ci.org/XadillaX/auto-object.svg)](https://travis-ci.org/XadillaX/auto-object)
[![Dependency](https://david-dm.org/XadillaX/auto-object.svg)](https://david-dm.org/XadillaX/auto-object)

Create an object or a class that may accessed by any property name just like
[Proxy](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Proxy).

eg.

```js
const myObject = autoObject.createObject(function(name) {
    if(name.startsWith("say")) {
        return function() {
            return name.substr(3);
        }
    }

    return name;
});

myObject.foo;       ///< "foo"
myObject.sayFoo();  ///< "Foo"
myObject.sayBar();  ///< "Bar"
```

## Installation

```console
$ npm install --save auto-object
```

## API

Require `auto-object` at first:

```js
const autoObject = require("auto-object");
```

### createClass

Create an auto-object class and get its constructor function:

```js
autoObject.createClass([className])
```

+ `className`: the class name, default to `AutoClass`. (optional)
+ **return**: the special constructor function

Your own constructor function should attached on the prototype chain named
`$$constructor`. And the access function should attached on the prototype chain
named `$$access`. Both of `$$constructor` and `$$access` are optional.

#### What's `$$constructor`

We usually create a class constructor function like this:

```js
function MyClass(val) {
    this.$val = val;
}
```

But `createClass` exactly returns a constructor function which you can't
redefine. So we make `$$constructor` as your own constructor function.

eg.

```js
const MyClass = autoObject.createClass("MyClass");

MyClass.prototype.$$constructor = function(val) {
    this.$$val = val;
};

const ret = new MyClass(233); ///< MyClass { $$val: 233 }
```

#### What's `$$access`

`$$access` is the access function to deal with property accessing. This function
owns a single parameter which means the property name. You should return a
result for the accessing.

eg.

```js
const MyClass = autoObject.createClass("MyClass");

MyClass.prototype.$$constructor = function(val) {
    this.$$val = val;
};

MyClass.prototype.$$access = function(property) {
    const self = this;
    if(property.match(/^plus\d+$/)) {
        return function() {
            self.$$val += parseInt(property.substr(4));
            return $$val;
        };
    } else if(property.match(/^minus\d+$/)) {
        return function() {
            self.$$val -= parseInt(property.substr(5));
            return $$val;
        };
    }

    return "You're hacked";
};

const ret = new MyClass(233);   ///< MyClass { $$val: 233 }
ret.plus5();                    ///< 238
ret.plus2();                    ///< 240
ret.minus10();                  ///< 230
ret.foo;                        ///< "You're hacked"
```

#### Inherits

The result of `createClass` almost like the normal constructor function, it may
inherit from some other class by using `util.inherits` too.

eg.

```js
const MyClass = autoObject.createClass("MyClass");

MyClass.prototype.$$constructor = function() {
    EventEmitter.call(this);
};

util.inherits(MyClass, EventEmitter);

MyClass.prototype.$$access = function(name) {
    const self = this;
    return function(val) {
        self.emit(name, val);
    };
}

const ret = new MyClass();
ret.on("test", function(val) {
    console.log(val);
});
ret.test("Hello world");        // emit("test", "Hello world")
```

### createObject

Create and get an auto-object:

```js
autoObject.createObject([access]);
```

+ `access`: the access function. (optional)
+ **return**: the special auto-object

If you want to change access function lately, you just assign the function to
the `$$access` property of the returned object.

#### What's `$$access`

`$$access` is the access function to deal with property accessing. This function
owns a single parameter which means the property name. You should return a
result for the accessing.

eg.

```js
const obj = autoObject.createObject(function(name) {
    return `You're ${name}`;
});

obj.foo;        ///< "You're foo"
obj.bar;        ///< "You're bar"
```

You can change `$$access` at any time you want to do this.

eg.

```js
const obj = autoObject.createObject();

obj.foo;                        ///< undefined
obj.$$access = function(name) {
    return `You're ${name}`;
};
obj.foo;                        ///< "You're foo"
obj.bar;                        ///< "You're bar"
```

### Reserved Properties and Overwrite Rule

> **This is important!**

Here's a reserved properties list that won't go through the access function:

```js
[
  "constructor",
  "hasOwnProperty",
  "isPrototypeOf",
  "propertyIsEnumerable",
  "toLocaleString",
  "toString",
  "valueOf",
  "__defineGetter__",
  "__defineSetter__",
  "__lookupGetter__",
  "__lookupSetter__",
  "__proto__",
  "inspect",
  "$$constructor",
  "$$access"
]
```

And if you define a property manually in your auto-object, that certain property
won't go through the access function too.

eg.

```js
const obj = autoObject.createObject(function() {
    return "You're hacked";
});

obj.foo;                ///< "You're hacked"
obj.foo = "I'm free";
obj.foo;                ///< "I'm free"
delete obj.foo;
obj.foo;                ///< "You're hacked"
```

### Hacking `util.inherits`*

If you're using Node.js v4.x or you're considering to backward compatible v4.x
in your program, you should not use `util.inherits` to inherit your Auto-Object
class.

As a substitute, you should use `autoObject.inherits` instead. This function is
same as `util.inherits` when you're using v6.x or higher. But if you're using
v4.x, it's a hacking.

The code is like this under v4.x:

```js
for(const key in superCtor.prototype) {
    ctor.prototype[key] = superCtor.prototype[key];
}
```

The behavior is not 100% same as `util.inherits`. So in fact I don't recommend
you use this package under v4.x.

## Contribution

You're welcome to fork and make pull requests!

「雖然我覺得不怎麼可能有人會關注我」
