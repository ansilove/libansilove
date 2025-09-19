declare const factory: ((overrides?: Record<string, unknown>) => Promise<unknown>) & {
  ready?: Promise<unknown>;
};
export default factory;
