
/**
 * 
 * @param {*} url 
 * @param {*} bitmapOptions 
 * @returns {ImageBitmap}
 */
export async function fetchImageBitmap(url, bitmapOptions = undefined) {
    const response = await fetch(url);
    if (!response.ok) {
        throw new Error(`Failed to fetch image. Status: ${response.status}`);
    }
    const blob = await response.blob();
    return createImageBitmap(blob, bitmapOptions);
}


/**
 *
 * @param {GPUDevice}device
 * @param {ImageBitmap}bitmap
 * @returns {GPUTexture}
 */
export function createTextureFromBitmap(device, bitmap, descriptor) {

    const textureDescriptor = {
        size: {
            width: bitmap.width,
            height: bitmap.height
        },
        format: "rgba8unorm",
        usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT
    };

    const texture = device.createTexture(textureDescriptor);

    device.queue.copyExternalImageToTexture(
        { source: bitmap },
        { texture: texture },
        textureDescriptor.size
    );

    return texture;
}
export default class Material {
    /** @type {GPUTexture} */
    texture;
    /** @type {GPUTextureView} */
    view;
    /** @type {GPUSampler} */
    sampler;
    /** @type {ImageBitmap} */
    bitmap;

    constructor(texture, view, sampler, bitmap) {
        this.texture = texture;
        this.view = view;
        this.sampler = sampler;
        this.bitmap = bitmap;
    }
    /**
     * 
     * @param {GPUDevice}} device 
     * @param {string} url 
     * @returns {Promise<Material>}
     */
    static async buildFromUrl(device, url) {
        const bitmap = await fetchImageBitmap(url);
        const texture = createTextureFromBitmap(device, bitmap);
        const view = texture.createView({
            format: "rgba8unorm",
            dimension: "2d",
            aspect: "all",
            baseMipLevel: 0,
            mipLevelCount: 1,
            baseArrayLayer: 0,
            arrayLayerCount: 1
        });

        const sampler = device.createSampler({
            addressModeU: "repeat",
            addressModeV: "repeat",
            magFilter: "linear",
            minFilter: "nearest",
            mipmapFilter: "nearest",
            maxAnisotropy: 1
        });

        return new Material(texture, view, sampler, bitmap);
    }
}