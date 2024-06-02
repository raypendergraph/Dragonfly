
import {Application, ApplicationConfiguration} from "./aspectJS/application.js";
import {DocumentProvider} from "./aspectJS/documentProvider.js";
import {UserInterfaceController} from "./aspectJS/userInterfaceController.js";
import { Scene } from "./aspectJS/scene.js";
import { Renderer } from "./aspectJS/renderer.js";
import Logger from "./aspectJS/Logger.js";
var logger = new Logger();

const config = new ApplicationConfiguration({})
const scene = new Scene();
const documentProvider = new DocumentProvider(config);
const renderer = new Renderer(documentProvider.canvasElement);
const userInterface = new UserInterfaceController(
    undefined,
    documentProvider.canvasElement);
userInterface.start();
const app = new Application(config, renderer, scene, documentProvider);
await app.initialize();
app.start();
app.run();