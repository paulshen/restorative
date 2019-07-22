# Restorative

[![CircleCI](https://circleci.com/gh/paulshen/restorative/tree/master.svg?style=svg)](https://circleci.com/gh/paulshen/restorative/tree/master)

Simple ReasonML state management. Comes with React hooks.

Although you can get very far with core ReasonML and React functionality, you may find yourself wanting to manage global state with subscriptions. `Restorative` may be your solution. Features a reducer store with performant subscriptions and selectors.

Similar projects include [Redux](https://redux.js.org/), [Reductive](https://github.com/reasonml-community/reductive), and [zustand](https://github.com/react-spring/zustand).

```re
type state = int;
type action =
  | Increment
  | Decrement;

let api =
  Restorative.createStore(0, (state, action) =>
    switch (action) {
    | Increment => state + 1
    | Decrement => state - 1
    }
  );
```

## Basic Usage

```re
let {dispatch, subscribe, getState} = api;
let unsubscribe = subscribe(state => Js.log(state));
dispatch(Increment); // calls subscriptions
getState(); // 1
unsubscribe();
```

## React hook

```re
let {useStore} = api;

[@react.component]
let make = () => {
  let (state, dispatch) = useStore();
  <button onClick={_ => dispatch(Increment)}>
    {React.string(string_of_int(state))}
  </button>;
};
```

## Selector

```re
type state = {
  a: int,
  b: int,
};
type action =
  | IncrementA
  | IncrementB;

let {subscribeWithSelector, dispatch} =
  createStore({a: 0, b: 0}, (state, action) =>
    switch (action) {
    | IncrementA => {...state, a: state.a + 1}
    | IncrementB => {...state, b: state.b + 1}
    }
  );

subscribeWithSelector(a => Js.log(a), ~selector=state => state.a, ());
dispatch(IncrementA); // calls listener
dispatch(IncrementB); // does not call listener
```

### useStoreWithSelector

```re
[@react.component]
let make = () => {
  let (a, dispatch) = useStoreWithSelector(state => state.a, ());
  // Only updates when a changes
  ...
};
```

## Equality

Restorative will not call listeners if the selected state has not "changed" (entire state if no selector). By default, Restorative uses `Object.is` for equality checking. All `subscribe` and `useStore` functions take an optional `~equalityFn: ('state, 'state) => bool`.

```re
useStoreWithSelector(
  state => [|state.a, state.b|],
  ~equalityFn=(a, b) => a == b,
  ()
);
```

### Comparison with JavaScript libraries

We get all the benefits of Reason's great type system. Instead of plain JavaScript objects, we use variants to model actions. All operations have sound types and allow work (e.g. action creators) to happen at compile time.

### Comparison with React Context

Restorative maintains lists of subscriptions for each store. When a React context value changes, it iterates through all children Fiber nodes to find context consumers. React context is not well suited for fast-changing data. Subscriptions, on the other hand, allow for more precise operations at the cost of more complexity (maintaining list of subscribers).

### Comparison with Redux

Redux applications typically use a single global store and dispatcher. With Restorative, you can create multiple stores, each with its own dispatcher. This allows better separation of state logic.
