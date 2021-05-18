let worker;
const timespan = document.querySelector("span#time");
const buttons = Array.from(document.querySelectorAll("button"));
function initworker() {
  worker = new Worker("js/tickworker.js");
  worker.onmessage = ({ data }) => {
    timespan.innerHTML = data;
  };
}
let enablebuttons = (enabledArray) => {
  for (let i = 0; i < 4; i++) {
    if (enabledArray.indexOf(i) >= 0) {
      buttons[i].removeAttribute("disabled");
    } else {
      buttons[i].setAttribute("disabled", true);
    }
  }
};
buttons.map((btn, idx) => {
  switch (idx) {
    case 0:
      btn.onclick = () => {
        enablebuttons([1, 3]);
        initworker();
      };
      break;
    case 1:
      btn.onclick = () => {
        worker.postMessage("pause");
        enablebuttons([2, 3]);
      };

      break;
    case 2:
      btn.onclick = () => {
        worker.postMessage("resume");
        enablebuttons([1, 3]);
      };

      break;
    case 3:
      btn.onclick = () => {
        worker.terminate();
        enablebuttons([0]);
        timespan.innerHTML = "0";
      };
      break;
    default:
      break;
  }
});
enablebuttons([0]);
