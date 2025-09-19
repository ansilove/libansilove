import { describe, expect, it } from "bun:test";
import * as libansilove from "../src/index";

const SAMPLE = "\u001b[1;36mHello from libansilove!\u001b[0m\r\n";

const example = {
	ans: Bun.file(`${__dirname}/CL!-AL07.ANS`),
	png: Bun.file(`${__dirname}/CL!-AL07.ANS.png`),
};

describe("libansilove wasm bindings", () => {
	it("renders ANSI art to PNG", async () => {
		const bindings = await libansilove.load();
		const { png } = bindings.renderAnsi(SAMPLE);
		expect(png.byteLength).toBeGreaterThan(8);
	});

	it("renders the same as ansilove", async () => {
		const bindings = await libansilove.load();
		const { png } = bindings.renderAnsi(await example.ans.arrayBuffer());
		const control = await example.png.arrayBuffer();
		expect(png).toEqual(new Uint8Array(control));
	});
});
