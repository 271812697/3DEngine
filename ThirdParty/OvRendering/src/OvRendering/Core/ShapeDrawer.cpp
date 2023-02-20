/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Core/ShapeDrawer.h"
#include "OvRendering/Resources/Loaders/ShaderLoader.h"

OvRendering::Core::ShapeDrawer::ShapeDrawer(OvRendering::Core::Renderer& p_renderer) : m_renderer(p_renderer)
{
	std::vector<Geometry::Vertex> vertices;
	vertices.push_back
	({
		0, 0, 0,
		0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	});
	vertices.push_back
	({
		0, 0, 0,
		0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	});

	m_lineMesh = new Resources::Mesh(vertices, { 0, 1 }, 0);

	std::string vertexShader = R"(
#version 430 core

uniform vec3 start;
uniform vec3 end;
uniform mat4 viewProjection;

void main()
{
	vec3 position = gl_VertexID == 0 ? start : end;
    gl_Position = viewProjection * vec4(position, 1.0);
}

)";

	std::string fragmentShader = R"(
#version 430 core

uniform vec3 color;

out vec4 FRAGMENT_COLOR;

void main()
{
	FRAGMENT_COLOR = vec4(color, 1.0);
}
)";

	m_lineShader = OvRendering::Resources::Loaders::ShaderLoader::CreateFromSource(vertexShader, fragmentShader);

	vertexShader = R"(
#version 430 core

uniform vec3 start;
uniform vec3 end;
uniform mat4 viewProjection;

out vec3 fragPos;

void main()
{
	vec3 position = gl_VertexID == 0 ? start : end;
	fragPos = position;
    gl_Position = viewProjection * vec4(position, 1.0);
}

)";

	fragmentShader = R"(
#version 430 core

uniform vec3 color;
uniform vec3 viewPos;
uniform float linear;
uniform float quadratic;
uniform float fadeThreshold;

out vec4 FRAGMENT_COLOR;

in vec3 fragPos;

float AlphaFromAttenuation()
{
	vec3 fakeViewPos = viewPos;
	fakeViewPos.y = 0;

    const float distanceToLight = max(max(length(viewPos - fragPos) - fadeThreshold, 0) - viewPos.y, 0);
    const float attenuation = (linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
    return 1.0 / attenuation;
}

void main()
{
	FRAGMENT_COLOR = vec4(color, AlphaFromAttenuation());
}
)";

    std::string grid_vertexShader = R"(
#version 460 core
const float scale = 1000.0;
uniform mat4 viewProjection;
uniform vec3 viewPos;
out vec2 _uv;
const vec3 positions[6] = vec3[] (
    vec3(-1, 0, -1), vec3(-1, 0, 1), vec3(1, 0, 1),
    vec3(1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1)
);

void main() {
    uint index = viewPos.y >= 0 ? gl_VertexID : (5 - gl_VertexID);  // reverse winding order when y < 0
    vec3 position = positions[index] * scale;
    gl_Position =  viewProjection*vec4(position, 1.0);
    _uv = position.xz;  // limit the grid to the X-Z plane (y == 0)
}
)";

    std::string grid_fragmentShader = R"(
#version 460 core
const float scale = 100.0;
const float lod_floor = 8.0;  // minimum number of pixels between lines before LOD could switch
const vec4 x_axis_color = vec4(220, 20, 60, 255) / 255.0;
const vec4 z_axis_color = vec4(0, 46, 255, 255) / 255.0;
uniform float cell_size = 1.0;
uniform vec4 thin_line_color = vec4(vec3(1.0,0.0,0.0), 1.0);
uniform vec4 wide_line_color = vec4(vec3(0.0,1.0,1.0), 1.0);  // every 10th line is thick
#define INV_LN10 0.434294481903252  // 1 over ln10
#define clamp01(x) clamp(x, 0.0, 1.0)
float log10(float x) { return log(x) * INV_LN10; }
float max2(const vec2 v) { return max(v.x, v.y); }
vec2 step3(const vec2 a, const vec2 x, const vec2 b) { return step(a, x) - step(b, x); }
in vec2 _uv;
out vec4 color;
void main() {
    // higher derivative = farther cell = smaller LOD = less details = more transparent
    vec2 derivative = fwidth(_uv);
    float lod = max(0.0, log10(length(derivative) * lod_floor / cell_size) + 1.0);
    float fade = fract(lod);

    // cell size at LOD level 0, 1 and 2, each higher level is 10 times larger
    float cell_size_0 = cell_size * pow(10.0, floor(lod));
    float cell_size_1 = cell_size_0 * 10.0;
    float cell_size_2 = cell_size_1 * 10.0;

    derivative *= 4.0;  // each anti-aliased line covers up to 4 pixels

    // compute absolute distance to cell line centers for each LOD and pick max x/y to be the alpha
    // alpha_0 >= alpha_1 >= alpha_2
    float alpha_0 = max2(1.0 - abs(clamp01(mod(_uv, cell_size_0) / derivative) * 2.0 - 1.0));
    float alpha_1 = max2(1.0 - abs(clamp01(mod(_uv, cell_size_1) / derivative) * 2.0 - 1.0));
    float alpha_2 = max2(1.0 - abs(clamp01(mod(_uv, cell_size_2) / derivative) * 2.0 - 1.0));

    // line margins can be used to check where the current line is (e.g. x = 0, or y = 3, etc)
    vec2 margin = min(derivative, 1.0);
    vec2 basis = step3(vec2(0.0), _uv, margin);

    // blend between falloff colors to handle LOD transition and highlight world axis X and Z
    vec4 c = alpha_2 > 0.0
        ? (basis.y > 0.0 ? x_axis_color : (basis.x > 0.0 ? z_axis_color : wide_line_color))
        : (alpha_1 > 0.0 ? mix(wide_line_color, thin_line_color, fade) : thin_line_color);

    // calculate opacity falloff based on distance to grid extents
    float opacity_falloff = 1.0 - clamp01(length(_uv) / scale);

    // blend between LOD level alphas and scale with opacity falloff
    c.a *= (alpha_2 > 0.0 ? alpha_2 : alpha_1 > 0.0 ? alpha_1 : (alpha_0 * (1.0 - fade))) * opacity_falloff;
    color = c;
};
)";

	m_gridShader = OvRendering::Resources::Loaders::ShaderLoader::CreateFromSource(grid_vertexShader, grid_fragmentShader);
}

