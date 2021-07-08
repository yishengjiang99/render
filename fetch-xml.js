export function fetchxml(url, cb) {
    const xhr = new XMLHttpRequest();
    xhr.open("GET", url);
    xhr.responseType = "document";
    xhr.send();
    xhr.onload = function () {
        cb(null, xhr.responseXML.documentElement);
    };
    xhr.onerror = (ev) => cb(new Error(ev.type), null);
}
window.prests = [];
let docf=[];
let drums=[];
fetchxml("sm.xml", (err, doc) => {
    if (!err && doc) {
        docf=doc;
        doc.querySelectorAll("Preset").forEach(p => {
            prests.push(new Proxy(p, {
                get(target,key){
                    return {
                        ...p.attributes,
                        zones: Array.from(p.querySelectorAll("Zone"))
                    }
                }
            })); 
        });
        console.log(window.prests[0]);
    }
});
console.log(window.prests[0]);

function newSFZone(attrs) {
    const attributeKeys = genstr();
    const attributeValues = attrs.split(",").map((s) => parseInt(s));
    return new Proxy(attributeValues, {
        get: (target, key) => {
            const idx = attributeKeys.indexOf(key);
            return idx > -1 ? target[idx] : null;
        },
    });
}