/**
 * XadillaX <i@2333.moe> created at 2017-06-17 19:36:50 with ❤
 *
 * Copyright (c) 2017 xcoder.in, all rights reserved.
 */
"use strict";

const should = require("should");
const lorem  = require("unicode-lorem");

const AutoObject = require("../");

describe("# create object", function() {
    it("should create object", function() {
        const obj = AutoObject.createObject();
        obj.toString().should.equal("[object Object]");
        should(obj.test).equal(undefined);

        obj.$$access = function(name) {
            return name.toUpperCase();
        };

        obj.test.should.equal("TEST");
    });

    it("should create object with parameter", function() {
        const test = function(name) {
            return "you're hacked";
        };
        const obj = AutoObject.createObject(test);
        obj.$$access.should.equal(test);

        obj.any.should.equal("you're hacked");

        for(let i = 0; i < 10; i++) {
            obj[lorem(10).str.join("")].should.equal("you're hacked");
        }

        const key = lorem(10).str.join("");
        obj[key] = key;
        obj[key].should.equal(key);
    });
});
