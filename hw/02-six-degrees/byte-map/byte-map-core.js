(function (global) {
  "use strict";

  const DEFAULT_SOURCE = `movies:
  - title: Clerks
    year: 1993
    cast:
      - Cher
      - Liberace

  - title: Moonstruck
    year: 1988
    cast:
      - Cher
      - Liberace
      - Madonna

  - title: Zoolander
    year: 1999
    cast:
      - Liberace
      - Madonna
`;

  const MAX_MOVIES = 20;
  const MAX_ACTORS = 50;
  const MAX_TEXT_BYTES = 100;
  const MAX_FILE_BYTES = 8192;

  function fail(line, message) {
    const error = new Error(line ? `ხაზი ${line}: ${message}` : message);
    error.line = line;
    throw error;
  }

  function parseScalar(text, line) {
    const value = text.trim();
    if (!value) fail(line, "მნიშვნელობა ცარიელია.");
    if (value.startsWith('"') || value.endsWith('"')) {
      if (!(value.startsWith('"') && value.endsWith('"'))) {
        fail(line, "double-quoted ტექსტს ორივე ბრჭყალი სჭირდება.");
      }
      try {
        const parsed = JSON.parse(value);
        if (typeof parsed !== "string") fail(line, "მოსალოდნელია ტექსტი.");
        return parsed;
      } catch (error) {
        fail(line, "არასწორი double-quoted ტექსტია.");
      }
    }
    if (value.startsWith("'") || value.endsWith("'")) {
      if (!(value.startsWith("'") && value.endsWith("'") && value.length >= 2)) {
        fail(line, "single-quoted ტექსტს ორივე ბრჭყალი სჭირდება.");
      }
      return value.slice(1, -1).replace(/''/g, "'");
    }
    return value;
  }

  function parseMovies(source) {
    const lines = source.replace(/\r\n?/g, "\n").split("\n");
    const movies = [];
    let current = null;
    let sawRoot = false;
    let inCast = false;

    for (let index = 0; index < lines.length; ++index) {
      const lineNumber = index + 1;
      const raw = lines[index];
      if (!raw.trim() || raw.trimStart().startsWith("#")) continue;
      if (raw.includes("\t")) fail(lineNumber, "ტაბის ნაცვლად space-ები გამოიყენეთ.");

      if (raw === "movies:") {
        if (sawRoot || movies.length) fail(lineNumber, "`movies:` მხოლოდ ერთხელ უნდა ეწეროს.");
        sawRoot = true;
        continue;
      }

      let match = raw.match(/^  - title:\s*(.+)$/);
      if (match) {
        if (!sawRoot) fail(lineNumber, "ფაილი `movies:`-ით უნდა იწყებოდეს.");
        current = {title: parseScalar(match[1], lineNumber), year: null, cast: [], line: lineNumber};
        movies.push(current);
        inCast = false;
        continue;
      }

      match = raw.match(/^    year:\s*(.+)$/);
      if (match) {
        if (!current) fail(lineNumber, "`year`-ს წინ ფილმის `title` უნდა ეწეროს.");
        if (current.year !== null) fail(lineNumber, "ამ ფილმს `year` უკვე აქვს.");
        if (!/^-?\d+$/.test(match[1].trim())) fail(lineNumber, "წელი მთელი რიცხვი უნდა იყოს.");
        current.year = Number(match[1].trim());
        inCast = false;
        continue;
      }

      if (/^    cast:\s*$/.test(raw)) {
        if (!current) fail(lineNumber, "`cast`-ს წინ ფილმის `title` უნდა ეწეროს.");
        inCast = true;
        continue;
      }

      match = raw.match(/^      -\s+(.+)$/);
      if (match) {
        if (!current || !inCast) fail(lineNumber, "მსახიობი `cast:` სიის შიგნით უნდა ეწეროს.");
        current.cast.push(parseScalar(match[1], lineNumber));
        continue;
      }

      fail(lineNumber, "ამ YAML-like ფორმატს ეს ჩანაწერი არ ესმის.");
    }

    if (!sawRoot) fail(0, "ფაილი `movies:`-ით უნდა იწყებოდეს.");
    if (!movies.length) fail(0, "დაამატეთ ერთი ფილმი მაინც.");
    if (movies.length > MAX_MOVIES) fail(0, `ნებადართულია მაქსიმუმ ${MAX_MOVIES} ფილმი.`);

    const printableAscii = /^[\x20-\x7e]+$/;
    const filmKeys = new Set();
    const actorNames = new Set();
    for (const movie of movies) {
      if (!printableAscii.test(movie.title)) {
        fail(movie.line, "სათაური მხოლოდ printable ASCII სიმბოლოებს უნდა შეიცავდეს.");
      }
      if (movie.title.length > MAX_TEXT_BYTES) {
        fail(movie.line, `სათაური მაქსიმუმ ${MAX_TEXT_BYTES} ბაიტი შეიძლება იყოს.`);
      }
      if (!Number.isInteger(movie.year) || movie.year < 1900 || movie.year > 2155) {
        fail(movie.line, "წელი 1900-დან 2155-მდე უნდა იყოს.");
      }
      const filmKey = `${movie.title}\u0000${movie.year}`;
      if (filmKeys.has(filmKey)) fail(movie.line, "ასეთი სათაურისა და წლის ფილმი უკვე არსებობს.");
      filmKeys.add(filmKey);

      const localActors = new Set();
      for (const actor of movie.cast) {
        if (!printableAscii.test(actor)) fail(movie.line, "მსახიობის სახელი მხოლოდ printable ASCII უნდა იყოს.");
        if (actor.length > MAX_TEXT_BYTES) {
          fail(movie.line, `მსახიობის სახელი მაქსიმუმ ${MAX_TEXT_BYTES} ბაიტი შეიძლება იყოს.`);
        }
        if (localActors.has(actor)) fail(movie.line, `მსახიობი “${actor}” cast-ში მეორდება.`);
        localActors.add(actor);
        actorNames.add(actor);
      }
    }
    if (actorNames.size > MAX_ACTORS) fail(0, `ნებადართულია მაქსიმუმ ${MAX_ACTORS} მსახიობი.`);

    return {movies: movies.map(({title, year, cast}) => ({title, year, cast: [...cast]}))};
  }

  function compareText(a, b) {
    return a < b ? -1 : a > b ? 1 : 0;
  }

  function filmKey(movie) {
    return `${movie.title}\u0000${movie.year}`;
  }

  function normalizeModel(model) {
    const movies = model.movies.map((movie) => ({
      title: movie.title,
      year: movie.year,
      cast: [...movie.cast].sort(compareText),
    })).sort((a, b) => compareText(a.title, b.title) || a.year - b.year);

    const credits = new Map();
    for (const movie of movies) {
      for (const actor of movie.cast) {
        if (!credits.has(actor)) credits.set(actor, []);
        credits.get(actor).push(movie);
      }
    }
    const actors = [...credits.keys()].sort(compareText).map((name) => ({
      name,
      movies: credits.get(name),
    }));
    return {movies, actors};
  }

  function align(value, boundary) {
    return Math.ceil(value / boundary) * boundary;
  }

  function actorRecordSize(actor) {
    let size = actor.name.length + 1;
    size = align(size, 2);
    size += 2;
    size = align(size, 4);
    return size + actor.movies.length * 4;
  }

  function movieRecordSize(movie) {
    let size = movie.title.length + 1;
    size += 1;
    size = align(size, 2);
    size += 2;
    size = align(size, 4);
    return size + movie.cast.length * 4;
  }

  function calculateOffsets(records, sizeOf) {
    let cursor = 4 + records.length * 4;
    const offsets = new Map();
    for (const record of records) {
      offsets.set(record, cursor);
      cursor += sizeOf(record);
    }
    return {offsets, size: cursor};
  }

  function makeWriter(size, littleEndian, fileName) {
    const bytes = new Uint8Array(size);
    const view = new DataView(bytes.buffer);
    const segments = [];
    let nextId = 0;

    function segment(start, length, type, label, description, options) {
      if (!length) return null;
      const value = options || {};
      const item = {
        id: `${fileName}:${nextId++}`,
        fileName,
        start,
        length,
        type,
        label,
        description,
        decoded: value.decoded,
        targetRecord: value.targetRecord || null,
        recordKey: value.recordKey || null,
      };
      segments.push(item);
      return item;
    }

    function uint8(position, value, metadata) {
      view.setUint8(position, value);
      segment(position, 1, metadata.type, metadata.label, metadata.description, metadata);
    }

    function uint16(position, value, metadata) {
      view.setUint16(position, value, littleEndian);
      segment(position, 2, metadata.type, metadata.label, metadata.description, metadata);
    }

    function uint32(position, value, metadata) {
      view.setUint32(position, value, littleEndian);
      segment(position, 4, metadata.type, metadata.label, metadata.description, metadata);
    }

    function cstring(position, value, metadata) {
      for (let i = 0; i < value.length; ++i) bytes[position + i] = value.charCodeAt(i);
      bytes[position + value.length] = 0;
      segment(position, value.length + 1, metadata.type, metadata.label, metadata.description, metadata);
      return position + value.length + 1;
    }

    function padding(position, length, recordKey, reason) {
      segment(position, length, "padding", "padding", reason, {recordKey});
      return position + length;
    }

    return {bytes, segments, uint8, uint16, uint32, cstring, padding};
  }

  function finishFile(name, writer) {
    const segmentAt = new Array(writer.bytes.length);
    for (const segment of writer.segments) {
      for (let offset = segment.start; offset < segment.start + segment.length; ++offset) {
        segmentAt[offset] = segment;
      }
    }
    return {name, bytes: writer.bytes, segments: writer.segments, segmentAt};
  }

  function serializeFileHeader(writer, records, offsets, kind) {
    writer.uint32(0, records.length, {
      type: "count",
      label: `${kind} count`,
      decoded: records.length,
      description: `ფაილში ${records.length} ${kind === "actor" ? "მსახიობის" : "ფილმის"} ჩანაწერია.`,
    });
    records.forEach((record, index) => {
      const key = kind === "actor" ? `actor:${record.name}` : `movie:${filmKey(record)}`;
      const offset = offsets.get(record);
      writer.uint32(4 + index * 4, offset, {
        type: "table-offset",
        label: `${kind} offset ${index}`,
        decoded: offset,
        targetRecord: key,
        description: `offset ${offset} ფაილის დასაწყისიდან შესაბამის ჩანაწერზე მიუთითებს.`,
      });
    });
  }

  function buildDatabase(model, endian) {
    const normalized = normalizeModel(model);
    const actorLayout = calculateOffsets(normalized.actors, actorRecordSize);
    const movieLayout = calculateOffsets(normalized.movies, movieRecordSize);
    if (actorLayout.size > MAX_FILE_BYTES || movieLayout.size > MAX_FILE_BYTES) {
      fail(0, `სასწავლო მაგალითის თითოეული ფაილი მაქსიმუმ ${MAX_FILE_BYTES} ბაიტი შეიძლება იყოს.`);
    }
    const littleEndian = endian === "little";
    const actorWriter = makeWriter(actorLayout.size, littleEndian, "actordata");
    const movieWriter = makeWriter(movieLayout.size, littleEndian, "moviedata");

    const actorOffsetByName = new Map(normalized.actors.map((actor) => [actor.name, actorLayout.offsets.get(actor)]));
    const movieOffsetByKey = new Map(normalized.movies.map((movie) => [filmKey(movie), movieLayout.offsets.get(movie)]));

    serializeFileHeader(actorWriter, normalized.actors, actorLayout.offsets, "actor");
    for (const actor of normalized.actors) {
      const recordKey = `actor:${actor.name}`;
      const start = actorLayout.offsets.get(actor);
      let cursor = actorWriter.cstring(start, actor.name, {
        type: "string",
        label: `name: ${actor.name}`,
        decoded: actor.name,
        recordKey,
        description: "მსახიობის null-ით დასრულებული C-string.",
      });
      const alignedName = align(cursor, 2);
      cursor = actorWriter.padding(cursor, alignedName - cursor, recordKey,
          "movieCount 2-ბაიტიან საზღვარზე უნდა დაიწყოს.");
      actorWriter.uint16(cursor, actor.movies.length, {
        type: "count",
        label: "movieCount",
        decoded: actor.movies.length,
        recordKey,
        description: `${actor.name}-ის ფილმების რაოდენობა.`,
      });
      cursor += 2;
      const alignedOffsets = align(cursor, 4);
      cursor = actorWriter.padding(cursor, alignedOffsets - cursor, recordKey,
          "ფილმების offset-ები 4-ბაიტიან საზღვარზე უნდა დაიწყოს.");
      actor.movies.forEach((movie, index) => {
        const target = `movie:${filmKey(movie)}`;
        const value = movieOffsetByKey.get(filmKey(movie));
        actorWriter.uint32(cursor, value, {
          type: "cross-offset",
          label: `movie offset ${index}`,
          decoded: value,
          targetRecord: target,
          recordKey,
          description: `moviedata + ${value} მიუთითებს “${movie.title}” (${movie.year}) ჩანაწერზე.`,
        });
        cursor += 4;
      });
    }

    serializeFileHeader(movieWriter, normalized.movies, movieLayout.offsets, "movie");
    for (const movie of normalized.movies) {
      const key = filmKey(movie);
      const recordKey = `movie:${key}`;
      const start = movieLayout.offsets.get(movie);
      let cursor = movieWriter.cstring(start, movie.title, {
        type: "string",
        label: `title: ${movie.title}`,
        decoded: movie.title,
        recordKey,
        description: "ფილმის null-ით დასრულებული C-string.",
      });
      const yearOffset = movie.year - 1900;
      movieWriter.uint8(cursor, yearOffset, {
        type: "year",
        label: "year - 1900",
        decoded: `${yearOffset} → ${movie.year}`,
        recordKey,
        description: `ერთი unsigned ბაიტი: ${movie.year} - 1900 = ${yearOffset}.`,
      });
      cursor += 1;
      const alignedCount = align(cursor, 2);
      cursor = movieWriter.padding(cursor, alignedCount - cursor, recordKey,
          "castCount 2-ბაიტიან საზღვარზე უნდა დაიწყოს.");
      movieWriter.uint16(cursor, movie.cast.length, {
        type: "count",
        label: "castCount",
        decoded: movie.cast.length,
        recordKey,
        description: `“${movie.title}” (${movie.year}) ფილმის მსახიობების რაოდენობა.`,
      });
      cursor += 2;
      const alignedOffsets = align(cursor, 4);
      cursor = movieWriter.padding(cursor, alignedOffsets - cursor, recordKey,
          "მსახიობების offset-ები 4-ბაიტიან საზღვარზე უნდა დაიწყოს.");
      movie.cast.forEach((actor, index) => {
        const value = actorOffsetByName.get(actor);
        movieWriter.uint32(cursor, value, {
          type: "cross-offset",
          label: `actor offset ${index}`,
          decoded: value,
          targetRecord: `actor:${actor}`,
          recordKey,
          description: `actordata + ${value} მიუთითებს ${actor}-ის ჩანაწერზე.`,
        });
        cursor += 4;
      });
    }

    return {
      model: normalized,
      endian,
      actordata: finishFile("actordata", actorWriter),
      moviedata: finishFile("moviedata", movieWriter),
    };
  }

  function formatByte(value, radix) {
    if (radix === "binary") return value.toString(2).padStart(8, "0");
    if (radix === "decimal") return value.toString(10).padStart(3, "0");
    return value.toString(16).padStart(2, "0");
  }

  function formatAddress(value, radix) {
    if (radix === "binary") return `0b${value.toString(2).padStart(16, "0")}`;
    if (radix === "decimal") return value.toString(10).padStart(4, "0");
    return `0x${value.toString(16).padStart(4, "0")}`;
  }

  function bytesForSegment(file, segment) {
    return file.bytes.slice(segment.start, segment.start + segment.length);
  }

  const Core = {
    DEFAULT_SOURCE,
    parseMovies,
    buildDatabase,
    formatByte,
    formatAddress,
    bytesForSegment,
  };

  global.ImdbByteMapCore = Core;
  if (typeof module !== "undefined" && module.exports) module.exports = Core;
})(typeof globalThis !== "undefined" ? globalThis : window);
