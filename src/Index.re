module Example = {
  [@react.component]
  let make = () => {
    <div> {React.string("Hello")} </div>;
  };
};

ReactDOMRe.renderToElementWithId(<Example />, "root");