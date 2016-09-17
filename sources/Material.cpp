#include "Material.hpp"
#include "imgui/imgui.h"

std::map<std::string, Material::PMaterial> Material::m_builtinMaterial;

void Material::OnEditorGUI() {
    if (ImGui::CollapsingHeader("Material")) {
        auto& uniforms = m_shader->uniforms();
        for (auto& u : uniforms) {
            if (builtinUniformNames.find(u.name) != builtinUniformNames.end()) {
                continue;
            }
            if (u.type == GL_FLOAT) {
                ImGui::SliderFloat(u.name.c_str(), &m_uniforms.floats[u.name], 0, 1);
            } else if (u.type == GL_FLOAT_VEC3) {
                ImGui::InputFloat3(u.name.c_str(), glm::value_ptr(m_uniforms.vec3s[u.name]));
            }
        }
    }
}