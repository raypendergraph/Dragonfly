import {mat4, vec3} from "gl-matrix";
import {degreesToRadians} from "./math.js";

export class Camera {

    /** @type {vec3} */
    #position;
    /** @type {vec3} */
    eulers;
    /** @type {mat4} */
    #view;
    /** @type {vec3} */
    #forwards;
    /** @type {vec3} */
    #right;
    /** @type {vec3} */
    #up;

    /**
     *
     * @param {vec3}position
     * @param {number}theta
     * @param {number}phi
     */
    constructor(position, theta, phi) {
        this.#position = position;
        this.eulers = [0, phi, theta];
        this.#forwards = vec3.create();
        this.#right = vec3.create();
        this.#up = vec3.create();
    }

    /**
     *
     * @returns {vec3}
     */
    get right() {
        return this.#right;
    }

    /**
     *
     * @returns {vec3}
     */
    get forward() {
        return this.#forwards;
    }

    /**
     *
     * @returns {vec3}
     */
    get position() {
        return this.#position;
    }

    update() {
        this.#forwards = [
            Math.cos(degreesToRadians(this.eulers[2])) * Math.cos(degreesToRadians(this.eulers[1])),
            Math.sin(degreesToRadians(this.eulers[2])) * Math.cos(degreesToRadians(this.eulers[1])),
            Math.sin(degreesToRadians(this.eulers[1]))
        ];

        vec3.cross(this.#right, this.#forwards, [0, 0, 1]);

        vec3.cross(this.#up, this.#right, this.#forwards);

        var target = vec3.create();
        vec3.add(target, this.#position, this.#forwards);

        this.#view = mat4.create();
        mat4.lookAt(this.#view, this.#position, target, this.#up);
    }

    /**
     *
     * @returns {mat4}
     */
    get view() {
        return this.#view;
    }
}