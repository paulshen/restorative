// Generated by BUCKLESCRIPT VERSION 5.0.6, PLEASE EDIT WITH CARE
'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var ReactDOMRe = require("reason-react/src/ReactDOMRe.js");
var Restorative$ReactHooksTemplate = require("./Restorative.bs.js");

function Index$Example(Props) {
  return React.createElement("div", undefined, "Hello");
}

var Example = /* module */[/* make */Index$Example];

ReactDOMRe.renderToElementWithId(React.createElement(Index$Example, { }), "root");

function myReducer(state, action) {
  return /* record */[/* count */state[/* count */0] + 1 | 0];
}

var match = Restorative$ReactHooksTemplate.create(/* record */[/* count */1], myReducer);

var dispatch = match[/* dispatch */3];

var subscribeWithSelector = match[/* subscribeWithSelector */2];

var subscribe = match[/* subscribe */1];

var getState = match[/* getState */0];

console.log(Curry._1(getState, /* () */0));

Curry._3(subscribe, (function (state) {
        console.log(state);
        return /* () */0;
      }), undefined, /* () */0);

Curry._4(subscribeWithSelector, (function (count) {
        console.log(count);
        return /* () */0;
      }), (function (state) {
        return state[/* count */0];
      }), undefined, /* () */0);

Curry._4(subscribeWithSelector, (function (count) {
        console.log(count);
        return /* () */0;
      }), (function (state) {
        return String(state[/* count */0] + 1 | 0);
      }), undefined, /* () */0);

Curry._1(dispatch, /* Increment */0);

exports.Example = Example;
exports.myReducer = myReducer;
exports.getState = getState;
exports.subscribe = subscribe;
exports.subscribeWithSelector = subscribeWithSelector;
exports.dispatch = dispatch;
/*  Not a pure module */
