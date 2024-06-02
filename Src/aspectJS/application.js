import { DocumentProvider } from "./documentProvider.js";
import { Renderer } from "./renderer.js";
import { Scene } from "./scene.js";

const CANVAS_ELEMENT_ID = "df-main"

export class ApplicationConfiguration {
    /**
     *
     * @param {Object}object
     */
    constructor(object) {

    }

    get canvasElementID() {
        return CANVAS_ELEMENT_ID;
    }
}

export class Application {

    /** @type{Scene} */
    #scene;
    /** @type{Renderer} */
    #renderer;
    /** @type{DocumentProvider} */
    #document;

    /** @type{HTMLElement} */
    #keyLabel;

    /** @type{HTMLElement} */
    #mouseXLabel;

    /** @type{HTMLElement} */
    #mouseYLabel;

    /** @type{number} */
    #forwardAmount;
    /** @type{number} */
    #rightAmount;

    /**
     * 
     * @param {ApplicationConfiguration} config 
     * @param {Renderer} renderer 
     * @param {Scene} sceneProvider 
     * @param {DocumentProvider} documentProvider 
     */
    constructor(config, renderer, scene, documentProvider) {
        this.#scene = scene;
        this.#renderer = renderer;
        this.#document = documentProvider;
        this.run = this.run.bind(this);
    }
    handleKeyDown(event) {
        this.#keyLabel.innerText = event.code;

        if (event.code == "ArrowUp") {
            this.#forwardAmount = 0.02;
        }
        if (event.code == "ArrowDown") {
            this.#forwardAmount = -0.02;
        }
        if (event.code == "ArrowLeft") {
            this.#rightAmount = -0.02;
        }
        if (event.code == "ArrowRight") {
            this.#rightAmount = 0.02;
        }
    }

    handleKeyUp(event) {
        this.#keyLabel.innerText = event.code;

        if (event.code == "ArrowUp") {
            this.#forwardAmount = 0;
        }
        if (event.code == "ArrowDown") {
            this.#forwardAmount = 0;
        }
        if (event.code == "ArrowLeft") {
            this.#rightAmount = 0;
        }
        if (event.code == "ArrowRight") {
            this.#rightAmount = 0;
        }
    }

    handleMouseMove(event) {
        this.#mouseXLabel.innerText = event.clientX.toString();
        this.#mouseYLabel.innerText = event.clientY.toString();
        this.#scene.spinCamera( event.movementX / 5, event.movementY / 5);
    }

    async initialize(){
        await this.#renderer.initialize();
    }

    start() {
        this.#forwardAmount = 0;
        this.#rightAmount = 0;
        this.#renderer.start();
        this.#scene = new Scene();
        this.#keyLabel = document.getElementById("key-label");
        this.#mouseXLabel = this.#document.getElementWithID("mouse-x-label");
        this.#mouseYLabel = this.#document.getElementWithID("mouse-y-label");


        document.onkeydown = (event) => this.handleKeyDown(event);
        document.onkeyup= (event) => this.handleKeyUp(event);
        const canvas = this.#document.canvasElement;
        canvas.onclick = () => canvas.requestPointerLock();
        canvas.addEventListener("mousemove", (event) => this.handleMouseMove(event));
    }
    run(){
        var running= true;
        this.#scene.update();
        this.#scene.movePlayer(this.#forwardAmount, this.#rightAmount);
        this.#renderer.render(
            this.#scene.getCamera(),
            this.#scene.objectData,
            this.#scene.objectCount
        );

        if (running) {
            requestAnimationFrame(this.run);
        }
    }
    stop() {

    }
}