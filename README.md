# Restorative

Simple ReasonML state management.

```re
type state = int;
type action =
  | Increment
  | Decrement;

let api =
  Restorative.createStore(0, (state, action) =>
    switch (action) {
    | Increment => state + 1
    | Decrement => state + 1
    }
  );
```

## Basic Usage

```re
let {dispatch, subscribe, getState} = api;
getState(); // 0
let unsubscribe = subscribe(state => Js.log(state));
dispatch(Increment);
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
