import { vec3, mat4 } from "gl-matrix";
import { degreesToRadians } from "./math.js";

export class Transform {
    /** @type{vec3}*/
    position;
    /** @type{vec3} */
    eulers;
    /** @type{mat4} */
    model;

    /**
     * 
     * @param {vec3} position 
     * @param {mat4} theta 
     */
    constructor(position, theta) {
        this.position = position;
        this.eulers = vec3.create();
        this.eulers[2] = theta;
    }

    update() {
        this.eulers[2] += 1;
        this.eulers[2] %= 360;

        this.model = mat4.create();
        mat4.translate(this.model, this.model, this.position);
        mat4.rotateZ(this.model, this.model, degreesToRadians(this.eulers[2]));
    }
}