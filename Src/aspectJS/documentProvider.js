export class DocumentProvider {
    /** @type{ApplicationConfiguration} */
    #config; 

    /** @type{Object} */
    #elements
    /**
     *
     * @param {ApplicationConfiguration} config
     */
    constructor(config) {
        this.#config = config;
        this.#elements = {};
    }

    get document(){
         
    }
    /**
     *
     * @returns {*|HTMLElement}
     */
    get canvasElement() {
        return this.getElementWithID(this.#config.canvasElementID);
    }

    /**
     *
     * @param {string}id
     * @returns {HTMLElement|*}
     */
    getElementWithID(id) {
        let cached = this.#elements[id];
        if (cached) {
            return cached;
        }

        cached = document.getElementById(id);
        if (!cached) {
            throw new Error(`expected canvas element with ID ${id} not found`);
        }
        this.#elements[id] = cached;
        return cached;
    }
}
