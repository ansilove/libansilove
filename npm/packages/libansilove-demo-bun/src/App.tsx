import { loadBrowser } from "libansilove/browser";
import { useEffect, useState } from "react";

import "./index.css";

const SAMPLE_ANSI = `\x1b[1;36mHello from libansilove!\x1b[0m\r\n`;

export default function App() {
	const [pngUrl, setPngUrl] = useState<string | null>(null);
	const [version, setVersion] = useState<string>("");
	const [error, setError] = useState<string | null>(null);

	useEffect(() => {
		let objectUrl: string | null = null;

		(async () => {
			try {
				const ansilove = await loadBrowser();
				setVersion(ansilove.version);

				const { png } = ansilove.renderAnsi(SAMPLE_ANSI);
				const blob = new Blob([png], { type: "image/png" });
				objectUrl = URL.createObjectURL(blob);
				setPngUrl(objectUrl);
			} catch (err) {
				const message = err instanceof Error ? err.message : String(err);
				setError(message);
			}
		})();

		return () => {
			if (objectUrl) {
				URL.revokeObjectURL(objectUrl);
			}
		};
	}, []);

	return (
		<div className="app">
			<h1>Bun + React + libansilove</h1>
			<p>
				Edit <code>src/App.tsx</code> and save to test HMR
			</p>

			<section className="demo">
				<h2>libansilove WebAssembly demo</h2>
				{error ? (
					<p className="error">Failed to render ANSI art: {error}</p>
				) : pngUrl ? (
					<figure>
						<img
							src={pngUrl}
							alt="Rendered ANSI sample"
							className="ansi-preview"
						/>
						<figcaption>Rendered with libansilove {version}</figcaption>
					</figure>
				) : (
					<p>Rendering ANSI art…</p>
				)}
			</section>
		</div>
	);
}
