let createStoreWithReduxDevtoolSupport:
  (
    'state,
    ('state, 'action) => 'state,
    ~storeName: string,
    ~actionToName: 'action => string=?,
    ~stateJsonEncoder: 'state => Js.Json.t=?,
    ~stateJsonDecoder: Js.Json.t => 'state=?,
    unit
  ) =>
  Restorative_Core.api('state, 'action);