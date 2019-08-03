open Restorative;

type state = {count: int};
type action =
  | Increment;

let reducer = (state, action) => {
  switch (action) {
  | Increment => {count: state.count + 1}
  };
};
let {
  getState,
  subscribe,
  subscribeWithSelector,
  dispatch,
  useStore,
  useStoreWithSelector,
} =
  createStore({count: 1}, reducer);

[@bs.val] external jsonStringify: 'a => string = "JSON.stringify";

module Example = {
  type derivedState = {countDividedBy5: int};

  [@react.component]
  let make = () => {
    React.useEffect0(() => {
      Js.log(getState());
      subscribe(state => Js.log(state), ()) |> ignore;
      subscribeWithSelector(state => state.count, count => Js.log(count), ())
      |> ignore;
      subscribeWithSelector(
        state => string_of_int(state.count + 1),
        count => Js.log(count),
        (),
      )
      |> ignore;
      subscribeWithSelector(
        state => {countDividedBy5: state.count / 5},
        derivedState => Js.log2("Derived", derivedState),
        ~equalityFn=(a, b) => a == b,
        (),
      )
      |> ignore;
      dispatch(Increment);

      None;
    });

    let (state, _dispatch) = useStore();
    let (jsonState, dispatch) = useStoreWithSelector(jsonStringify, ());
    let (changingSelector, _dispatch) =
      useStoreWithSelector(
        state => jsonState ++ " " ++ string_of_int(state.count),
        (),
      );

    <div>
      <div> {React.string("Count: " ++ string_of_int(state.count))} </div>
      <div> {React.string("JSON: " ++ jsonState)} </div>
      <div> {React.string("Changing selector: " ++ changingSelector)} </div>
      <div>
        <button onClick={_ => dispatch(Increment)}>
          {React.string("Increment")}
        </button>
      </div>
    </div>;
  };
};

ReactDOMRe.renderToElementWithId(<Example />, "root");