OvRendering::Core::ShapeDrawer::~ShapeDrawer()
{
	delete m_lineMesh;
	OvRendering::Resources::Loaders::ShaderLoader::Destroy(m_lineShader);
}

void OvRendering::Core::ShapeDrawer::SetViewProjection(const OvMaths::FMatrix4& p_viewProjection)
{
	m_lineShader->Bind();
	m_lineShader->SetUniformMat4("viewProjection", p_viewProjection);
	m_lineShader->Unbind();

	m_gridShader->Bind();
	m_gridShader->SetUniformMat4("viewProjection", p_viewProjection);
	m_gridShader->Unbind();
}

void OvRendering::Core::ShapeDrawer::DrawLine(const OvMaths::FVector3& p_start, const OvMaths::FVector3& p_end, const OvMaths::FVector3& p_color, float p_lineWidth)
{
	m_lineShader->Bind();

	m_lineShader->SetUniformVec3("start", p_start);
	m_lineShader->SetUniformVec3("end", p_end);
	m_lineShader->SetUniformVec3("color", p_color);

	m_renderer.SetRasterizationMode(OvRendering::Settings::ERasterizationMode::LINE);
	m_renderer.SetRasterizationLinesWidth(p_lineWidth);
	m_renderer.Draw(*m_lineMesh, Settings::EPrimitiveMode::LINES);
	m_renderer.SetRasterizationLinesWidth(1.0f);
	m_renderer.SetRasterizationMode(OvRendering::Settings::ERasterizationMode::FILL);

	m_lineShader->Unbind();
}

void OvRendering::Core::ShapeDrawer::DrawGrid(const OvMaths::FVector3& p_viewPos, const OvMaths::FVector3& p_color, int32_t p_gridSize, float p_linear, float p_quadratic, float p_fadeThreshold, float p_lineWidth)
{
    m_gridShader->Bind();
    m_gridShader->SetUniformVec3("viewPos", p_viewPos);
    m_renderer.SetCapability(OvRendering::Settings::ERenderingCapability::BLEND, true);
    m_lineMesh->Bind();
    glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);  
    m_lineMesh->Unbind();


    m_renderer.SetCapability(OvRendering::Settings::ERenderingCapability::BLEND, false);
    m_gridShader->Unbind();
	
    /*
    m_gridShader->Bind();
    m_gridShader->SetUniformVec3("color", p_color);
	m_gridShader->SetUniformVec3("viewPos", p_viewPos);
	m_gridShader->SetUniformFloat("linear", p_linear);
	m_gridShader->SetUniformFloat("quadratic", p_quadratic);
	m_gridShader->SetUniformFloat("fadeThreshold", p_fadeThreshold);

	m_renderer.SetRasterizationMode(OvRendering::Settings::ERasterizationMode::LINE);
	m_renderer.SetRasterizationLinesWidth(p_lineWidth);
	m_renderer.SetCapability(OvRendering::Settings::ERenderingCapability::BLEND, true);

	for (int32_t i = -p_gridSize + 1; i < p_gridSize; ++i)
	{
		m_gridShader->SetUniformVec3("start", { -(float)p_gridSize + std::floor(p_viewPos.x), 0.f, (float)i + std::floor(p_viewPos.z) });
		m_gridShader->SetUniformVec3("end", { (float)p_gridSize + std::floor(p_viewPos.x), 0.f, (float)i + std::floor(p_viewPos.z) });
		m_renderer.Draw(*m_lineMesh, Settings::EPrimitiveMode::LINES);

		m_gridShader->SetUniformVec3("start", { (float)i + std::floor(p_viewPos.x), 0.f, -(float)p_gridSize + std::floor(p_viewPos.z) });
		m_gridShader->SetUniformVec3("end", { (float)i + std::floor(p_viewPos.x), 0.f, (float)p_gridSize + std::floor(p_viewPos.z) });
		m_renderer.Draw(*m_lineMesh, Settings::EPrimitiveMode::LINES);
	}

	m_renderer.SetCapability(OvRendering::Settings::ERenderingCapability::BLEND, false);
	m_renderer.SetRasterizationLinesWidth(1.0f);
	m_renderer.SetRasterizationMode(OvRendering::Settings::ERasterizationMode::FILL);
    
    	m_gridShader->Unbind();
    */


}