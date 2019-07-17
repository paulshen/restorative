type reducer('state, 'action) = ('state, 'action) => 'state;

type api('state, 'action) = {
  getState: unit => 'state,
  subscribe:
    ('state => unit, ~equalityFn: ('state, 'state) => bool=?, unit) =>
    (. unit) => unit,
  subscribeWithSelector:
    'slice.
    (
      'slice => unit,
      ~selector: 'state => 'slice,
      ~equalityFn: ('slice, 'slice) => bool=?,
      unit
    ) =>
    (. unit) => unit,

  dispatch: 'action => unit,
  useStore:
    (~equalityFn: ('state, 'state) => bool=?, unit) =>
    ('state, 'action => unit),
  useStoreWithSelector:
    'slice.
    ('state => 'slice, ~equalityFn: ('slice, 'slice) => bool=?, unit) =>
    ('slice, 'action => unit),

};

[@bs.val] external objIs: ('a, 'b) => bool = "Object.is";

let createStore =
    (initialState: 'state, reducer: reducer('state, 'action))
    : api('state, 'action) => {
  let state = ref(initialState);
  let listeners = ref([||]);

  let getState = () => state^;

  let dispatch = action => {
    state := reducer(state^, action);
    listeners^ |> Js.Array.forEach(listener => listener());
  };

  let subscribe = (listener, ~equalityFn=objIs, ()) => {
    let currentState = ref(state^);
    let listenerFn = () => {
      let state = state^;
      if (!equalityFn(state, currentState^)) {
        listener(state);
        currentState := state;
      };
    };
    listeners := listeners^ |> Js.Array.concat([|listenerFn|]);
    (.) => {
      listeners := listeners^ |> Js.Array.filter(l => l !== listenerFn);
    };
  };

  let subscribeWithSelector = (listener, ~selector, ~equalityFn=objIs, ()) => {
    let currentSlice = ref(selector(state^));
    let listenerFn = () => {
      let slice = selector(state^);
      if (!equalityFn(slice, currentSlice^)) {
        listener(slice);
        currentSlice := slice;
      };
    };
    listeners := listeners^ |> Js.Array.concat([|listenerFn|]);
    (.) => {
      listeners := listeners^ |> Js.Array.filter(l => l !== listenerFn);
    };
  };

  let useStore = (~equalityFn=?, ()) => {
    let (_, forceUpdate) = React.useState(() => 1);
    React.useLayoutEffect0(() => {
      let unsubscribe =
        subscribe(_ => forceUpdate(x => x + 1), ~equalityFn?, ());
      Some(() => unsubscribe(.));
    });
    (state^, dispatch);
  };

  let useStoreWithSelector = (selector, ~equalityFn=?, ()) => {
    let (slice, setSlice) = React.useState(() => selector(state^));
    React.useLayoutEffect0(() => {
      let unsubscribe =
        subscribeWithSelector(
          slice => setSlice(_ => slice),
          ~selector,
          ~equalityFn?,
          (),
        );
      Some(() => unsubscribe(.));
    });
    (slice, dispatch);
  };

  {
    getState,
    subscribe,
    subscribeWithSelector,
    dispatch,
    useStore,
    useStoreWithSelector,
  };
};