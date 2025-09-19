#!/usr/bin/osascript -l JavaScript
ObjC.import('Foundation');

function getArgs() {
  const processInfo = $.NSProcessInfo.processInfo;
  const objcArgs = processInfo.arguments;
  const count = objcArgs.count;
  const raw = [];
  for (let i = 0; i < count; i++) {
    raw.push(ObjC.unwrap(objcArgs.objectAtIndex(i)));
  }

  const filtered = [];
  for (let i = 1; i < raw.length; i++) { // drop process name at index 0
    const value = raw[i];
    if (value === '-l') { // skip interpreter language flag and its value
      i++;
      continue;
    }
    if (value === 'JavaScript') {
      continue;
    }
    filtered.push(value);
  }

  if (filtered.length > 0) {
    filtered.shift(); // drop script path
  }

  return filtered;
}

function waitFor(fn, attempts, delaySeconds) {
  for (let i = 0; i < attempts; i++) {
    const value = fn();
    if (value)
      return value;
    delay(delaySeconds);
  }
  return null;
}

function runScript(safari, doc, source) {
  try {
    return safari.doJavaScript(source, { in: doc });
  } catch (error) {
    const message = String(error);
    if (message.indexOf('Allow JavaScript from Apple Events') !== -1) {
      throw new Error('Safari blocked automation. Enable "Allow JavaScript from Apple Events" in Safari ▸ Settings ▸ Advanced.');
    }
    throw error;
  }
}

(function main() {
  const args = getArgs();
  const targetUrl = args[0] || 'http://localhost:8765/index.html';

  const safari = Application('Safari');
  safari.includeStandardAdditions = true;
  safari.activate();
  safari.openLocation(targetUrl);

  function findDoc() {
    const docs = safari.documents;
    for (let i = 0; i < docs.length; i++) {
      const doc = docs[i];
      try {
        const url = doc.url();
        if (url && url.indexOf(targetUrl.split('?')[0]) === 0)
          return doc;
      } catch (e) {
        // document may not be ready; ignore and retry
      }
    }
    return null;
  }

  const doc = waitFor(findDoc, 40, 0.5);
  if (!doc)
    throw new Error('Unable to find Safari tab for ' + targetUrl);

  const statusText = waitFor(() => runScript(safari, doc,
    "(function () { var el = document.getElementById('status'); return el ? el.textContent : ''; })();"),
    80, 0.5);

  if (!statusText || statusText.indexOf('libansilove wasm version:') !== 0)
    throw new Error('Unexpected status text: ' + statusText);

  const previewSrc = waitFor(() => runScript(safari, doc,
    "(function () { var el = document.getElementById('preview'); return el && el.src ? el.src : ''; })();"),
    80, 0.5);

  if (!previewSrc || previewSrc.indexOf('blob:') !== 0)
    throw new Error('Preview image not initialised: ' + previewSrc);

  statusText + '\n' + previewSrc;
})();
