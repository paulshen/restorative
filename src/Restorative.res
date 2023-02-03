type reducer<'state, 'action> = ('state, 'action) => 'state

type api<'state, 'action> = {
  getState: unit => 'state,
  subscribe: ('state => unit, ~areEqual: ('state, 'state) => bool=?, unit, unit) => unit,
  subscribeWithSelector: 'slice. (
    'state => 'slice,
    'slice => unit,
    ~areEqual: ('slice, 'slice) => bool=?,
    unit,
    unit,
  ) => unit,
  dispatch: 'action => unit,
  useStore: (~areEqual: ('state, 'state) => bool=?, unit) => 'state,
  useStoreWithSelector: 'slice. (
    'state => 'slice,
    ~areEqual: ('slice, 'slice) => bool=?,
    unit,
  ) => 'slice,
}

@val external objIs: ('a, 'b) => bool = "Object.is"

let createStore = (initialState: 'state, reducer: reducer<'state, 'action>): api<
  'state,
  'action,
> => {
  let state = ref(initialState)
  let listeners = ref([])

  let getState = () => state.contents

  let dispatch = action => {
    state := reducer(state.contents, action)
    listeners.contents->Js.Array2.forEach(listener => listener())
  }

  let subscribe = (listener, ~areEqual=objIs, ()) => {
    let currentState = ref(state.contents)
    let listenerFn = () => {
      let state = state.contents
      if !areEqual(state, currentState.contents) {
        listener(state)
        currentState := state
      }
    }
    listeners := listeners.contents->Js.Array2.concat([listenerFn])
    () => listeners := listeners.contents->Js.Array2.filter(l => l !== listenerFn)
  }

  let subscribeWithSelector = (selector, listener, ~areEqual=objIs, ()) => {
    let currentSlice = ref(selector(state.contents))
    let listenerFn = () => {
      let slice = selector(state.contents)
      if !areEqual(slice, currentSlice.contents) {
        listener(slice)
        currentSlice := slice
      }
    }
    listeners := listeners.contents |> Js.Array.concat([listenerFn])
    () => listeners := listeners.contents |> Js.Array.filter(l => l !== listenerFn)
  }

  let useStore = (~areEqual=?, ()) => {
    let (_, forceUpdate) = React.useState(() => 1)
    React.useLayoutEffect0(() => {
      let unsubscribe = subscribe(_ => forceUpdate(x => x + 1), ~areEqual?, ())
      // The state may have changed between render and this effect.
      forceUpdate(x => x + 1)
      Some(() => unsubscribe())
    })
    state.contents
  }

  let useStoreWithSelector = (selector, ~areEqual=?, ()) => {
    let sliceRef = React.useRef(None)
    let selectorRef = React.useRef(selector)

    let prevSlice = sliceRef.current
    let prevSelector = selectorRef.current
    let slice = if selector === prevSelector {
      switch prevSlice {
      | Some(slice) => slice
      | None => selector(state.contents)
      }
    } else {
      selector(state.contents)
    }
    React.useLayoutEffect1(() => {
      sliceRef.current = Some(slice)

      None
    }, [slice])
    React.useLayoutEffect1(() => {
      selectorRef.current = selector
      None
    }, [selector])
    let (_, forceUpdate) = React.useState(() => 1)
    React.useLayoutEffect0(() => {
      let unsubscribe = subscribeWithSelector(
        state => selectorRef.current(state),
        slice => {
          sliceRef.current = Some(slice)

          forceUpdate(x => x + 1)
        },
        ~areEqual?,
        (),
      )
      sliceRef.current = Some(selector(state.contents))
      // The state may have changed between render and this effect.
      forceUpdate(x => x + 1)
      Some(() => unsubscribe())
    })
    slice
  }

  {
    getState: getState,
    subscribe: subscribe,
    subscribeWithSelector: subscribeWithSelector,
    dispatch: dispatch,
    useStore: useStore,
    useStoreWithSelector: useStoreWithSelector,
  }
}
