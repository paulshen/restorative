type reducer('state, 'action) = ('state, 'action) => 'state;

type api('state, 'action) = {
  getState: unit => 'state,
  subscribe:
    ('state => unit, ~areEqual: ('state, 'state) => bool=?, unit, unit) => unit,
  subscribeWithSelector:
    'slice.
    (
      'state => 'slice,
      'slice => unit,
      ~areEqual: ('slice, 'slice) => bool=?,
      unit,
      unit
    ) =>
    unit,

  dispatch: 'action => unit,
  useStore: (~areEqual: ('state, 'state) => bool=?, unit) => 'state,
  useStoreWithSelector:
    'slice.
    ('state => 'slice, ~areEqual: ('slice, 'slice) => bool=?, unit) => 'slice,

};

let createStore: ('state, reducer('state, 'action)) => api('state, 'action);