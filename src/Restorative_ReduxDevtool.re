module DevtoolInstance = {
  type t;

  [@bs.deriving abstract]
  type messagePayload = {
    [@bs.as "type"]
    type_: string,
  };
  let messagePayloadTypeGet = type_Get;

  [@bs.deriving abstract]
  type message = {
    [@bs.as "type"]
    type_: string,
    [@bs.optional]
    payload: messagePayload,
    state: option(string),
  };

  [@bs.send] external subscribe: (t, message => unit) => unit = "subscribe";
  [@bs.send] external init: (t, ~initialState: 'a) => unit = "init";
  [@bs.send] external send: (t, ~action: 'a, ~state: 'b) => unit = "send";
};

module ReduxExtension = {
  type t;
  // https://github.com/zalmoxisus/redux-devtools-extension/blob/c220fd18d6fb4b0790a501ec4df0de52b5bee460/docs/API/Arguments.md
  type connectOptions = {name: string};

  let get: unit => option(t) = [%bs.raw
    {|
function () {
  var extension;
  try {
    extension = window.__REDUX_DEVTOOLS_EXTENSION__ || window.top.__REDUX_DEVTOOLS_EXTENSION__;
  } catch {};
  return extension;
}
|}
  ];

  [@bs.send]
  external connect: (t, connectOptions) => DevtoolInstance.t = "connect";
};

external jsonToAny: Js.Json.t => 'a = "%identity";
external anyToJson: 'a => Js.Json.t = "%identity";
let defaultActionToName = _ => "UnknownAction";

[@bs.scope ("process", "env")] [@bs.val]
external nodeEnv: option(string) = "NODE_ENV";

let hasWarnedAboutMissingExtension = ref(false);

let createStoreWithReduxDevtoolSupport =
    (
      initialState: 'state,
      reducer: ('state, 'action) => 'state,
      ~storeName: string,
      ~actionToName: 'action => string=defaultActionToName,
      ~stateJsonEncoder: 'state => Js.Json.t=anyToJson,
      ~stateJsonDecoder: Js.Json.t => 'state=jsonToAny,
      (),
    ) => {
  let api = Restorative_Core.createStore(initialState, reducer);

  let dispatch =
    switch (ReduxExtension.get()) {
    | Some(extension) =>
      open DevtoolInstance;
      let instance = extension->ReduxExtension.connect({name: storeName});
      instance->subscribe(message =>
        switch (type_Get(message), payloadGet(message)) {
        | ("DISPATCH", Some(payload)) =>
          switch (messagePayloadTypeGet(payload), stateGet(message)) {
          | ("JUMP_TO_ACTION", Some(state))
          | ("JUMP_TO_STATE", Some(state)) =>
            api.replaceState(state->Js.Json.parseExn->stateJsonDecoder)
          | _ => ()
          }
        | _ => ()
        }
      );
      instance->init(~initialState);
      (
        action => {
          api.dispatch(action);
          instance->send(
            ~action={
              Js.Json.object_(
                Js.Dict.fromArray([|
                  (
                    "type",
                    Js.Json.string(storeName ++ ":" ++ actionToName(action)),
                  ),
                  ("payload", anyToJson(action)),
                |]),
              );
            },
            ~state=api.getState()->stateJsonEncoder,
          );
        }
      );
    | None =>
      if (! hasWarnedAboutMissingExtension^ && nodeEnv === Some("development")) {
        Js.log(
          "[Restorative] You do not have Redux devtools installed/enabled.",
        );
        hasWarnedAboutMissingExtension := true;
      };
      api.dispatch;
    };

  {...api, dispatch};
};