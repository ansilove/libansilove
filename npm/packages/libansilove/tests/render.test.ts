import { describe, expect, it } from "bun:test";
import { load } from "../src/index";

const SAMPLE = "\u001b[1;36mHello from libansilove!\u001b[0m\r\n";

describe("libansilove wasm bindings", () => {
	it("renders ANSI art to PNG", async () => {
		const bindings = await load();
		const { png } = bindings.renderAnsi(SAMPLE);
		expect(png.byteLength).toBeGreaterThan(8);
	});
});
