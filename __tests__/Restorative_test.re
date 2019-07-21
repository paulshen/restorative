open Jest;
open Restorative;

type state = {
  count: int,
  static: string,
};
type action =
  | Increment
  | Decrement;

let reducer = (state, action) => {
  switch (action) {
  | Increment => {...state, count: state.count + 1}
  | Decrement => {...state, count: state.count - 1}
  };
};

describe("Restorative", () => {
  open Expect;
  open! Expect.Operators;

  test("getState returns initial state", () => {
    let initialState = {count: 0, static: "foo"};
    let {getState} = createStore(initialState, reducer);
    expect(getState()) |> toBe(initialState);
  });

  test("it updates state", () => {
    let {getState, dispatch} =
      createStore({count: 0, static: "foo"}, reducer);
    dispatch(Increment);
    expect(getState().count) |> toBe(1);
  });

  describe("subscriptions", () => {
    test("it calls subscriptions", () => {
      let mock1 = JestJs.fn(ignore);
      let subscription1 = MockJs.fn(mock1);
      let mock2 = JestJs.fn(ignore);
      let subscription2 = MockJs.fn(mock2);

      let {getState, subscribe, dispatch} =
        createStore({count: 0, static: "foo"}, reducer);
      subscribe(subscription1, ()) |> ignore;
      subscribe(subscription2, ()) |> ignore;
      dispatch(Increment);
      let state = getState();
      expect((MockJs.calls(mock1), MockJs.calls(mock2)))
      |> toEqual(([|state|], [|state|]));
    });

    test("subscriptions can unsubscribe", () => {
      let mock1 = JestJs.fn(ignore);
      let subscription1 = MockJs.fn(mock1);
      let mock2 = JestJs.fn(ignore);
      let subscription2 = MockJs.fn(mock2);

      let {subscribe, dispatch} =
        createStore({count: 0, static: "foo"}, reducer);
      let unsubscribe1 = subscribe(subscription1, ());
      let unsubscribe2 = subscribe(subscription2, ());
      dispatch(Increment);
      unsubscribe1();
      dispatch(Increment);
      unsubscribe2();
      dispatch(Increment);
      expect((
        Array.length(MockJs.calls(mock1)),
        Array.length(MockJs.calls(mock2)),
      ))
      |> toEqual((1, 2));
    });
  });
});