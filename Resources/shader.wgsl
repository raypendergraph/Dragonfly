
@group(0) @binding(0) var<uniform> uTime: f32;

struct VertexInput {
	@location(0) position: vec2<f32>,
	@location(1) color: vec3<f32>,
};

struct VertexOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) color: vec3<f32>,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	let ratio = 640.0 / 480.0;
    var offset = vec2f(-0.6875, -0.463);
    offset += 0.3 * vec2f(cos(uTime), sin(uTime));
	out.position = vec4<f32>(in.position.x + offset.x, (in.position.y + offset.y) * ratio, 0.0, 1.0);
	out.color = in.color;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	// We apply a gamma-correction to the color
	//let corrected_color = pow(in.color, vec3<f32>(2.2));
	return vec4<f32>(in.color, 1.0);
}