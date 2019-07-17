module Example = {
  [@react.component]
  let make = () => {
    <div> {React.string("Hello")} </div>;
  };
};

ReactDOMRe.renderToElementWithId(<Example />, "root");

type state = {count: int};
type action =
  | Increment;

let myReducer = (state, action) => {
  switch (action) {
  | Increment => {count: state.count + 1}
  };
};
let {getState, subscribe, subscribeWithSelector, dispatch}:
  Restorative.api(state, action) =
  Restorative.create({count: 1}, myReducer);
Js.log(getState());
subscribe(state => Js.log(state), ()) |> ignore;
subscribeWithSelector(
  count => Js.log(count),
  ~selector=state => state.count,
  (),
)
|> ignore;
subscribeWithSelector(
  count => Js.log(count),
  ~selector=state => string_of_int(state.count + 1),
  (),
)
|> ignore;
dispatch(Increment);