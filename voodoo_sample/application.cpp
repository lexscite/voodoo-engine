// This file is part of Voodoo Engine.
//
// Voodoo Engine is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Voodoo Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

#include "stdafx.h"

#include "application.h"

#include <voodoo/renderer.h>
#include <voodoo/camera.h>
#include <voodoo/logger.h>
#include <voodoo/model.h>
#include <voodoo/text.h>

namespace voodoo {
// Static members initialization.
std::wstring Application::name_;
std::shared_ptr<Window> Application::window_;
Time* Application::time_;
std::shared_ptr<DirectXManager> Application::directx_;
std::shared_ptr<Scene> Application::scene_;

bool Application::Init(HINSTANCE instance, std::wstring name) {
  name_ = name;
  time_ = Time::Get();

  window_ = std::make_shared<Window>();
  if (!window_->Init(instance, 800, 600, name_)) {
    Log::Error("Failed to initialize window");
    return false;
  }

  directx_ = std::make_shared<DirectXManager>();
  if (!directx_->Init(window_, true, false)) {
    Log::Error("Failed to initialize DirectX");
    return false;
  }

  if (!LoadScene()) {
    Log::Error("Failed to load scene");
    return false;
  } else {
    if (!scene_->Init()) {
      Log::Error("Failed to initialize scene");
      return false;
    }
  }

  return true;
}

int Application::Run() {
  MSG msg;
  memset(&msg, 0, sizeof(msg));
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      time_->Tick();
      CalculateFrameStats();
      if (!Update(0.0f)) {
        // Temporary error exit code
        return 404;
      }
    }
  }

  return static_cast<int>(msg.wParam);
}

std::shared_ptr<DirectXManager> Application::GetDirectX() { return directx_; }

std::shared_ptr<Scene> Application::GetScene() { return scene_; }

bool Application::Update(float delta_time) {
  using namespace std;
  directx_->BeginScene(scene_->GetClearColor());
  vector<shared_ptr<Renderer>> renderers;

  for (auto go : scene_->GetGameObjects()) {
    auto r = go->GetComponent<Renderer>();
    if (r) {
      renderers.push_back(r);
    }
    for (auto c : go->GetComponents()) {
      c->Update();
    }
  }

  for (auto r : renderers) {
    directx_->Render(r);
  }

  //if (!scene_->Update()) {
  //  Log::Info("Failed to update scene");
  //  return false;
  //}

  directx_->EndScene();

  return true;
}

void Application::CalculateFrameStats() {
  static int frame_count = 0;
  static float time_elapsed = 0;

  frame_count++;

  if ((time_->GetTime() - time_elapsed) >= 1) {
    float fps = static_cast<float>(frame_count);
    float ms_per_frame = 1000 / fps;

    // Display frame stats in window caption
    std::wostringstream caption;
    caption.precision(6);
    caption << name_ << " | FPS: " << fps << " Frame time: " << ms_per_frame
            << "ms";
    SetWindowTextW(window_->GetHandle(),
                   static_cast<LPCWSTR>(caption.str().c_str()));

    frame_count = 0;
    time_elapsed++;
  }
}

bool Application::LoadScene() {
  auto scene = std::make_shared<Scene>();

  auto camera = std::make_shared<GameObject>("Camera");
  camera->AddComponent<Camera>();
  camera->GetTransform()->SetPosition(3.75f, 5, 3.75f);
  camera->GetTransform()->SetRotationByDegrees(45, 225, 0);
  scene->AddGameObject(camera);

  auto cube = std::make_shared<GameObject>("Cube");
  cube->AddComponent<Renderer>(
    directx_->GetDevice(),
    directx_->GetDeviceContext());
  cube->AddComponent<Model>(
    "../assets/meshes/cube.mesh",
    "../assets/shaders/default_vs.cso",
    "../assets/shaders/default_ps.cso",
    "../assets/textures/placeholder.png");
  cube->GetTransform()->SetPosition(0, 0, 0);
  scene->AddGameObject(cube);

  auto text = std::make_shared<GameObject>("Text");
  text->AddComponent<Renderer>(
    directx_->GetDevice(),
    directx_->GetDeviceContext());
  text->AddComponent<Text>(
    "Hello World",
    "../assets/shaders/font_vs.cso",
    "../assets/shaders/font_ps.cso",
    "../assets/textures/fonts/consolas.png");
  text->GetTransform()->SetPosition(1, 1, 1);
  text->GetTransform()->SetScale(0.005f);
  scene->AddGameObject(text);

  scene->SetCamera(camera->GetComponent<Camera>());

  scene_ = scene;

  return true;
}
}  // namespace voodoo