import * as libansilove from "libansilove/browser";
import React, { useMemo } from "react";

export async function ansilove(_: { ansi: string }): Promise<string> {
	const ansilove = await libansilove.loadBrowser();

	const { png } = ansilove.renderAnsi(_.ansi);
	const blob = new Blob([png], { type: "image/png" });
	return URL.createObjectURL(blob);
}

export default function Ansiove(props: { ansi: string }) {
	const urlPromise = useMemo(
		() => ansilove({ ansi: props.ansi }),
		[props.ansi],
	);
	return <LazyImg srcPromise={urlPromise} alt="Rendered ANSI art" />;
}

export function LazyImg(
	props: {
		srcPromise: Promise<string>;
	} & React.ImgHTMLAttributes<HTMLImageElement>,
) {
	return <img alt="ANSI ART" {...props} src={React.use(props.srcPromise)} />;
}
