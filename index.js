/**
 * XadillaX <i@2333.moe> created at 2017-06-17 16:51:20 with ❤
 *
 * Copyright (c) 2017 xcoder.in, all rights reserved.
 */
"use strict";

const util = require("util");

const auto = require("./build/Release/auto");

function rawEmitWarning(msg) {
    process.emitWarning(msg, "AUTO001", "NoAccessFunction");
}

function fakeEmitWarning(msg) {
    console.warn(`(node) [AUTO001] NoAccessFunction: ${msg}`);
}

auto.init({
    emitWarning: typeof process.emitWarning === "function" ?
        rawEmitWarning :
        fakeEmitWarning
});

exports.createClass = auto.createClass;

exports.createObject = function(access) {
    const ret = auto.createObject();
    if(typeof access === "function") {
        Object.defineProperty(ret, "$$access", {
            value: access,
            enumerable: false,
            configurable: false,
            writable: true
        });
    }
    return ret;
};

exports.inherits = function(ctor, superCtor) {
    if(!process.version.startsWith("v4.")) {
        return util.inherits(ctor, superCtor);
    }

    // Fix for Node.js v4 (issue https://github.com/XadillaX/auto-object/issues/1)
    //
    // It's not 100% safe. For an example,
    //
    // ```
    // const obj = new Cls();
    // obj instanceof superCtor; //< false
    // ```
    //
    // **IMPORTANT:** DO NOT CONSIDER VERSION BELOW 4
    for(const key in superCtor.prototype) {
        ctor.prototype[key] = superCtor.prototype[key];
    }
};

exports.internalProperties = auto.internalProperties;
