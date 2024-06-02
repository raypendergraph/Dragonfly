const
    UI_CLICK = "click",
    UI_DBLCLICK = "dblclick",
    UI_MOUSEDOWN = "mousedown",
    UI_MOUSEUP = "mouseup",
    UI_MOUSEOVER = "mouseover",
    UI_MOUSEMOVE = "mousemove",
    UI_MOUSEOUT = "mouseout",
    UI_KEYDOWN = "keydown",
    UI_KEYPRESS = "keypress",
    UI_KEYUP = "keyup",
    UI_FOCUS = "focus",
    UI_BLUR = "blur",
    UI_CHANGE = "change",
    UI_INPUT = "input",
    UI_SUBMIT = "submit",
    UI_RESIZE = "resize",
    UI_SCROLL = "scroll",
    UI_WHEEL = "wheel",
    UI_CONTEXTMENU = "contextmenu",
    UI_DRAG = "drag",
    UI_DRAGSTART = "dragstart",
    UI_DRAGEND = "dragend",
    UI_DRAGOVER = "dragover",
    UI_DRAGENTER = "dragenter",
    UI_DRAGLEAVE = "dragleave",
    UI_DROP = "drop",
    UI_TOUCHSTART = "touchstart",
    UI_TOUCHMOVE = "touchmove",
    UI_TOUCHEND = "touchend",
    UI_TOUCHCANCEL = "touchcancel",
    UI_POINTERDOWN = "pointerdown",
    UI_POINTERUP = "pointerup",
    UI_POINTERMOVE = "pointermove",
    UI_POINTEROVER = "pointerover",
    UI_POINTEROUT = "pointerout",
    UI_POINTERENTER = "pointerenter",
    UI_POINTERLEAVE = "pointerleave",
    UI_POINTERCANCEL = "pointercancel"

/**
 * Event handler callback type.
 *
 * @callback EventHandler
 * @param {Event} event - The event object.
 */

export class UserInterfaceController {
    /**
     *
     * @param {InputSettings}inputSettings
     * @param {Element}applicationElement
     */
    constructor(inputSettings, applicationElement) {
        this._inputSettings = inputSettings;
        this._applicationElement = applicationElement;
    }

    /**
     * @param {Event}event
     */
    #handleEvent(event) {
        console.warn(event)
    }

    start() {
        //this._inputSettings.start();
        this._applicationElement.addEventListener("click", this.#handleEvent);
    }

    stop() {

    }

    /**
     *
     * @param {string}eventKey
     * @param {EventHandler}handler
     */
    subscribe(eventKey, handler) {
        this._applicationElement.addEventListener()
    }
}