mocha.setup("bdd");
describe("mocha", () => {
  it("tests things", () => {
    chai.expect(true);
    const xhr = new XMLHttpRequest();
    xhr.open("GET", "./index2.xml", true);

    // If specified, responseType must be empty string or "document"
    xhr.responseType = "document";

    // Force the response to be parsed as XML
    xhr.overrideMimeType("text/xml");

    xhr.onload = function () {
      if (xhr.readyState === xhr.DONE && xhr.status === 200) {
        expect(xhr.response.document).to.exit;
        xhr.response.document.querySelector("Preset[pid=52][bank_id=54]");
      }
    };

    xhr.send();
  });
});
mocha.run();
