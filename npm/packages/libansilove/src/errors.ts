export const AnsiloveErrorCodes = {
	InvalidParam: 1,
	Format: 2,
	Memory: 3,
	Range: 4,
	FileRead: 5,
	FileWrite: 6,
	Gd: 7,
} as const;

export type AnsiloveErrorCode =
	(typeof AnsiloveErrorCodes)[keyof typeof AnsiloveErrorCodes];

export class AnsiloveWasmError extends Error {
	readonly code: number;

	constructor(message: string, code: number) {
		super(message);
		this.code = code;
		this.name = new.target.name;
	}
}

export class AnsiloveInvalidParamError extends AnsiloveWasmError {
	constructor() {
		super("libansilove reported invalid parameters", AnsiloveErrorCodes.InvalidParam);
	}
}

export class AnsiloveFormatError extends AnsiloveWasmError {
	constructor() {
		super("libansilove could not parse the provided ANSI data", AnsiloveErrorCodes.Format);
	}
}

export class AnsiloveMemoryError extends AnsiloveWasmError {
	constructor() {
		super("libansilove ran out of memory while rendering", AnsiloveErrorCodes.Memory);
	}
}

export class AnsiloveRangeError extends AnsiloveWasmError {
	constructor() {
		super("Rendering options were outside the supported range", AnsiloveErrorCodes.Range);
	}
}

export class AnsiloveFileReadError extends AnsiloveWasmError {
	constructor() {
		super("libansilove encountered a file read error", AnsiloveErrorCodes.FileRead);
	}
}

export class AnsiloveFileWriteError extends AnsiloveWasmError {
	constructor() {
		super("libansilove encountered a file write error", AnsiloveErrorCodes.FileWrite);
	}
}

export class AnsiloveGdError extends AnsiloveWasmError {
	constructor() {
		super("libansilove failed while processing image data", AnsiloveErrorCodes.Gd);
	}
}

export const createErrorFromCode = (code: number): AnsiloveWasmError => {
	const positive = Math.abs(code);
	switch (positive) {
		case AnsiloveErrorCodes.InvalidParam:
			return new AnsiloveInvalidParamError();
		case AnsiloveErrorCodes.Format:
			return new AnsiloveFormatError();
		case AnsiloveErrorCodes.Memory:
			return new AnsiloveMemoryError();
		case AnsiloveErrorCodes.Range:
			return new AnsiloveRangeError();
		case AnsiloveErrorCodes.FileRead:
			return new AnsiloveFileReadError();
		case AnsiloveErrorCodes.FileWrite:
			return new AnsiloveFileWriteError();
		case AnsiloveErrorCodes.Gd:
			return new AnsiloveGdError();
		default:
			return new AnsiloveWasmError(
				`libansilove returned unknown error code ${code}`,
				positive,
			);
	}
};

