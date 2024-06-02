export default class Logger {
    static #default;

    debug(message, context) {
        console.log(message, context);
    }

    static getDefaultLogger() {
        if (this.#default) {
            return this.#default;
        }
        this.#default = new Logger();
        return this.#default;
    }
}