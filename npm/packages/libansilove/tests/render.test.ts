import { describe, expect, it } from "bun:test";
import { Buffer } from "node:buffer";
import { PNG } from "pngjs";
import load, { AnsiloveRangeError } from "../src/libansilove";
import { libansilove } from "../src/index";

const SAMPLE = "\u001b[1;36mHello from libansilove!\u001b[0m\r\n";

const example = {
	ans: Bun.file(`${__dirname}/CL!-AL07.ANS`),
	png: Bun.file(`${__dirname}/CL!-AL07.ANS.png`),
};

const decodePng = (input: Uint8Array) => {
	const { data, width, height } = PNG.sync.read(Buffer.from(input));
	return {
		width,
		height,
		data: new Uint8Array(data),
	};
};

describe("libansilove wasm bindings", () => {
	it("renders ANSI art to PNG", async () => {
		const bindings = await libansilove.load();
		const { png } = bindings.renderAnsi(SAMPLE);
		expect(png.byteLength).toBeGreaterThan(8);
	});

	it("renders the same as the native library", async () => {
		const bindings = await load();
		const { png } = bindings.renderAnsi(await example.ans.arrayBuffer());
		const control = new Uint8Array(await example.png.arrayBuffer());
		const rendered = decodePng(png);
		const baseline = decodePng(control);
		expect(rendered.width).toBe(baseline.width);
		expect(rendered.height).toBe(baseline.height);
		expect(rendered.data).toEqual(baseline.data);
	});

	it("supports explicit auto column selection", async () => {
		const bindings = await load();
		const { png } = bindings.renderAnsi(SAMPLE, { columns: "auto" });
		expect(png.byteLength).toBeGreaterThan(8);
	});

	it("accepts named render modes", async () => {
		const bindings = await load();
		const { png } = bindings.renderAnsi(SAMPLE, { mode: "ced" });
		expect(png.byteLength).toBeGreaterThan(8);
	});

	it("applies retina scale factors", async () => {
		const bindings = await load();
		const base = decodePng(new Uint8Array(await example.png.arrayBuffer()));
		const { png } = bindings.renderAnsi(await example.ans.arrayBuffer(), {
			scaleFactor: 2,
		});
		const scaled = decodePng(png);
		expect(scaled.width).toBe(base.width * 2);
		expect(scaled.height).toBe(base.height * 2);
	});

	it("maps libansilove error codes to rich errors", async () => {
		const bindings = await load();
		expect(() =>
			bindings.renderAnsi(SAMPLE, { columns: 5000 }),
		).toThrow(AnsiloveRangeError);
	});
});
