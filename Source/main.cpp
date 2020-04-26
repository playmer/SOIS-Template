#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <array>
#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include "SOIS/ImGuiSample.hpp"
#include "SOIS/ApplicationContext.hpp"

#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_internal.h"


// Returns float between 0 and 1, inclusive;
float Rand()
{
  return ((float)rand() / (RAND_MAX));
}

ImVec2 ToImgui(glm::vec2 vec)
{
    return ImVec2(vec.x, vec.y);
}

class FancyPoint
{
public:
    glm::vec2 mPos;
    glm::vec2 mVelocity;
    glm::vec3 mColor;
    float mRadius = 2.f;

    FancyPoint(glm::vec2 pos, float r = 2.f, glm::vec3 c = glm::vec3(1.f, 1.f, 1.f), glm::vec2 velocity = glm::vec2(1, 1))
    {
        mPos = pos;
        mRadius = r;
        mColor = c;
        mVelocity = velocity;
    }

    bool IsOutCanvas()
    {
        ImGuiIO& io = ImGui::GetIO();
        glm::vec2 p = mPos;
        float w = io.DisplaySize.x;
        float h = io.DisplaySize.y;
        return (p.x > w || p.y > h || p.x < 0 || p.y < 0);
    }

    void update()
    {
        mPos.x += mVelocity.x;
        mPos.y += mVelocity.y;

        //mVelocity.x *= (Rand() > .01) ? 1 : -1;
        //mVelocity.y *= (Rand() > .01) ? 1 : -1;
        if (IsOutCanvas())
        {
            ImGuiIO& io = ImGui::GetIO();

            mPos = glm::vec2(Rand() * io.DisplaySize.x, Rand() * io.DisplaySize.y);
            mVelocity.x *= Rand() > .5 ? 1 : -1;
            mVelocity.y *= Rand() > .5 ? 1 : -1;
        }
    }

    void draw() const
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddCircleFilled(ImVec2(mPos.x, mPos.y), mRadius, ImColor{mColor.r, mColor.g, mColor.b});
    }

};

constexpr float cMinDistance = 73.f;

std::vector<FancyPoint> InitPoints()
{
    std::vector<FancyPoint> points;
    

    ImGuiIO& io = ImGui::GetIO();
    float canvasWidth = io.DisplaySize.x;
    float canvasHeight = io.DisplaySize.y;

    for (size_t i = 0; i < 100; i++)
    {
        points.emplace_back(glm::vec2(Rand() * canvasWidth, Rand() * canvasHeight), 3.4f, glm::vec3(1,0,0), glm::vec2(Rand()>.5?Rand():-Rand(), Rand()>.5?Rand():-Rand()));
    }

    return std::move(points);
}

void DrawPoints(std::vector<FancyPoint>& points)
{
    for (auto const& point1 : points)
    {
        for (auto const& point2 : points)
        {
            if ((&point1 != &point2) && (glm::distance(point1.mPos, point2.mPos) <= cMinDistance))
            {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddLine(ToImgui(point1.mPos), ToImgui(point2.mPos), ImColor(255, 255, 255));
            }
        }
    }
    
    // Draw the points separately to make them draw on top
    for (auto const& point1 : points)
    {
        point1.draw();
    }
}

void UpdatePoints(std::vector<FancyPoint>& points)
{
    ImGuiIO& io = ImGui::GetIO();

    glm::vec2 mouse = glm::vec2(io.MousePos.x, io.MousePos.y);

    for (auto& point : points)
    {
        point.update();

        if (glm::distance(mouse, point.mPos) < 100.f)
        {
            auto direction = glm::normalize(point.mPos - mouse);
        
            point.mPos = mouse + (direction * 105.f);
        }
    }
}

std::string GetImGuiIniPath()
{
    auto sdlIniPath = SDL_GetPrefPath("PlaymerTools", "PadInput");

    std::filesystem::path path{ sdlIniPath };
    SDL_free(sdlIniPath);

    path /= "imgui.ini";

    return path.u8string();
}

int main(int, char**)
{
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    SOIS::ApplicationInitialization();
  
    auto iniPath = GetImGuiIniPath();

    SOIS::ApplicationContextConfig config;
    config.aBlocking = false;
    config.aIniFile = iniPath.c_str();
    config.aWindowName = "SOIS Template";

    SOIS::ApplicationContext context{config};
    //SOIS::ImGuiSample sample;

    std::vector<FancyPoint> points;

    while (context.Update())
    {
        ImGui::Begin(
            "Canvas", 
            nullptr, 
            ImGuiWindowFlags_NoBackground | 
            ImGuiWindowFlags_NoBringToFrontOnFocus | 
            ImGuiWindowFlags_NoCollapse |  
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoInputs | 
            ImGuiWindowFlags_NoMove);
        {
            static bool firstRun = true;

            if (firstRun)
            {
                points = InitPoints();
                firstRun = false;
            }

            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
            ImGui::SetWindowPos(ImVec2(0, 0));

            DrawPoints(points);
        }
        ImGui::End();

        UpdatePoints(points);

        //sample.Update();
    }

    return 0;
}
