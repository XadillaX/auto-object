/**
 * XadillaX <i@2333.moe> created at 2017-06-17 16:59:52 with ❤
 *
 * Copyright (c) 2017 xcoder.in, all rights reserved.
 */
"use strict";

const util = require("util");

const should = require("should");

const AutoObject = require("../");

describe("# create class", function() {
    it("base create", function() {
        const cls = AutoObject.createClass();
        cls.name.should.equal("AutoClass");
    });

    it("create with name", function() {
        const cls = AutoObject.createClass("MyClass");
        cls.name.should.equal("MyClass");
    });

    it("create instance", function() {
        const Cls = AutoObject.createClass("MyClass");
        const obj = new Cls();
        obj.toString().should.equal("[object MyClass]");
        should(obj.test).equal(undefined);
    });

    it("create instance with constructor", function() {
        const Cls = AutoObject.createClass("MyClass");
        Cls.prototype.$$constructor = function(val) {
            this.val = val;
        };
        const obj = new Cls(233);
        obj.toString().should.equal("[object MyClass]");
        obj.val.should.equal(233);
        should(obj.test).equal(undefined);
    });

    it("should inherit", function(done) {
        const EventEmitter = require("events").EventEmitter;
        const Cls = AutoObject.createClass("Inherit");
        AutoObject.inherits(Cls, EventEmitter);

        Cls.prototype.$$constructor = function() {
            EventEmitter.call(this);
        };
        
        Cls.prototype.$$access = function(name) {
            return name;
        };

        const obj = new Cls();
        obj.toString().should.equal("[object Inherit]");
        if(!process.version.startsWith("v4")) obj.should.be.instanceof(EventEmitter);
        obj.should.be.instanceof(Cls);

        obj.name.should.be.equal("name");
        obj.on("emit", function(val) {
            val.should.equal(233);
            done();
        });

        obj.emit("emit", 233);
    });

    it("should visit functions and values", function() {
        const Cls = AutoObject.createClass("Echo");
        Cls.prototype.$$constructor = function(type, val) {
            this.$$type = type;
            this.$$val = val || 0;
        };
        Cls.prototype.$$access = function(name) {
            if(!name) return undefined;

            const self = this;
            if(name.match(/^plus\d+$/)) {
                return function() {
                    if(name === "plus11") throw new Error("can't be 11");
                    self.$$val += parseInt(name.substr(4));
                    return this;
                };
            }

            if(name === "devil") {
                throw new Error("can't be devil");
            }
            return `${name} is ${this.$$type}.`;
        };

        const obj = new Cls("animal", 220);
        obj.toString().should.equal("[object Echo]");
        obj.dog.should.equal("dog is animal.");
        obj.cat.should.equal("cat is animal.");
        should(obj[""]).equal(undefined);

        should.throws(() => {
            obj.devil;
        }, Error, "can't be devil");

        obj.plus13().should.equal(obj);
        obj.$$val.should.equal(233);
        obj.plus20().plus10().should.equal(obj);
        obj.$$val.should.equal(263);

        should.throws(() => {
            obj.plus11();
        }, Error, "can't be 11");
    });

    it("should overwrite or not overwrite", function() {
        const Cls = AutoObject.createClass("Overwrite");
        Cls.prototype.$$access = function(val) {
            return 233;
        };

        const obj = new Cls();
        for(const key of AutoObject.internalProperties) {
            should(obj[key]).not.equal(233);
        }
        obj.any.should.equal(233);
        obj.any = 234;
        obj.any.should.equal(234);
        delete obj.any;
        obj.any.should.equal(233);
    });
});
