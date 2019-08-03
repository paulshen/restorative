open Jest;
open Restorative;
open ReactTestingLibrary;

[@bs.val] [@bs.module "react-dom/test-utils"]
external act: (unit => unit) => unit = "act";

type state = {
  count: int,
  static: string,
  name: string,
};
type action =
  | Increment
  | Decrement
  | ChangeName(string);

let initialState = {count: 0, static: "foo", name: "Alice"};
let reducer = (state, action) => {
  switch (action) {
  | Increment => {...state, count: state.count + 1}
  | Decrement => {...state, count: state.count - 1}
  | ChangeName(name) => {...state, name}
  };
};

module TestComponent = {
  [@react.component]
  let make = (~useStore, ~render, ~shouldDispatch=false, ()) => {
    let (state, dispatch) = useStore();
    React.useEffect0(() => {
      if (shouldDispatch) {
        dispatch(Increment);
      };
      None;
    });
    render(state);
    React.null;
  };
};

describe("Restorative", () => {
  open Expect;

  test("getState returns initial state", () => {
    let {getState} = createStore(initialState, reducer);
    expect(getState()) |> toBe(initialState);
  });

  test("it updates state", () => {
    let {getState, dispatch} = createStore(initialState, reducer);
    dispatch(Increment);
    expect(getState().count) |> toBe(1);
  });

  describe("Subscriptions", () => {
    test("it calls subscriptions", () => {
      let subscription1 = JestJs.fn(ignore);
      let subscription2 = JestJs.fn(ignore);

      let {getState, subscribe, dispatch} =
        createStore(initialState, reducer);
      subscribe(MockJs.fn(subscription1), ()) |> ignore;
      subscribe(MockJs.fn(subscription2), ()) |> ignore;
      dispatch(Increment);
      let state = getState();
      expect((MockJs.calls(subscription1), MockJs.calls(subscription2)))
      |> toEqual(([|state|], [|state|]));
    });

    test("subscriptions can unsubscribe", () => {
      let subscription1 = JestJs.fn(ignore);
      let subscription2 = JestJs.fn(ignore);

      let {subscribe, dispatch} = createStore(initialState, reducer);
      let unsubscribe1 = subscribe(MockJs.fn(subscription1), ());
      let unsubscribe2 = subscribe(MockJs.fn(subscription2), ());
      dispatch(Increment);
      unsubscribe1();
      dispatch(Increment);
      unsubscribe2();
      dispatch(Increment);
      expect((
        Array.length(MockJs.calls(subscription1)),
        Array.length(MockJs.calls(subscription2)),
      ))
      |> toEqual((1, 2));
    });

    test("it respects custom areEqual", () => {
      let subscription1 = JestJs.fn(ignore);
      let subscription2 = JestJs.fn(ignore);

      let {subscribe, dispatch} = createStore(initialState, reducer);
      subscribe(
        MockJs.fn(subscription1),
        ~areEqual=(a, b) => a.count == b.count,
        (),
      )
      |> ignore;
      subscribe(
        MockJs.fn(subscription2),
        ~areEqual=(a, b) => a.name == b.name,
        (),
      )
      |> ignore;

      dispatch(Increment);
      let count1 = (
        Array.length(MockJs.calls(subscription1)),
        Array.length(MockJs.calls(subscription2)),
      );

      dispatch(ChangeName("Bob"));
      let count2 = (
        Array.length(MockJs.calls(subscription1)),
        Array.length(MockJs.calls(subscription2)),
      );

      expect((count1, count2)) |> toEqual(((1, 0), (1, 1)));
    });
  });

  describe("Selectors", () => {
    test("it selects state slice", () => {
      let subscription = JestJs.fn(ignore);

      let {subscribeWithSelector, dispatch} =
        createStore(initialState, reducer);
      subscribeWithSelector(
        state => state.count,
        MockJs.fn(subscription),
        (),
      )
      |> ignore;

      dispatch(Increment);
      expect(MockJs.calls(subscription)) |> toEqual([|1|]);
    });

    test("it respects slice equality", () => {
      let countSubscription = JestJs.fn(ignore);
      let staticSubscription = JestJs.fn(ignore);

      let {subscribeWithSelector, dispatch} =
        createStore(initialState, reducer);
      subscribeWithSelector(
        state => state.count,
        MockJs.fn(countSubscription),
        (),
      )
      |> ignore;
      subscribeWithSelector(
        state => state.static,
        MockJs.fn(staticSubscription),
        (),
      )
      |> ignore;

      dispatch(Increment);
      expect((
        MockJs.calls(countSubscription),
        MockJs.calls(staticSubscription),
      ))
      |> toEqual(([|1|], [||]));
    });
  });

  describe("Hooks", () => {
    test("useStore", () => {
      let {useStore, dispatch} = createStore(initialState, reducer);

      let renderMock = JestJs.fn(ignore);
      let renderFn = MockJs.fn(renderMock);
      let useStore = () => useStore();
      render(<TestComponent useStore render=renderFn />) |> ignore;

      let calls1 = MockJs.calls(renderMock);
      act(() => dispatch(Increment));
      let calls2 = MockJs.calls(renderMock);

      expect((Array.length(calls1), calls2))
      |> toEqual((1, [|initialState, {...initialState, count: 1}|]));
    });

    test("useStore can dispatch", () => {
      let subscription = JestJs.fn(ignore);
      let {useStore, getState, subscribe} =
        createStore(initialState, reducer);
      subscribe(MockJs.fn(subscription), ()) |> ignore;

      let renderMock = JestJs.fn(ignore);
      let renderFn = MockJs.fn(renderMock);
      let useStore = () => useStore();
      render(<TestComponent useStore render=renderFn shouldDispatch=true />)
      |> ignore;

      expect((
        Array.length(MockJs.calls(renderMock)),
        getState(),
        Array.length(MockJs.calls(subscription)),
      ))
      |> toEqual((2, {...initialState, count: 1}, 1));
    });

    test("useStoreWithSelector", () => {
      let {useStoreWithSelector, dispatch} =
        createStore(initialState, reducer);

      let renderMock = JestJs.fn(ignore);
      let renderFn = MockJs.fn(renderMock);
      let useStoreWithSelector = () =>
        useStoreWithSelector(state => state.count, ());
      render(<TestComponent useStore=useStoreWithSelector render=renderFn />)
      |> ignore;

      let calls1 = MockJs.calls(renderMock);
      act(() => dispatch(Increment));
      let calls2 = MockJs.calls(renderMock);
      act(() => dispatch(ChangeName("Bob")));
      let calls3 = MockJs.calls(renderMock);

      expect((calls1, calls2, calls3))
      |> toEqual(([|0|], [|0, 1|], [|0, 1|]));
    });
  });
});