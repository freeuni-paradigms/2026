(function () {
  "use strict";

  const {
    DEFAULT_SOURCE,
    parseMovies,
    buildDatabase,
    formatByte,
    formatAddress,
    bytesForSegment,
  } = window.ImdbByteMapCore;

  const root = document.querySelector("#byte-map");
  const find = (selector) => root.querySelector(selector);
  const ui = {
    editor: find("textarea"),
    status: find(".status"),
    reset: find(".reset"),
    radixControl: find(".radix-control"),
    endianControl: find(".endian-control"),
    actorMap: find(".actor-map"),
    movieMap: find(".movie-map"),
    actorSize: find(".actor-size"),
    movieSize: find(".movie-size"),
    actorInspector: find(".actor-inspector"),
    movieInspector: find(".movie-inspector"),
  };

  let radix = "decimal";
  let endian = "big";
  let model;
  let database;
  let selectedSegment = null;
  let hoveredSegment = null;
  let focusedSegment = null;
  let renderTimer;

  const segmentById = new Map();
  const explanationById = new Map();
  const recordTargets = new Map();

  function element(tag, className, text) {
    const node = document.createElement(tag);
    if (className) node.className = className;
    if (text !== undefined) node.textContent = text;
    return node;
  }

  function clearInteraction() {
    selectedSegment = null;
    hoveredSegment = null;
    focusedSegment = null;
  }

  function refreshControls() {
    ui.radixControl.querySelectorAll("button").forEach((button) => {
      button.setAttribute("aria-pressed", String(button.dataset.value === radix));
    });
    ui.endianControl.querySelectorAll("button").forEach((button) => {
      button.setAttribute("aria-pressed", String(button.dataset.value === endian));
    });
  }

  function columnsPerRow() {
    return radix === "binary" ? 4 : 8;
  }

  function cellWidth() {
    if (radix === "binary") return "72px";
    if (radix === "decimal") return "38px";
    return "31px";
  }

  function addressWidth() {
    return radix === "binary" ? "136px" : "48px";
  }

  function formatColumn(column) {
    if (radix === "binary") return column.toString(2).padStart(3, "0");
    return column.toString(radix === "decimal" ? 10 : 16);
  }

  function createExplanation(file, segment) {
    const bytes = bytesForSegment(file, segment);
    const end = segment.start + segment.length - 1;
    const startAddress = formatAddress(segment.start, radix);
    const endAddress = formatAddress(end, radix);
    const lines = [
      segment.label,
      `range: ${startAddress}${end === segment.start ? "" : `–${endAddress}`} (${segment.length} B)`,
      `binary:  ${[...bytes].map((byte) => formatByte(byte, "binary")).join(" ")}`,
      `hex:     ${[...bytes].map((byte) => formatByte(byte, "hex")).join(" ")}`,
      `decimal: ${[...bytes].map((byte) => formatByte(byte, "decimal")).join(" ")}`,
    ];
    if (segment.decoded !== undefined) lines.push(`decoded: ${segment.decoded}`);
    lines.push(segment.description);
    return lines.join("\n");
  }

  function rebuildLookups() {
    segmentById.clear();
    explanationById.clear();
    recordTargets.clear();
    for (const file of [database.actordata, database.moviedata]) {
      for (const segment of file.segments) {
        segmentById.set(segment.id, segment);
        explanationById.set(segment.id, createExplanation(file, segment));
        if (segment.recordKey) {
          const current = recordTargets.get(segment.recordKey);
          if (!current || segment.start < current.offset) {
            recordTargets.set(segment.recordKey, {fileName: file.name, offset: segment.start});
          }
        }
      }
    }
  }

  function renderFile(file, container) {
    container.replaceChildren();
    container.className = `byte-map ${file.name === "actordata" ? "actor-map" : "movie-map"} radix-${radix}`;
    const columns = columnsPerRow();
    container.style.setProperty("--columns", columns);
    container.style.setProperty("--cell-width", cellWidth());
    container.style.setProperty("--address-width", addressWidth());

    const heading = element("div", "byte-row");
    heading.append(element("span", "column-heading", "offset"));
    for (let column = 0; column < columns; ++column) {
      heading.append(element("span", "column-heading", `+${formatColumn(column)}`));
    }
    container.append(heading);

    for (let rowStart = 0; rowStart < file.bytes.length; rowStart += columns) {
      const row = element("div", "byte-row");
      row.append(element("span", "address", formatAddress(rowStart, radix)));
      for (let column = 0; column < columns; ++column) {
        const offset = rowStart + column;
        if (offset >= file.bytes.length) {
          row.append(element("span", "filler"));
          continue;
        }
        const segment = file.segmentAt[offset];
        const button = element("button", `byte type-${segment.type}`, formatByte(file.bytes[offset], radix));
        button.type = "button";
        button.dataset.offset = offset;
        button.dataset.fileName = file.name;
        button.dataset.segmentId = segment.id;
        if (segment.recordKey) button.dataset.recordKey = segment.recordKey;
        const explanation = explanationById.get(segment.id);
        button.setAttribute("aria-label", `${formatAddress(offset, radix)}: ${explanation.replace(/\n/g, ", ")}`);
        row.append(button);
      }
      container.append(row);
    }
  }

  function activePreview() {
    return hoveredSegment || focusedSegment;
  }

  function applyHighlights() {
    const preview = activePreview();
    const previewTarget = preview && preview.targetRecord ? recordTargets.get(preview.targetRecord) : null;
    root.querySelectorAll(".byte").forEach((button) => {
      button.classList.toggle("selected",
          Boolean(selectedSegment && button.dataset.segmentId === selectedSegment.id));
      button.classList.toggle("target",
          Boolean(selectedSegment && selectedSegment.targetRecord &&
                  button.dataset.recordKey === selectedSegment.targetRecord));
      button.classList.toggle("group-hover",
          Boolean(preview && button.dataset.segmentId === preview.id));
      button.classList.toggle("offset-target",
          Boolean(previewTarget && button.dataset.fileName === previewTarget.fileName &&
                  Number(button.dataset.offset) === previewTarget.offset));
    });
  }

  function renderInspector(fileName, segment, target) {
    target.replaceChildren();
    const heading = element("h3");
    const text = element("pre");
    if (!segment || segment.fileName !== fileName) {
      heading.textContent = "ბაიტის განმარტება";
      text.textContent = "მიიტანეთ კურსორი ბაიტზე ან დააჭირეთ მას.\nარჩეული offset შესაბამის ჩანაწერს მეორე პანელშიც მონიშნავს.";
    } else {
      heading.textContent = segment.label;
      text.textContent = explanationById.get(segment.id).split("\n").slice(1).join("\n");
    }
    target.append(heading, text);
  }

  function renderInspectors() {
    const active = activePreview() || selectedSegment;
    renderInspector("actordata", active, ui.actorInspector);
    renderInspector("moviedata", active, ui.movieInspector);
  }

  function renderDatabase() {
    rebuildLookups();
    ui.actorSize.textContent = `${database.actordata.bytes.length} ბაიტი`;
    ui.movieSize.textContent = `${database.moviedata.bytes.length} ბაიტი`;
    renderFile(database.actordata, ui.actorMap);
    renderFile(database.moviedata, ui.movieMap);
    applyHighlights();
    renderInspectors();
  }

  function updateFromSource() {
    clearTimeout(renderTimer);
    try {
      const nextModel = parseMovies(ui.editor.value);
      const nextDatabase = buildDatabase(nextModel, endian);
      model = nextModel;
      database = nextDatabase;
      clearInteraction();
      ui.status.textContent = `${database.model.movies.length} ფილმი, ${database.model.actors.length} მსახიობი`;
      ui.status.classList.remove("error");
      ui.editor.removeAttribute("aria-invalid");
      renderDatabase();
    } catch (error) {
      ui.status.textContent = error.message;
      ui.status.classList.add("error");
      ui.editor.setAttribute("aria-invalid", "true");
    }
  }

  function byteFromEvent(event) {
    return event.target instanceof Element ? event.target.closest(".byte") : null;
  }

  function segmentFromByte(button) {
    return button ? segmentById.get(button.dataset.segmentId) : null;
  }

  function updatePreview() {
    applyHighlights();
    renderInspectors();
  }

  function scrollToRecord(recordKey) {
    const target = recordTargets.get(recordKey);
    if (!target) return;
    const container = target.fileName === "actordata" ? ui.actorMap : ui.movieMap;
    const button = container.querySelector(`.byte[data-offset="${target.offset}"]`);
    const reducedMotion = matchMedia("(prefers-reduced-motion: reduce)").matches;
    if (button) button.scrollIntoView({block: "nearest", inline: "nearest", behavior: reducedMotion ? "auto" : "smooth"});
  }

  root.addEventListener("pointerover", (event) => {
    const button = byteFromEvent(event);
    if (!button) return;
    const previous = event.relatedTarget instanceof Element ? event.relatedTarget.closest(".byte") : null;
    if (previous && previous.dataset.segmentId === button.dataset.segmentId) return;
    hoveredSegment = segmentFromByte(button);
    updatePreview();
  });

  root.addEventListener("pointerout", (event) => {
    const button = byteFromEvent(event);
    if (!button) return;
    const next = event.relatedTarget instanceof Element ? event.relatedTarget.closest(".byte") : null;
    if (next && next.dataset.segmentId === button.dataset.segmentId) return;
    if (hoveredSegment && hoveredSegment.id === button.dataset.segmentId) {
      hoveredSegment = null;
      updatePreview();
    }
  });

  root.addEventListener("focusin", (event) => {
    const button = byteFromEvent(event);
    if (!button) return;
    focusedSegment = segmentFromByte(button);
    updatePreview();
  });

  root.addEventListener("focusout", (event) => {
    const button = byteFromEvent(event);
    if (!button) return;
    const next = event.relatedTarget instanceof Element ? event.relatedTarget.closest(".byte") : null;
    if (next && next.dataset.segmentId === button.dataset.segmentId) return;
    focusedSegment = null;
    updatePreview();
  });

  root.addEventListener("click", (event) => {
    const button = byteFromEvent(event);
    if (!button) return;
    selectedSegment = segmentFromByte(button);
    updatePreview();
    if (selectedSegment.targetRecord) scrollToRecord(selectedSegment.targetRecord);
  });

  ui.reset.addEventListener("click", () => {
    ui.editor.value = DEFAULT_SOURCE;
    updateFromSource();
  });

  ui.editor.addEventListener("input", () => {
    clearTimeout(renderTimer);
    renderTimer = setTimeout(updateFromSource, 180);
  });

  ui.radixControl.addEventListener("click", (event) => {
    const button = event.target.closest("button[data-value]");
    if (!button || button.dataset.value === radix) return;
    radix = button.dataset.value;
    clearInteraction();
    refreshControls();
    renderDatabase();
  });

  ui.endianControl.addEventListener("click", (event) => {
    const button = event.target.closest("button[data-value]");
    if (!button || button.dataset.value === endian) return;
    endian = button.dataset.value;
    clearInteraction();
    database = buildDatabase(model, endian);
    refreshControls();
    renderDatabase();
  });

  ui.editor.value = DEFAULT_SOURCE;
  refreshControls();
  updateFromSource();
})();
