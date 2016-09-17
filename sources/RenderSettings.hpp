#ifndef RenderSettings_hpp
#define RenderSettings_hpp

#include "Behaviour.hpp"
#include "RenderSystem.hpp"

class RenderSettings : public Script
{
public:
    InjectClassName(RenderSettings);
    
    bool m_isWireFrameMode      = false;
    bool m_useGammaCorrection   = true;
    
    virtual void OnEditorGUI() override {
        if (ImGui::Checkbox("Wire Frame", &m_isWireFrameMode)) {
            RenderSystem::GetInstance().setWireFrameMode(m_isWireFrameMode);
        }
        if (ImGui::Checkbox("Gamma Correction", &m_useGammaCorrection)) {
            RenderSystem::GetInstance().setGammaCorrection(m_useGammaCorrection);
        }
    }
};

#endif // RenderSettings_hpp