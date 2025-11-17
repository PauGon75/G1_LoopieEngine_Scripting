#pragma once

#include <string>
#include <imgui.h>


namespace Loopie {

    class ImGuiHelpers
    {
    public:
        static std::string TruncateText(const std::string& text, float width);
        static void TextCentered(const std::string& text, float align = 0.5f);
    };

    enum SplitterMode {
        VERTICAL,
        HORIZONTAL
    };

    class Splitter {
    public:

        Splitter(SplitterMode mode = SplitterMode::HORIZONTAL, float ratio = 0.7f, float size = 4.0f, float rounding = 3.0f, int colorStyle = 23);
        void Place(const ImVec2& avail);

        float GetRatio() const { return m_ratio; }
        void SetRatio(float ratio) { m_ratio = ratio; }
        void GetSizes(const ImVec2& avail, float& sizeFirst, float& sizeSecond) const;
        float GetSplitterSize() const { return m_size; }
        float GetRounding() const { return m_rounding; }
        void SetRounding(float rounding) { m_rounding = rounding; }
        int GetColor() const { return m_colorStyle; }
        void SetColor(int color) { m_colorStyle = color; }
        SplitterMode GetMode() const { return m_mode; }
        void SetMode(SplitterMode mode) { m_mode = mode; }

    private:
        SplitterMode m_mode;
        float m_ratio;
        float m_size;
        float m_rounding;
        int m_colorStyle;
    };

    
}